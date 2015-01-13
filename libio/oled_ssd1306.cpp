#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include "oled_ssd1306.hpp"
#include "glcdfont.hpp"
#include "oled_commands.hpp"

using namespace SSD1306_CMDS;

void OLED_ssd1306::resetDevice(void) {
    gpioReset->write(0);  
    usleep(RESET_TIME);        // Wait for reset duration
    gpioReset->write(1);     
    usleep(STABLE_WAIT_TIME);  // Wait for the device stabling
}

void OLED_ssd1306::initializeDevice() {
    displayOff();
    stopscroll();
    sendCommand(SSD1306_SETDISPLAYCLOCKDIV, 0x80);
    sendCommand(SSD1306_SETMULTIPLEX, MULTIPLEX);
    sendCommand(SSD1306_SETDISPLAYOFFSET, 0x00);
    sendCommand(SSD1306_SETSTARTLINE | 0);
    sendCommand(SSD1306_CHARGEPUMP, CHARGEPUMP);
    sendCommand(SSD1306_MEMORYMODE, 0x00);
    sendCommand(SSD1306_SEGREMAP | 0x1);
    sendCommand(SSD1306_COMSCANDEC);
    sendCommand(SSD1306_SETCOMPINS, COMPINS);
    sendCommand(SSD_SET_CONTRASTLEVEL, CONTRAST);
    sendCommand(SSD1306_SETPRECHARGE, PRECHARGE);
    sendCommand(SSD1306_SETVCOMDETECT, 0x40);
    sendCommand(SSD1306_DISPLAYALLON_RESUME);
    sendCommand(SSD1306_NORMAL_DISPLAY);
    sendCommand(0x21, 0, 127);
    sendCommand(0x22, 0, 127);
    flushDisplay();
    usleep(5000);
    displayOn();
}

void OLED_ssd1306::flushDisplay(void) {
    int i, j;
    uint8_t buf[BYTE_PER_LCDWIDTH+1] = {};
    uint8_t dummyBuf[3+1] = {};
    uint8_t *pbuf = frameBuf;

    dummyBuf[0] = 0x40;
    sendCommand(SSD1306_SETLOWCOLUMN  | 0x0); // low col = 0
    sendCommand(SSD1306_SETHIGHCOLUMN | 0x0); // hi col = 0
    sendCommand(SSD1306_SETSTARTLINE  | 0x0); // line #0

    for (i=0; i < HEIGHT/8; i++) {
        sendCommand(0xB0+i, 0, 0x10);
        // W/A for left 3-vertial column could not displayed
        i2c->write(dummyBuf, sizeof(dummyBuf));
        for (j = 0; j < WIDTH/BYTE_PER_LCDWIDTH; j++) {
            buf[0] = 0x40;
            memcpy(&buf[1], pbuf, BYTE_PER_LCDWIDTH);
            i2c->write(buf, sizeof(buf));
            pbuf += BYTE_PER_LCDWIDTH;
        }
    }
}

void OLED_ssd1306::clearScreen(void) {
    memset(frameBuf, 0, WIDTH/8 * HEIGHT);
}

void OLED_ssd1306::putRect(int x1, int y1, int x2, int y2, PutMode mode) {
    int minX, minY,  maxX, maxY, x, y;

    minX = std::min(x1, x2);
    minY = std::min(y1, y2);
    maxX = std::max(x1, x2);
    maxY = std::max(y1, y2);
    
    for (x = minX; x <= maxX; x++) {
        for (y = minY; y <= maxY; y++) {
            setPixelPutMode(x, y, mode);
        }
    }
}

