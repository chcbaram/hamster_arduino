
#include "Arduino.h"
#include "hamster.h"







Hamster::Hamster()
{
  uart_ch = _DEF_UART1;
  uart_baud = 115200;
  uart_port_name[0] = 0;
  is_port_open = false;
  is_connected = false;
  is_exit = false;

  rx_packet_state = 0;
  rx_packet_count_sensor = 0;
}

Hamster::~Hamster()
{
  is_exit = true;    
  pthread_join(thread_uart_rx, NULL);
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
  Hamster *p_hamster = (Hamster *)arg;
  uint32_t rx_length;
  uint8_t  rx_buffer[1024];
  uint32_t pre_time;
  uint8_t ret;


  pre_time = millis();
  while(p_hamster->is_exit == false)
  {
    if (p_hamster->is_port_open == false)
    {
      Sleep(1);
      continue;
    }

    if (millis()-pre_time >= 1000)
    {
      pre_time = millis();
      uartPrintf(p_hamster->uart_ch, "FF\r");
    }

    rx_length = uartReadBytes(p_hamster->uart_ch, rx_buffer, 1024);

    if (rx_length > 0)
    {
      for (int i=0; i<rx_length; i++)
      {
        #if 0
        if (rx_buffer[i] == 0x0D)
        {
          printf("\n");
        }
        else
        {
          printf("%c", rx_buffer[i]);
        }
        #endif
        ret = p_hamster->rxPacket(rx_buffer[i]);

        if (ret == RET_RX_PACKET_SENSOR)
        {
          p_hamster->rx_packet_count_sensor++;
          p_hamster->is_connected = true;

          for (int i=0; i<6; i++)
          {
            p_hamster->effector.mac_address[i] = p_hamster->sensor_info.mac_address[i];
          }
          //p_hamster->printPacketSensor();
        }
      }
    }
    Sleep(1);
  }

  if (p_hamster->is_port_open)
  {
    uartClose(p_hamster->uart_ch);
  }
  pthread_exit(NULL);
}

void* Hamster::threadUartTx(void *arg)
{
  Hamster *p_hamster = (Hamster *)arg;
  uint32_t pre_time;


  pre_time = millis();
  while(p_hamster->is_exit == false)
  {
    if (p_hamster->is_port_open == true && p_hamster->is_connected == true)
    {
      p_hamster->txPacket();
    }
    Sleep(20);
  }

  pthread_exit(NULL);
}

void Hamster::begin(const char *port_name)
{
  bool ret = false;
  int err;

  resetValue();

  if (uartOpen(uart_ch, (char *)port_name, uart_baud) == 0)
  {
    strcpy(uart_port_name, port_name);
    printf("[OK] : Hamster::begin\n");
    is_port_open = true;
    ret = true;
  }
  else
  {
    printf("[NG] : Hamster::begin\n");
  }

  if (ret == true)
  {
    err = pthread_create(&thread_uart_tx, NULL, threadUartTx, this);
    err = pthread_create(&thread_uart_rx, NULL, threadUartRx, this);
    if (err != 0)
    {
      printf("[__] : can't create thread :[%s]\n", strerror(err));
      ret = false;
    }
    else
    {
      printf("[__] : Thread created successfully\n");    
      ret = true;
    }
  }
}

