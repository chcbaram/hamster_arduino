//----------------------------------------------------------------------------
//    프로그램명   : arduino_util 관련 함수
//
//    만든이		:
//
//    날  짜		:
//
//    최종 수정	:
//
//    MPU_Type	:
//
//    파일명		: arduino_util.cpp
//----------------------------------------------------------------------------


#define ARDUINO_UTIL_LOCAL
#include "variant.h"



//UARTClass Serial((char *)"/dev/ttyUSB0");
//UARTClass Serial((char *)"/dev/ttyACM0");
ConsolSerial Serial;

uint32_t DDRC;
uint32_t PORTC;
uint32_t DDRD;
uint32_t PORTD;

//-- 내부 변수
//



//-- 내부 함수
//



/*---------------------------------------------------------------------------
	TITLE : delay
	WORK  :
	ARG   : void
	RET   : void
---------------------------------------------------------------------------*/
void delay(int delay_ms)
{
  uint32_t time_pre;

  time_pre = millis();
  while(1)
  {
    if (millis()-time_pre >= delay_ms)
    {
      break;
    }
  }
}


/*---------------------------------------------------------------------------
	TITLE : mills
	WORK  :
	ARG   : void
	RET   : void
---------------------------------------------------------------------------*/
uint32_t millis( void )
{
  LARGE_INTEGER freq, counter;
  double ret;

  QueryPerformanceCounter(&counter);
  QueryPerformanceFrequency(&freq);

  ret = (double)counter.QuadPart / (double)freq.QuadPart * 1000.0;

  return (uint32_t)ret;
}


/*---------------------------------------------------------------------------
	TITLE : micros
	WORK  :
	ARG   : void
	RET   : void
---------------------------------------------------------------------------*/
uint32_t micros( void )
{
  LARGE_INTEGER freq, counter;
  double ret;

  QueryPerformanceCounter(&counter);
  QueryPerformanceFrequency(&freq);

  ret = (double)counter.QuadPart / (double)freq.QuadPart * 1000000.0;

  return (uint32_t)ret;
}

void digitalWrite( int num, uint8_t mode )
{

}


uint8_t digitalRead( int num )
{
	return 0;
}
