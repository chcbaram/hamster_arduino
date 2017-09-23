/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UARTClass.h"

/*
#include "./uart/Define.h"
#include "./uart/Uart_Lib.h"
#include "./uart/Uart_Sig_Lib.h"

#include "./uart/Uart_Lib.c"
#include "./uart/Uart_Sig_Lib.c"
*/

extern UARTClass Serial;


void UartRxd( char Data )
{
	Serial.RxHandler(Data);
}

// Constructors ////////////////////////////////////////////////////////////////
UARTClass::UARTClass(void){
	rx_buffer.iHead = 0;
	rx_buffer.iTail = 0;
	IsOpened = false;
	strcpy(PortStr, "/dev/ttyUSB0");
}

UARTClass::UARTClass( char *PortName )
{
	rx_buffer.iHead = 0;
	rx_buffer.iTail = 0;
	strcpy(PortStr, PortName);
}


void UARTClass::begin(const uint32_t dwBaudRate)
{
  begin(dwBaudRate, Mode_8N1);
}

void UARTClass::begin(const uint32_t dwBaudRate, const UARTModes config)
{
  uint32_t modeReg = 0;//static_cast<uint32_t>(config) & 0x00000E00;
  init(dwBaudRate, modeReg);
}

void UARTClass::init(const uint32_t dwBaudRate, const uint32_t modeReg)
{
	int baud;

	// Make sure both ring buffers are initialized back to empty.
	rx_buffer.iHead = rx_buffer.iTail = 0;
	tx_buffer.iHead = tx_buffer.iTail = 0;
/*
	switch( dwBaudRate )
	{
	case 115200:
		baud = BAUD_115200;
		break;
	case 57600:
		baud = BAUD_57600;
		break;
	case 38400:
		baud = BAUD_38400;
		break;
	case 19200:
		baud = BAUD_19200;
		break;
	case 2400:
		baud = BAUD_2400;
		break;
	case 1200:
		baud = BAUD_1200;
		break;

	default:
		baud = BAUD_115200;
		break;
	}

	//-- 시리얼 통신 초기화
	UartHandle = Uart_Open( (char *)PortStr, baud );

	if( UartHandle < 0 )
	{
		printf("Uart Open Failed \n");
		return;
	}
	else
	{
		printf("Uart Open : %s \n", PortStr);
	}

	Uart_Sig_Init( UartHandle );
	Uart_Sig_Func_Init( UartHandle, UartRxd );
*/

	IsOpened = true;
}

void UARTClass::end( void )
{
  // Clear any received data
  rx_buffer.iHead = rx_buffer.iTail;

  // Wait for any outstanding data to be sent
  flush();
}

int UARTClass::available( void )
{
  return (uint32_t)(SERIAL_BUFFER_SIZE + rx_buffer.iHead - rx_buffer.iTail) % SERIAL_BUFFER_SIZE;
}

int UARTClass::peek( void )
{
  if ( rx_buffer.iHead == rx_buffer.iTail )
    return -1;

  return rx_buffer.buffer[rx_buffer.iTail];
}

int UARTClass::read( void )
{
  // if the head isn't ahead of the tail, we don't have any characters
  if ( rx_buffer.iHead == rx_buffer.iTail )
    return -1;

  uint8_t uc = rx_buffer.buffer[rx_buffer.iTail];
  rx_buffer.iTail = (unsigned int)(rx_buffer.iTail + 1) % SERIAL_BUFFER_SIZE;
  return uc;
}

void UARTClass::flush( void )
{
  while (tx_buffer.iHead != tx_buffer.iTail); //wait for transmit data to be sent
  // Wait for transmission to complete
}

size_t UARTClass::write(const uint8_t *buffer, size_t size)
{
	//Uart_Write( (char *)buffer, size );
	return size;
}

size_t UARTClass::write( const uint8_t uc_data )
{
	//HAL_UART_Transmit(_pUart, (uint8_t *)&uc_data, 1, 10);
	//Uart_Write( (char *)&uc_data, 1 );
	return 1;
}

void UARTClass::RxHandler( char Data ){

    if(available() < (SERIAL_BUFFER_SIZE - 1)){ //If there is empty space in rx_buffer, read a byte from the Serial port and save it to the buffer.
    rx_buffer.buffer[rx_buffer.iHead] = Data;
		rx_buffer.iHead = (uint16_t)(rx_buffer.iHead + 1) % SERIAL_BUFFER_SIZE;
  }
}
