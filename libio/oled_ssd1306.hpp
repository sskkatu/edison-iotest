#pragma once

#include <cstdint>
#include "mraa.hpp"

namespace putmode {
    typedef enum {
        TEXT_OVERWRITE,
        TEXT_REVERSE,
        TEXT_OR,
        TEXT_XOR,
    } TextPutMode;
    typedef enum {
        SET,
        RESET,
        XOR,
    } PutMode;
}
using namespace mraa;
using namespace putmode;

/**
 * Products :
 *  - http://www.sainsmart.com/sainsmart-1-3-i2c-iic-serial-128x64-white-oled-for-arduino-uno-mega2560.html
 *  * Unfortunately, there's no sample-code or spec-sheet on above URL. It's very unkind!!!!
 *
 * Sample Code :
 *  - http://www.sainsmart.com/zen/documents/1.3OLED/1.30_OLED.zip
 *  * This sample-code is for another products, but it will work.
 *
 * SSD1306 DataSheets (OLED Driver/Controller IC) :
 *  - https://www.adafruit.com/datasheets/SSD1306.pdf
 *
 */
class Oled {
private:
    // Default I2C port/address
    static const int     DEFAULT_I2C_BUS_NO     =    6; 
    static const uint8_t DEFAULT_I2C_ADDRESS    = 0x3c;
    static const int     DEFAULT_GPIO_RESET_PIN =   24;

    // OLED Parameters
    static const int WIDTH             =     128; // Pixel width
    static const int HEIGHT            =      64; // Pixel height
    static const int BYTE_PER_LCDWIDTH = WIDTH/8; // Bytes per LCD width
    static const int RESET_TIME        =   10000; // Device reset duration time
    static const int STABLE_WAIT_TIME  =  100000; // Device stabling wait after reset
    static const uint8_t MULTIPLEX     =    0x3f;
    static const uint8_t COMPINS       =    0x12;
    static const uint8_t CONTRAST      =    0xCF;
    static const uint8_t CHARGEPUMP    =    0x14;
    static const uint8_t PRECHARGE     =    0x22;

    I2c* i2c;
    Gpio* gpioReset;
    uint8_t *frameBuf;

    void resetDevice(void);
    void initializeDevice(void);
    mraa_result_t sendCommand(uint8_t cmd);
    mraa_result_t sendCommand(uint8_t cmd, uint8_t d1);
    mraa_result_t sendCommand(uint8_t cmd, uint8_t d1, uint8_t d2);

public:
    Oled(int i2cBusNo = DEFAULT_I2C_BUS_NO, 
         uint8_t i2cAddress = DEFAULT_I2C_ADDRESS, 
         int gpioResetPinNo = DEFAULT_GPIO_RESET_PIN) {
        i2c = new I2c(i2cBusNo);
        i2c->address(i2cAddress);
        i2c->frequency(MRAA_I2C_FAST);
        gpioReset = new Gpio(gpioResetPinNo);
        gpioReset->dir(DIR_OUT);
        frameBuf = new uint8_t[WIDTH/8 * HEIGHT] {};
        resetDevice();
        initializeDevice();
    }

    ~Oled() {
        delete i2c;
        delete gpioReset;
        delete [] frameBuf;
    }    

    void update(void);
    void startDisplay(void);
    void stopDisplay(void);
    void invertDisplay(uint8_t i);

    void clearScreen(void);
    void putTextFormat(int x, int y, const char *fmt, ...);
    void putTextFormat(int x, int y, TextPutMode mode, const char *fmt, ...);
    void putTextString(int x, int y, const char *text);
    void putTextString(int x, int y, TextPutMode mode, const char *text);
    void putRect(int x1, int y1, int x2, int y2, PutMode mode = SET);
    void putLine(int x1, int y1, int x2, int y2, PutMode mode = SET);
    void putCircle(int x, int y, int r, PutMode mode = SET);
    void setPixel(int x, int y, int isSet=1);
    void putPixel(int x, int y, PutMode mode = SET);
    int  getPixel(int x, int y);
};
