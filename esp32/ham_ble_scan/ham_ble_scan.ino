/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "hamster.h"



Hamster hamster;



#define HAMSTER_UUID_RX       "00009001-9c80-11e3-a5e2-0800200c9a66"
#define HAMSTER_UUID_CHAR_RX  "0000900a-9c80-11e3-a5e2-0800200c9a66"
#define HAMSTER_UUID_TX       "0000A000-9c80-11e3-a5e2-0800200c9a66"
#define HAMSTER_UUID_CHAR_TX  "0000a006-9c80-11e3-a5e2-0800200c9a66"



// The remote service we wish to connect to.
static BLEUUID    rx_service_UUID(HAMSTER_UUID_RX); 
static BLEUUID    rx_char_UUID   (HAMSTER_UUID_CHAR_RX);
static BLEUUID    tx_service_UUID(HAMSTER_UUID_TX); 
static BLEUUID    tx_char_UUID   (HAMSTER_UUID_CHAR_TX);




static BLEAddress *p_server_address;
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* p_rx_characteristic;
static BLERemoteCharacteristic* p_tx_characteristic;


void updateLed(void);



static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    
  hamster.parsingPacketSensor(pData);    
}

bool connectToServer(BLEAddress pAddress) 
{
    Serial.print("Forming a connection to ");
    Serial.println(pAddress.toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    // Connect to the remove BLE Server.
    pClient->connect(pAddress);
    Serial.println(" - Connected to server");


    //-- RX 
    //
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRxRemoteService = pClient->getService(rx_service_UUID);

    if (pRxRemoteService == nullptr) {
      Serial.print("Failed to find our rx_service_UUID: ");
      Serial.println(rx_service_UUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    p_rx_characteristic = pRxRemoteService->getCharacteristic(rx_char_UUID);
    if (p_rx_characteristic == nullptr) {
      Serial.print("Failed to find rx_char_UUID: ");
      Serial.println(rx_char_UUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found rx_char_UUID");


    const uint8_t v[]={0x1,0x0};
    p_rx_characteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)v,2,false);
    p_rx_characteristic->registerForNotify(notifyCallback);


    //-- TX 
    //
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pTxRemoteService = pClient->getService(tx_service_UUID);

    if (pTxRemoteService == nullptr) {
      Serial.print("Failed to find our tx_service_UUID: ");
      Serial.println(tx_service_UUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our tx_service_UUID");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    p_tx_characteristic = pTxRemoteService->getCharacteristic(tx_char_UUID);
    if (p_tx_characteristic == nullptr) {
      Serial.print("Failed to find tx_char_UUID: ");
      Serial.println(tx_char_UUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found tx_char_UUID");
    
}


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
{
  void onResult(BLEAdvertisedDevice advertisedDevice) 
  {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getName().compare("Hamster") == 0)
    {
      Serial.println("Hamster Found");
      advertisedDevice.getScan()->stop();

      p_server_address = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;      
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); 
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster

  BLEScanResults foundDevices = pBLEScan->start(30);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  hamster.begin("");
  hamster.uartWrite = bleWrite;
}

void loop() {
  uint8_t rx_buffer[20] = {0, };


  if (doConnect == true) 
  {
    if (connectToServer(*p_server_address)) 
    {
      connected = true;
    } 
    doConnect = false;
  }

  if (connected) 
  {
    updateBleTx();
    updateStatus();    
  }

  if (Serial.available() > 0)
  {
    uint8_t ch;

    ch = Serial.read();

    if (ch == 'e')
    {
      hamster.wheels(20, 20);
    }
    if (ch == 'd')
    {
      hamster.wheels(-20, -20);
    }    
    if (ch == 's')
    {
      hamster.wheels(0, 20);
    }    
    if (ch == 'f')
    {
      hamster.wheels(20, 0);
    }    
    if (ch == '0')
    {
      hamster.wheels(0, 0);
    }
  }

  updateLed();
}






void bleWrite(uint8_t *p_data, uint32_t length)
{
  p_tx_characteristic->writeValue(p_data, length);
}

void updateBleTx(void)
{
  static uint32_t pre_time;

  if (millis()-pre_time >= 20)
  {
    pre_time = millis();
    hamster.txPacket();
  }
}

void updateLed(void)
{
  static uint32_t pre_led_time;

  if (millis()-pre_led_time >= 500)
  {
    pre_led_time = millis();

    if (connected == true)
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }
}

void updateStatus(void)
{
  static uint32_t pre_time;

  if (millis()-pre_time >= 50)
  {
    pre_time = millis();
    hamster.printPacketSensor();
  }
}
