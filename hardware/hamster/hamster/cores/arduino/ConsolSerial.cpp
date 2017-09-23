
#include "ConsolSerial.h"
#include "variant.h"


ConsolSerial::ConsolSerial(){
}

void ConsolSerial::begin(uint32_t baud_count){
}

void ConsolSerial::end(void){
}


int ConsolSerial::available(void){
  return 0;
}

int ConsolSerial::peek(void)
{
  return 0;
}

int ConsolSerial::read(void)
{
#if 0
  if ( vcp_is_available() == 0 )
    return -1;

  return vcp_getch();
#else
  return 0;
#endif
}

void ConsolSerial::flush(void){

}

size_t ConsolSerial::write(const uint8_t *buffer, size_t size)
{
	int i;

	for( i=0; i<size; i++ )
	{
		putc(buffer[i], stdout);
	}

  return size;
}


size_t ConsolSerial::write(uint8_t c) {
	return write(&c, 1);
}


// This operator is a convenient way for a sketch to check whether the
// port has actually been configured and opened by the host (as opposed
// to just being connected to the host).  It can be used, for example, in
// setup() before printing to ensure that an application on the host is
// actually ready to receive and display the data.
// We add a short delay before returning to fix a bug observed by Federico
// where the port is configured (lineState != 0) but not quite opened.
ConsolSerial::operator bool()
{
  return true;
}

