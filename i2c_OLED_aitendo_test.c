#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "mraa.h"
#include "iotest.h"
#include "oled_commands.h"


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

/* I2C/GPIO PORT Settings */
#define I2CPORT          1 // Target I2C port
#define I2CADR        0x3c // OLED slave address
#define GPIO_I2C1SDA     7 
#define GPIO_I2C1SCL    19 
#define GPIO_RESET      10 // Use the SPI-2-CLK as a GPIO-RESET signal

/* LCD Parammeters */
#define LCDWIDTH       128
#define LCDHEIGHT       64
#define MULTIPLEX     0x3f
#define COMPINS       0x12
#define CONTRAST      0xCF
#define CHARGEPUMP    0x14
#define PRECHARGE     0xF1
#define RESET_TIME   10000 // OLED device reset duration 
                           // (In actually, it needed only 3us according to the specsheet.)
#define STABLE_WAIT 100000 // Wait the OLED device stabling
#define BYTE_PER_LCDWIDTH (LCDWIDTH/8) // Byte length per LCD width


/* Private functions */
static void enablePullup(int gpioport);
static void resetAndWaitStable(void);
static void init(void);
static void sendCommand(uint8_t cmd);
static void sendCommandByte(uint8_t cmd, uint8_t data);
static void sendCommandByte2(uint8_t cmd, uint8_t data1, uint8_t data2);

/* Global variables */
static mraa_i2c_context i2c = NULL;

static uint8_t frameBuf[BYTE_PER_LCDWIDTH*LCDHEIGHT];

int i2cOLED_aitendo_Test(void)
{
    int i, j, k, x, y;
    int sec_prev = -1, flg = 0;
    enablePullup(GPIO_I2C1SDA);        // i2c1 SDA
    enablePullup(GPIO_I2C1SCL);        // i2c1 SCL

    i2c = mraa_i2c_init(I2CPORT);
    if (i2c == NULL) {
        fprintf(stderr, "Cannot open i2c port:%d\n", I2CPORT);
        exit(1);
    }

    mraa_i2c_frequency(i2c, MRAA_I2C_FAST);
    mraa_i2c_address(i2c, I2CADR);
    init();
#if (0)
    for (i=0;; i++) {
        //        for (j=0; j < sizeof(frameBuf) ; j++) {
        //            frameBuf[j] = 0x5555;
        //        }
        clearFrameBuffer();
        for (j=0; j < 256; j++) {
            if ((j+1)*6 < sizeof(frameBuf)) {
                for (k=0; k < 5; k++) {
                    frameBuf[j*6 + k] = glcdfont[(j+i)%0x100*5+k];
                }
                frameBuf[j*6+5] = 0x00;
            }
        }
        display();
    }
#else
    while (1) {
        time_t timer;
        struct tm *local;
        struct tm *utc;
        const char *ROT = "|/-\\*";
        clearFrameBuffer();
        printOLEDText(1,  1, "Hello, Edison.");
        printOLEDText(1, 16, "IP : %s", getIPv4AdrString("wlan0"));
        timer = time(NULL);
        local = localtime(&timer);
        printOLEDText(3, 32, "Date : %04d/%02d/%02d", 
                      local->tm_year + 1900,
                      local->tm_mon + 1,
                      local->tm_mday);
        printOLEDText(3, 44, "Time :   %02d:%02d:%02d",
                      local->tm_hour,
                      local->tm_min,
                      local->tm_sec);

        flg++;
        printOLEDText(112, 1, "%c", ROT[flg%5]);
        sec_prev = local->tm_sec;

        display();
    }
#endif
    mraa_i2c_stop(i2c);
    return 0;
}

static void init() 
{
    resetAndWaitStable();              // Device reset and Wait stable

    sendCommand(SSD_DISPLAY_OFF);
    stopscroll();
    sendCommandByte(SSD1306_SETDISPLAYCLOCKDIV, 0x80);
    sendCommandByte(SSD1306_SETMULTIPLEX, MULTIPLEX); 
    sendCommandByte(SSD1306_SETDISPLAYOFFSET, 0x00);
    sendCommand(SSD1306_SETSTARTLINE | 0);
    sendCommandByte(SSD1306_CHARGEPUMP, CHARGEPUMP); 
    sendCommand(SSD1306_SEGREMAP | 0x1);
    sendCommand(SSD1306_COMSCANDEC);
    sendCommandByte(SSD1306_SETCOMPINS, COMPINS);
    sendCommandByte(SSD_SET_CONTRASTLEVEL, CONTRAST);
    sendCommandByte(SSD1306_SETPRECHARGE, PRECHARGE);
    sendCommandByte(SSD1306_SETVCOMDETECT, 0x40);
    sendCommand(SSD1306_DISPLAYALLON_RESUME);
    sendCommand(SSD1306_NORMAL_DISPLAY);
    sendCommand(SSD_DISPLAY_ON);
    // clearFrameBuffer();
    display();
    usleep(5000);
}

static void sendCommand(uint8_t cmd)
{
    uint8_t buf[2];
    buf[0] = SSD_COMMAND_MODE;
    buf[1] = cmd;
    mraa_i2c_write(i2c, buf, sizeof(buf));
}

static void sendCommandByte(uint8_t cmd, uint8_t data)
{
    uint8_t buf[3];
    buf[0] = SSD_COMMAND_MODE;
    buf[1] = cmd;
    buf[2] = data;
    mraa_i2c_write(i2c, buf, sizeof(buf));
}

static void sendCommandByte2(uint8_t cmd, uint8_t data1, uint8_t data2)
{
    uint8_t buf[4];
    buf[0] = SSD_COMMAND_MODE;
    buf[1] = cmd;
    buf[2] = data1;
    buf[3] = data2;
    mraa_i2c_write(i2c, buf, sizeof(buf));
}

static void resetAndWaitStable(void) 
{
     mraa_gpio_context gpio = mraa_gpio_init(GPIO_RESET);
     mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
     mraa_gpio_write(gpio, 0);
     usleep(RESET_TIME);   // Reset period
     mraa_gpio_write(gpio, 1);
     usleep(STABLE_WAIT);  // Wait for the device stable
}

static void enablePullup(int gpioport)
{
     mraa_gpio_context gpio = mraa_gpio_init(gpioport);
     if (gpio == NULL) {
          fprintf(stderr, "Cannot open GPIO port:%d\n", gpioport);
     }
     mraa_gpio_mode(gpio, MRAA_GPIO_PULLUP);
     mraa_gpio_close(gpio);
}
