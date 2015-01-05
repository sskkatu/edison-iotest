#ifndef __IOTEST_H__
#define __IOTEST_H__

// --  External functions
extern int gpioTest(void);
extern int i2cTest(void);
extern int i2cOLEDTest(void);
extern int spiTest(void);
extern int spiWS1812BTest(void);

extern void invertDisplay(uint8_t i);
extern void startscrollright(uint8_t start, uint8_t stop);
extern void startscrollleft(uint8_t start, uint8_t stop);
extern void startscrolldiagright(uint8_t start, uint8_t stop);
extern void startscrolldiagleft(uint8_t start, uint8_t stop);
extern void stopscroll(void);
extern void display(void);
extern void clearDisplay(void);


// -- Global resources
extern uint8_t glcdfont[];

#endif
