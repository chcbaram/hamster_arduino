#include <hamster.h>

Hamster hamster;


void setup() {
  // put your setup code here, to run once:
  hamster.begin("COM16");
}

void loop() {
  int left;
  int right;
  int length;
  int error;
  int error_handle;
  int motor_speed = 0;
  int l_speed = 0;
  int r_speed = 0;
  int speed_control_out;
  int handle_control_out;
  int error_margin = 3;

  
  left  = hamster.leftProximity();
  right = hamster.rightProximity(); 
  length = left+right / 2;

  error = 60 - length;
  speed_control_out = error / 1;
  if (speed_control_out < error_margin && speed_control_out > -error_margin)
  {
    speed_control_out = 0;
  }
  
  error_handle = (left - right);
  handle_control_out = error_handle / 1;
  if (handle_control_out < error_margin && handle_control_out > -error_margin)
  {
    handle_control_out = 0;
  }
  
  l_speed = speed_control_out - handle_control_out;
  r_speed = speed_control_out + handle_control_out;

  l_speed = constrain(l_speed, -100, 100);
  r_speed = constrain(r_speed, -100, 100);

  if (length == 0)
  {
    l_speed = 0;
    r_speed = 0;
  }
  Serial.print(length);
  Serial.print(" ");  
  Serial.print(error);
  Serial.print(" ");
  Serial.print(speed_control_out);
  Serial.print(" ");
  Serial.print(handle_control_out);
  Serial.print(" ");
  Serial.print(motor_speed);
  Serial.println("");

  hamster.wheels(l_speed, r_speed);
  delay(20);
}
