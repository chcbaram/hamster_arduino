#ifndef _HAMSTER_H_
#define _HAMSTER_H_

#include <Arduino.h>



#define swap16(x)   (((x)>>8)&0x00FF)|(((x)<<8)&0xFF00)


#define RET_RX_EMPTY              0
#define RET_RX_PACKET_SENSOR      1
#define RET_RX_PACKET_ROBOT       2


#define RX_PACKET_SENSOR_LENGTH   53
#define RX_PACKET_EFFECTOR_LENGTH 53
#define RX_PACKET_ROBOT_LENGTH    31


#define LED_OFF	    0	//LED를 끈다.
#define LED_BLUE	  1	//LED를 파란색으로 켠다. (R: 0, G: 0, B: 255)
#define LED_GREEN	  2	//LED를 초록색으로 켠다. (R: 0, G: 255, B: 0)
#define LED_CYAN	  3	//LED를 하늘색으로 켠다. (R: 0, G: 255, B: 255)
#define LED_RED	    4	//LED를 빨간색으로 켠다. (R: 255, G: 0, B: 0)
#define LED_MAGENTA	5	//LED를 보라색으로 켠다. (R: 255, G: 0, B: 255)
#define LED_YELLOW	6	//LED를 노란색으로 켠다. (R: 255, G: 255, B: 0)
#define LED_WHITE	  7


typedef union
{
  struct
  {
    uint16_t light;
  } flag_0;

  struct
  {
    int8_t   temp;
    uint8_t  battery;
  } flag_1;
} flag_packet_t;

typedef struct
{
  uint8_t version;              // 0
  uint8_t network_id;           // 1
  uint8_t command;              // 2
  int8_t  strength;             // 3
  uint8_t left_proximity;       // 4
  uint8_t right_proximity;      // 5
  uint8_t left_floor;           // 6
  uint8_t right_floor;          // 7
  int16_t acceleration_x;       // 8
  int16_t acceleration_y;       // 10
  int16_t acceleration_z;       // 12
  uint8_t flag;                 // 14
  uint8_t flag_data_a;          // 15
  uint8_t flag_data_b;          // 15

  uint8_t input_a;              // 17
  uint8_t input_b;              // 19

  uint8_t mac_address[6];       // 20

} __attribute__((packed)) hamster_rx_raw_packet_t;

typedef struct
{
  uint8_t version;             
  uint8_t network_id;          
  uint8_t command;             
  int8_t  strength;            
  uint8_t left_proximity;      
  uint8_t right_proximity;     
  uint8_t left_floor;          
  uint8_t right_floor;         
  int16_t acceleration_x;      
  int16_t acceleration_y;      
  int16_t acceleration_z;      
    
  uint16_t light;
  int8_t   temp;
  uint16_t battery;

  uint8_t input_a;
  uint8_t input_b;

  uint8_t mac_address[6];
} hamster_sensor_info_t;

typedef struct
{
  uint8_t version;                 

  uint8_t mac_address[6];
} hamster_robot_info_t;

typedef struct
{
  uint8_t version;    
  uint8_t topology;
  uint8_t network_id; 
  uint8_t command;    
  uint8_t security;    
  int8_t  left_wheel; 
  int8_t  right_wheel; 
  uint8_t left_led_color; 
  uint8_t right_led_color; 
  uint32_t buzzer_pitch;         
  uint8_t  buzzer_note;  
  uint8_t linetracer_mode;  
  uint8_t linetracer_speed;  
  uint8_t proximity_setup;  
  uint8_t acceleration_setup;  
  uint8_t acceleration_hz;  
  uint8_t io_setup;  
  uint8_t output_a;  
  uint8_t output_b;  
  int8_t  wheel_compansate;
  
  uint8_t mac_address[6];  
} hamster_effector_t;

class Hamster {

  public:
    Hamster();
    ~Hamster();    
    bool begin(const char *port_name);
    uint8_t asc_to_num(uint8_t data);
    uint8_t num_to_asc(uint8_t data);

    bool is_port_open;
    bool is_connected;
    bool is_exit;
    
    void resetValue(void);
    void txPacket(void);
    void parsingPacketSensor(uint8_t *p_data);
    void printPacketSensor(void);

    void (*uartWrite)(uint8_t *p_data, uint32_t length);

    // API
    //
    void wheels(double leftSpeed, double rightSpeed);
    void leftWheels(double speed);
    void rightWheels(double speed);
    void stop(void);
    void leds(int leftColor, int rightColor);
    void leftLed(int color);
    void rightLed(int color);
    void beep();
    void buzzer(double hz);
    int leftFloor(void);
    int rightFloor(void);
    int leftProximity(void);
    int rightProximity(void);
    int accelerationX(void);
    int accelerationY(void);
    int accelerationZ(void);
    int light(void);
    int temperature(void);
    int battery(void);

  private:


    bool     tx_done;
    

    uint8_t  rx_packet_state;
    uint8_t  rx_packet_index;    
    uint32_t rx_packet_count_sensor;    
    uint32_t rx_packet_pre_time;
    uint8_t  rx_packet_buffer[128];
    hamster_rx_raw_packet_t rx_raw_packet;


    hamster_sensor_info_t  sensor_info;
    hamster_robot_info_t   robot_info;
    hamster_effector_t     effector;


    static void* threadUartRx(void *arg);
    static void* threadUartTx(void *arg);

    uint8_t rxPacket(uint8_t data);        
    
};

#endif