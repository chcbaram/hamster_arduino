
#include "hamster.h"







Hamster::Hamster()
{
  is_port_open = false;
  is_connected = false;
  is_exit = false;

  rx_packet_state = 0;
  rx_packet_count_sensor = 0;

  uartWrite = NULL;
}

Hamster::~Hamster()
{
  is_exit = true;    
}

void Hamster::resetValue(void)
{
  effector.version                = 0;
  effector.topology               = 0;    
  effector.network_id             = 0; 
  effector.command                = 1;    
  effector.security               = 0;    
  effector.left_wheel             = 0; 
  effector.right_wheel            = 0; 
  effector.left_led_color         = 0;   
  effector.right_led_color        = 0; 
  effector.buzzer_pitch           = 0;         
  effector.buzzer_note            = 0;  
  effector.linetracer_mode        = 0;  
  effector.linetracer_speed       = 4;  
  effector.proximity_setup        = 2;  
  effector.acceleration_setup     = 0;  
  effector.acceleration_hz        = 3;
  effector.io_setup               = 0;  
  effector.output_a               = 0;  
  effector.output_b               = 0;  
  effector.wheel_compansate       = 0;
}

void* Hamster::threadUartRx(void *arg)
{
}

void* Hamster::threadUartTx(void *arg)
{
}

bool Hamster::begin(const char *port_name)
{
  bool ret = false;
  int err;

  resetValue();

  is_port_open = true;
  ret = true;

  return ret;
}

void Hamster::printPacketSensor(void)
{
  Serial.printf("Ver:%d ", sensor_info.version);
  Serial.printf("net:%d ", sensor_info.network_id);
  Serial.printf("cmd:%d ", sensor_info.command);
  Serial.printf("v:%d ", sensor_info.battery);
  Serial.printf("t:%d ", sensor_info.temp);
  Serial.printf("l_p:%03d ", sensor_info.left_proximity);
  Serial.printf("r_p:%03d ", sensor_info.right_proximity);
  Serial.printf("l_f:%03d ", sensor_info.left_floor);
  Serial.printf("r_f:%03d ", sensor_info.right_floor);
  Serial.printf("l:%X ", sensor_info.light);

  

  Serial.printf("%02X", sensor_info.mac_address[0]);
  Serial.printf("%02X", sensor_info.mac_address[1]);
  Serial.printf("%02X", sensor_info.mac_address[2]);
  Serial.printf("%02X", sensor_info.mac_address[3]);
  Serial.printf("%02X", sensor_info.mac_address[4]);
  Serial.printf("%02X", sensor_info.mac_address[5]);
  
  Serial.printf("\n");
}

uint8_t Hamster::asc_to_num(uint8_t data)
{
  uint8_t ret = 0;

  if (data >= '0' && data <= '9')
  {
    ret = data - '0';
  }
  else if (data >= 'a' && data <= 'z')
  {
    ret = data - 'a' + 10;
  }
  else if (data >= 'A' && data <= 'Z')
  {
    ret = data - 'A' + 10;
  }
  
  return ret;
}

uint8_t Hamster::num_to_asc(uint8_t data)
{
  uint8_t ret = ' ';


  if (data < 10)
  {
    ret = data + '0';
  }
  else
  {
    ret = (data - 10) + 'A';
  }
  
  return ret;
}

void Hamster::parsingPacketSensor(uint8_t *p_data)
{
  uint32_t i;
  uint32_t index;
  uint8_t *p_packet;
  uint8_t byte_data;
  p_packet = (uint8_t *)&rx_raw_packet;


  index = 0;
  for (i=0; i<20; i++)
  {
    p_packet[index++] = p_data[i];
  }

  
  sensor_info.version           = rx_raw_packet.version;             
  sensor_info.network_id        = rx_raw_packet.network_id;   
  sensor_info.command           = rx_raw_packet.command>>4 & 0x0F;      
  sensor_info.strength          = rx_raw_packet.strength;  
  sensor_info.left_proximity    = rx_raw_packet.left_proximity;
  sensor_info.right_proximity   = rx_raw_packet.right_proximity;
  sensor_info.left_floor        = rx_raw_packet.left_floor;
  sensor_info.right_floor       = rx_raw_packet.right_floor;
  sensor_info.acceleration_x    = swap16(rx_raw_packet.acceleration_x);
  sensor_info.acceleration_y    = swap16(rx_raw_packet.acceleration_y);
  sensor_info.acceleration_z    = swap16(rx_raw_packet.acceleration_z);
    
  if (rx_raw_packet.flag == 0)
  {
    sensor_info.light = rx_raw_packet.flag_data_a<<8 | rx_raw_packet.flag_data_b<<0;    
  }
  else
  {
    int temp;

    temp = (int8_t)rx_raw_packet.flag_data_a;
    sensor_info.temp    = 24 + temp / 2;
    sensor_info.battery = 200 + rx_raw_packet.flag_data_b * 100 / 100;      
  }

  sensor_info.input_a = rx_raw_packet.input_a;
  sensor_info.input_b = rx_raw_packet.input_b;


  index = 20;
  for (i=41; i<41+12; i+=2)
  {
    byte_data  = (asc_to_num(p_data[i+0])<<4) & 0xF0;
    byte_data |= (asc_to_num(p_data[i+1])<<0) & 0x0F;
    p_packet[index++] = byte_data;
  }  

  for (i=0; i<6; i++)
  {  
    sensor_info.mac_address[i] = rx_raw_packet.mac_address[i];  
  }
}

