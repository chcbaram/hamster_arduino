
#include "EEPROMs.h"
#include "variant.h"


EEPROM EEPROMs;

EEPROM::EEPROM(){
}


uint8_t EEPROM::read( int addr )
{
	return data_tbl[addr];
}


void EEPROM::write( int addr, uint8_t data )
{
	data_tbl[addr] = data;
}
