#ifndef _EEPROMs_H_INCLUDED
#define _EEPROMs_H_INCLUDED


#include "RingBuffer.h"
#include "Stream.h"


class EEPROM{

  public:
	EEPROM();

	uint8_t data_tbl[1024];

	uint8_t read( int addr );
	void write( int addr, uint8_t data );
};

extern EEPROM EEPROMs;

#endif
