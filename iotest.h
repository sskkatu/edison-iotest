#ifndef __IOTEST_H__
#define __IOTEST_H__

#include <stdarg.h>

// --  External functions
extern int gpioTest(void);

extern int i2cTest(void);
extern int i2cOLEDTest(void);
extern int i2cOLED_aitendo_Test(void);
extern void uartGpsTest(void);

extern int spiLCDTest(void);
extern int spiWS1812BTest(void);

extern const char* getIPv4AdrString(const char *ifname);



// -- Global resources
extern uint8_t glcdfont[];
const int GLCDFONT_DATASIZE;
const int GLCDFONT_MAX;

#endif
