#ifndef _CONSOL_SERIAL_H_INCLUDED
#define _CONSOL_SERIAL_H_INCLUDED


#include "RingBuffer.h"
#include "Stream.h"


class ConsolSerial : public Stream {

  public:
	ConsolSerial();
    void begin(uint32_t baud_count);
    void end(void);

    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t c);
    virtual size_t write(const uint8_t *buffer, size_t size);
    using Print::write; // pull in write(str) from Print
    operator bool();
};

#endif
