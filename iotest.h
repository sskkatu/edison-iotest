#ifndef __IOTEST_H__
#define __IOTEST_H__

#include <stdarg.h>

// --  External functions
extern int gpioTest(void);

extern int i2cTest(void);
extern int i2cOLEDTest(void);
extern void uartGpsTest(void);

extern int spiLCDTest(void);
extern int spiWS1812BTest(void);

extern void invertDisplay(uint8_t i);
extern void startscrollright(uint8_t start, uint8_t stop);
extern void startscrollleft(uint8_t start, uint8_t stop);
extern void startscrolldiagright(uint8_t start, uint8_t stop);
extern void startscrolldiagleft(uint8_t start, uint8_t stop);
extern void stopscroll(void);
extern void display(void);
extern void displayOff();
extern void displayOn();
extern void clearFrameBuffer(void);
extern void setOLEDPoint(int x, int y);
extern void resetOLEDPoint(int x, int y);
extern void setOLEDPixel(int x, int y, int isSet);
extern void printOLEDText(int x, int y, const char *fmt, ...);
extern const char* getIPv4AdrString(const char *ifname);



// -- Global resources
extern uint8_t glcdfont[];
const int GLCDFONT_DATASIZE;
const int GLCDFONT_MAX;

#endif
