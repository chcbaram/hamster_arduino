//----------------------------------------------------------------------------
//    프로그램명   : Lib_Joy 관련 헤더
//
//    만든이		:
//
//    날  짜		:
//
//    최종 수정	:
//
//    MPU_Type	:
//
//    파일명		: Lib_Joy.h
//----------------------------------------------------------------------------


#ifndef __ARDUINO_UTIL_H__
#define __ARDUINO_UTIL_H__


#ifdef   ARDUINO_UTIL_LOCAL
#define  EXT_ARDUINO_UTIL_DEF
#else
#define  EXT_ARDUINO_UTIL_DEF     extern
#endif


#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/param.h>
#include <signal.h>
#include <stdint.h>
#include <Windows.h>

#include "ConsolSerial.h"
#include "EEPROMs.h"
#include "WMath.h"
#include "UARTClass.h"
#include "uart/uart.h"
#include "wiring_constants.h"

//extern UARTClass Serial;
extern ConsolSerial Serial;


extern uint32_t DDRC;
extern uint32_t PORTC;
extern uint32_t DDRD;
extern uint32_t PORTD;


EXT_ARDUINO_UTIL_DEF void delay(int delay_ms);
EXT_ARDUINO_UTIL_DEF uint32_t millis( void );
EXT_ARDUINO_UTIL_DEF uint32_t micros( void );

EXT_ARDUINO_UTIL_DEF uint8_t digitalRead( int num );
EXT_ARDUINO_UTIL_DEF void digitalWrite( int num, uint8_t mode );


#endif