void OLED_ssd1306::putLine(int x0, int y0, int x1, int y1, PutMode mode) {
    int steep, x, y, dx, dy, err, ystep;

    steep = abs(y1-y0) > abs(x1-x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    dx = x1 - x0;
    dy = abs(y1-y0);
    err = dx / 2;
    y = y0;
    ystep = (y0 < y1) ? 1 : -1;
    for (x = x0; x <= x1; x++) {
        if (steep) {
            setPixelPutMode(y, x, mode);
        } else {
            setPixelPutMode(x, y, mode);
        }
        err = err - dy;
        if (err < 0) {
            y = y + ystep;
            err = err + dx;
        }
    }
}

void OLED_ssd1306::setPixelPutMode(int x, int y, PutMode mode) {
    int c;

    switch (mode) {
    case SET:
        setPixel(x,y);
        break;
    case RESET:
        resetPixel(x,y);
        break;
    case XOR:
        c = getPixel(x, y);
        setPixel(x, y, c ^ 1);
        break;
    }
}

void OLED_ssd1306::putTextFormat(int x, int y, const char *fmt, ...) {
    char textbuf[80];
    va_list arg;

    va_start(arg, fmt);
    vsnprintf(textbuf, sizeof(textbuf), fmt, arg);
    va_end(arg);

    putTextString(x, y, TEXT_XOR, textbuf);
}

void OLED_ssd1306::putTextFormat(int x, int y, TextPutMode mode, const char *fmt, ...) {
    char textbuf[80];
    va_list arg;

    va_start(arg, fmt);
    vsnprintf(textbuf, sizeof(textbuf), fmt, arg);
    va_end(arg);

    putTextString(x, y, mode, textbuf);
}

void OLED_ssd1306::putTextString(int x, int y, const char *text)  {
    putTextString(x, y, TEXT_XOR, text);
}

void OLED_ssd1306::putTextString(int x, int y, TextPutMode mode, const char *text) {
    unsigned char c, b, bi;
    int ix, iy;
    uint8_t bitmap;
    const uint8_t *fontdata;
    while ((c = *text++) != '\0') {
        fontdata = GLcdFont::get5x7FontBitmap(c);
        for (ix = 0; ix < 6; ix++) {
            if (ix < 5) {
                bitmap = fontdata[ix];
            } else {
                bitmap = 0x00;
            }
            for (iy = 0; iy < 8; iy++) {
                b = (bitmap & 0x01) ? 1 : 0;
                switch (mode) {
                case TEXT_OVERWRITE:
                    setPixel(x + ix, y + iy, b);
                    break;
                case TEXT_REVERSE:
                    setPixel(x + ix, y + iy, !b);
                    break;
                case TEXT_OR:
                    bi = getPixel(x + ix, y + iy);
                    setPixel(x + ix, y + iy, b | bi);
                    break;
                case TEXT_XOR:
                    bi = getPixel(x + ix, y + iy);
                    setPixel(x + ix, y + iy, b ^ bi);
                    break;
                }
                bitmap >>= 1;
            }
        }
        x += 6;
    }
}

void OLED_ssd1306::setPixel(int x, int y) {
    setPixel(x, y, 1);
}

void OLED_ssd1306::resetPixel(int x, int y) {
    setPixel(x, y, 0);
}

void OLED_ssd1306::setPixel(int x, int y, int isSet) {
    uint8_t *base = frameBuf;
    uint8_t bitmap;
    if (x < 0 || WIDTH <= x || y < 0 || HEIGHT <= y) {
        return;
    }

    base += (y/8)*WIDTH + x;
    bitmap = 0x01 << (y % 8);

    if (isSet) {
        *base |= bitmap;
    } else {
        *base &= ~bitmap;
    }
}

int OLED_ssd1306::getPixel(int x, int y) {
    uint8_t *base = frameBuf;
    uint8_t bitmap;
    if (x < 0 || WIDTH <= x || y < 0 || HEIGHT <= y) {
        return 0; 
    }

    base += (y/8)*WIDTH + x;
    bitmap = 0x01 << (y % 8);

    if (*base & bitmap) {
        return 1;
    }
    return 0;
}


void OLED_ssd1306::startScrollRight(uint8_t start, uint8_t stop) {
    sendCommand(SSD1306_RIGHT_HORIZONTAL_SCROLL);
    sendCommand(0X00);
    sendCommand(start);
    sendCommand(0X00);
    sendCommand(stop);
    sendCommand(0X01);
    sendCommand(0XFF);
    sendCommand(SSD_ACTIVATE_SCROLL);
}

void OLED_ssd1306::startScrollLeft(uint8_t start, uint8_t stop) {
    sendCommand(SSD1306_LEFT_HORIZONTAL_SCROLL);
    sendCommand(0X00);
    sendCommand(start);
    sendCommand(0X00);
    sendCommand(stop);
    sendCommand(0X01);
    sendCommand(0XFF);
    sendCommand(SSD_ACTIVATE_SCROLL);
}

void OLED_ssd1306::startScrollDiagRight(uint8_t start, uint8_t stop) {
    sendCommand(SSD1306_SET_VERTICAL_SCROLL_AREA);
    sendCommand(0X00);
    sendCommand(HEIGHT);
    sendCommand(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
    sendCommand(0X00);
    sendCommand(start);
    sendCommand(0X00);
    sendCommand(stop);
    sendCommand(0X01);
    sendCommand(SSD_ACTIVATE_SCROLL);
}

void OLED_ssd1306::startScrollDiagLeft(uint8_t start, uint8_t stop) {
    sendCommand(SSD1306_SET_VERTICAL_SCROLL_AREA);
    sendCommand(0X00);
    sendCommand(HEIGHT);
    sendCommand(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
    sendCommand(0X00);
    sendCommand(start);
    sendCommand(0X00);
    sendCommand(stop);
    sendCommand(0X01);
    sendCommand(SSD_ACTIVATE_SCROLL);
}

void OLED_ssd1306::stopscroll(void) {
    sendCommand(SSD_DEACTIVATE_SCROLL);
}

void OLED_ssd1306::displayOff(void) {
    sendCommand(SSD_DISPLAY_OFF);
}

void OLED_ssd1306::displayOn(void) {
    sendCommand(SSD_DISPLAY_ON);
}

void OLED_ssd1306::invertDisplay(uint8_t i) {
    if (i) {
        sendCommand(SSD_INVERSE_DISPLAY);
    } else {
        sendCommand(SSD1306_NORMAL_DISPLAY);
    }
}

mraa_result_t OLED_ssd1306::sendCommand(uint8_t cmd) {
    uint8_t buf[2];
    buf[0] = SSD_COMMAND_MODE;
    buf[1] = cmd;
    return i2c->write(buf, sizeof(buf));
}

mraa_result_t OLED_ssd1306::sendCommand(uint8_t cmd, uint8_t d1) {
    uint8_t buf[3];
    buf[0] = SSD_COMMAND_MODE;
    buf[1] = cmd;
    buf[2] = d1;
    return  i2c->write(buf, sizeof(buf));
}

mraa_result_t OLED_ssd1306::sendCommand(uint8_t cmd, uint8_t d1, uint8_t d2) {
    uint8_t buf[4];
    buf[0] = SSD_COMMAND_MODE;
    buf[1] = cmd;
    buf[2] = d1;
    buf[3] = d2;
    return i2c->write(buf, sizeof(buf));
}
                                                                    