uint8_t Hamster::rxPacket(uint8_t data)
{
  return 0;
}

void Hamster::txPacket(void)
{
  uint8_t tx_buffer[128];

  tx_buffer[0]  = effector.version<<4;
  tx_buffer[0] |= effector.topology<<0;

  tx_buffer[1]  = effector.network_id;

  tx_buffer[2]  = effector.command<<4;
  tx_buffer[2] |= effector.security<<0;

  tx_buffer[3]  = effector.left_wheel;

  tx_buffer[4]  = effector.right_wheel;
  
  tx_buffer[5]  = effector.left_led_color;

  tx_buffer[6]  = effector.right_led_color;

  tx_buffer[7] = effector.buzzer_pitch>>0;  
  tx_buffer[8] = effector.buzzer_pitch>>8;
  tx_buffer[9] = effector.buzzer_pitch>>16;

  tx_buffer[10] = effector.buzzer_note;
  
  tx_buffer[11]  = effector.linetracer_mode<<4;
  tx_buffer[11] |= effector.linetracer_speed<<0;
  
  tx_buffer[12] = effector.proximity_setup;  
  
  tx_buffer[13]  = effector.acceleration_setup<<4;
  tx_buffer[13] |= effector.acceleration_hz<<0;
  
  tx_buffer[14] = effector.io_setup;
  
  tx_buffer[15] = effector.output_a;

  tx_buffer[16] = effector.output_b;

  tx_buffer[17] = effector.wheel_compansate;
  
  tx_buffer[18] = 0;
  tx_buffer[19] = 0;
  
  if (uartWrite != NULL)
  {
    uartWrite(tx_buffer, 20);
  }
}


void Hamster::wheels(double leftSpeed, double rightSpeed)
{
  effector.left_wheel  = leftSpeed;
  effector.right_wheel = rightSpeed;
}

void Hamster::leftWheels(double speed)
{
  effector.left_wheel  = speed;
}

void Hamster::rightWheels(double speed)
{
  effector.right_wheel  = speed;
}

void Hamster::stop(void)
{
  effector.left_wheel  = 0;
  effector.right_wheel = 0;
}

void Hamster::leds(int leftColor, int rightColor)
{
  effector.left_led_color  = leftColor;
  effector.right_led_color = rightColor; 
}

void Hamster::leftLed(int color)
{
  effector.left_led_color = color;
}

void Hamster::rightLed(int color)
{
  effector.right_led_color = color;
}

void Hamster::beep()
{
  effector.buzzer_pitch = 440*100;
  delay(200);
  effector.buzzer_pitch = 0;
}

void Hamster::buzzer(double hz)
{
  effector.buzzer_pitch = (uint32_t)(hz*100);
}

int Hamster::leftFloor(void)
{
  return sensor_info.left_floor;
}

int Hamster::rightFloor(void)
{
  return sensor_info.right_floor;
}

int Hamster::leftProximity(void)
{
  return sensor_info.left_proximity;
}

int Hamster::rightProximity(void)
{
  return sensor_info.right_proximity;
}

int Hamster::accelerationX(void)
{
  return sensor_info.acceleration_x;
}

int Hamster::accelerationY(void)
{
  return sensor_info.acceleration_y;
}

int Hamster::accelerationZ(void)
{
  return sensor_info.acceleration_z;
}

int Hamster::light(void)
{
  return sensor_info.light;
}

int Hamster::temperature(void)
{
  return sensor_info.temp;
}

int Hamster::battery(void)
{
  return sensor_info.battery;
}