void Hamster::printPacketSensor(void)
{
  printf("Ver:%d ", sensor_info.version);
  printf("net:%d ", sensor_info.network_id);
  printf("cmd:%d ", sensor_info.command);
  printf("v:%d ", sensor_info.battery);
  printf("t:%d ", sensor_info.temp);
  printf("l_p:%03d ", sensor_info.left_proximity);
  printf("r_p:%03d ", sensor_info.right_proximity);
  printf("l_f:%03d ", sensor_info.left_floor);
  printf("r_f:%03d ", sensor_info.right_floor);
  printf("l:%X ", sensor_info.light);

  

  printf("%02X", sensor_info.mac_address[0]);
  printf("%02X", sensor_info.mac_address[1]);
  printf("%02X", sensor_info.mac_address[2]);
  printf("%02X", sensor_info.mac_address[3]);
  printf("%02X", sensor_info.mac_address[4]);
  printf("%02X", sensor_info.mac_address[5]);
  
  printf("\n");
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
  for (i=0; i<40; i+=2)
  {
    byte_data  = (asc_to_num(p_data[i+0])<<4) & 0xF0;
    byte_data |= (asc_to_num(p_data[i+1])<<0) & 0x0F;
    p_packet[index++] = byte_data;
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
  uint8_t ret = RET_RX_EMPTY;


  if (millis()-rx_packet_pre_time >= 500)
  {
    rx_packet_state = 0;
    rx_packet_index = 0;
  }
  rx_packet_pre_time = millis();


  switch(rx_packet_state)
  {
    case 0:
      rx_packet_index = 0;
      rx_packet_buffer[rx_packet_index++] = data;
      rx_packet_state = 1;
      break;

    case 1:
      rx_packet_buffer[rx_packet_index++] = data;


      if (rx_packet_buffer[0] == 'F' && rx_packet_buffer[1] == 'F')
      {
        rx_packet_state = 100;
      }
      else
      {
        rx_packet_state = 2;
      }
      break;

    case 2:
      if (data == 0x0D)
      {
        parsingPacketSensor(rx_packet_buffer);
        rx_packet_state = 0;
        ret = RET_RX_PACKET_SENSOR;
      }
      else
      {
        if (rx_packet_index <RX_PACKET_SENSOR_LENGTH)
        {
          rx_packet_buffer[rx_packet_index++] = data;
        }
        else
        {
          rx_packet_state = 0;
        }
      }      
      break;

    case 100:
      if (data == 0x0D)
      {
        rx_packet_state = 0;
      }
      else
      {
        if (rx_packet_index <RX_PACKET_SENSOR_LENGTH)
        {
          rx_packet_buffer[rx_packet_index++] = data;
        }
        else
        {
          rx_packet_state = 0;
        }
      }      
      break;
  }


  return ret;
}

void Hamster::txPacket(void)
{
  uint8_t tx_buffer[128];
  uint32_t index;


  index = 0;

  tx_buffer[0] = num_to_asc((effector.version)>>0 & 0x0F);
  tx_buffer[1] = num_to_asc((effector.topology)>>0 & 0x0F);

  tx_buffer[2] = num_to_asc((effector.network_id)>>4 & 0x0F);
  tx_buffer[3] = num_to_asc((effector.network_id)>>0 & 0x0F);

  tx_buffer[4] = num_to_asc((effector.command)>>0 & 0x0F);
  tx_buffer[5] = num_to_asc((effector.security)>>0 & 0x0F);

  tx_buffer[6] = num_to_asc((effector.left_wheel)>>4 & 0x0F);
  tx_buffer[7] = num_to_asc((effector.left_wheel)>>0 & 0x0F);

  tx_buffer[8] = num_to_asc((effector.right_wheel)>>4 & 0x0F);
  tx_buffer[9] = num_to_asc((effector.right_wheel)>>0 & 0x0F);

  tx_buffer[10] = num_to_asc((effector.left_led_color)>>4 & 0x0F);
  tx_buffer[11] = num_to_asc((effector.left_led_color)>>0 & 0x0F);

  tx_buffer[12] = num_to_asc((effector.right_led_color)>>4 & 0x0F);
  tx_buffer[13] = num_to_asc((effector.right_led_color)>>0 & 0x0F);

  tx_buffer[14] = num_to_asc((effector.buzzer_pitch)>>20 & 0x0F);
  tx_buffer[15] = num_to_asc((effector.buzzer_pitch)>>16 & 0x0F);
  
  tx_buffer[16] = num_to_asc((effector.buzzer_pitch)>>12 & 0x0F);
  tx_buffer[17] = num_to_asc((effector.buzzer_pitch)>>8 & 0x0F);

  tx_buffer[18] = num_to_asc((effector.buzzer_pitch)>>4 & 0x0F);
  tx_buffer[19] = num_to_asc((effector.buzzer_pitch)>>0 & 0x0F);

  tx_buffer[20] = num_to_asc((effector.buzzer_note)>>4 & 0x0F);
  tx_buffer[21] = num_to_asc((effector.buzzer_note)>>0 & 0x0F);
  
  tx_buffer[22] = num_to_asc((effector.linetracer_mode)>>0 & 0x0F);
  tx_buffer[23] = num_to_asc((effector.linetracer_speed)>>0 & 0x0F);
  
  tx_buffer[24] = num_to_asc((effector.proximity_setup)>>4 & 0x0F);
  tx_buffer[25] = num_to_asc((effector.proximity_setup)>>0 & 0x0F);
  
  tx_buffer[26] = num_to_asc((effector.acceleration_setup)>>0 & 0x0F);
  tx_buffer[27] = num_to_asc((effector.acceleration_hz)>>0 & 0x0F);
  
  tx_buffer[28] = num_to_asc((effector.io_setup)>>4 & 0x0F);
  tx_buffer[29] = num_to_asc((effector.io_setup)>>0 & 0x0F);
  
  tx_buffer[30] = num_to_asc((effector.output_a)>>4 & 0x0F);
  tx_buffer[31] = num_to_asc((effector.output_a)>>0 & 0x0F);

  tx_buffer[32] = num_to_asc((effector.output_b)>>4 & 0x0F);
  tx_buffer[33] = num_to_asc((effector.output_b)>>0 & 0x0F);

  tx_buffer[34] = num_to_asc((effector.wheel_compansate)>>4 & 0x0F);
  tx_buffer[35] = num_to_asc((effector.wheel_compansate)>>0 & 0x0F);
  
  tx_buffer[36] = num_to_asc((0)>>4 & 0x0F);
  tx_buffer[37] = num_to_asc((0)>>0 & 0x0F);

  tx_buffer[38] = num_to_asc((0)>>4 & 0x0F);
  tx_buffer[39] = num_to_asc((0)>>0 & 0x0F);
  
  tx_buffer[40] = '-';

  tx_buffer[41] = num_to_asc((effector.mac_address[0])>>4 & 0x0F);
  tx_buffer[42] = num_to_asc((effector.mac_address[0])>>0 & 0x0F);
  tx_buffer[43] = num_to_asc((effector.mac_address[1])>>4 & 0x0F);
  tx_buffer[44] = num_to_asc((effector.mac_address[1])>>0 & 0x0F);
  tx_buffer[45] = num_to_asc((effector.mac_address[2])>>4 & 0x0F);
  tx_buffer[46] = num_to_asc((effector.mac_address[2])>>0 & 0x0F);
  tx_buffer[47] = num_to_asc((effector.mac_address[3])>>4 & 0x0F);
  tx_buffer[48] = num_to_asc((effector.mac_address[3])>>0 & 0x0F);
  tx_buffer[49] = num_to_asc((effector.mac_address[4])>>4 & 0x0F);
  tx_buffer[50] = num_to_asc((effector.mac_address[4])>>0 & 0x0F);
  tx_buffer[51] = num_to_asc((effector.mac_address[5])>>4 & 0x0F);
  tx_buffer[52] = num_to_asc((effector.mac_address[5])>>0 & 0x0F);
  tx_buffer[53] = 0x0D;


  uartWrite(uart_ch, tx_buffer, RX_PACKET_EFFECTOR_LENGTH + 1);
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