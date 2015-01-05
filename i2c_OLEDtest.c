#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define CONTRAST      0x9F
#define CHARGEPUMP    0x10
#define PRECHARGE     0x22
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

int i2cOLEDTest(void)
{
    int i, j;
    enablePullup(GPIO_I2C1SDA);		// i2c1 SDA
    enablePullup(GPIO_I2C1SCL);		// i2c1 SCL

    i2c = mraa_i2c_init(I2CPORT);
    if (i2c == NULL) {
        fprintf(stderr, "Cannot open i2c port:%d\n", I2CPORT);
        exit(1);
    }

    mraa_i2c_frequency(i2c, MRAA_I2C_HIGH);
    mraa_i2c_address(i2c, I2CADR);
    init();
    for (i=0; ; i++) {
        //        for (j=0; j < sizeof(frameBuf) ; j++) {
        //            frameBuf[j] = 0x5555;
        //        }
        int x, y;
        uint8_t *p = frameBuf;
        for (j=0; j < 128*8; j++) {
            p[j] = glcdfont[(j+i*5) % GLCDFONT_DATASIZE];
        }
        display();
    }
    mraa_i2c_stop(i2c);
    return 0;
}

static void init() 
{
    memset(frameBuf, 0, sizeof(frameBuf));

    resetAndWaitStable();              // Device reset and Wait stable

	sendCommand(SSD_DISPLAY_OFF);
	sendCommandByte(SSD1306_SETDISPLAYCLOCKDIV, 0x80);
	sendCommandByte(SSD1306_SETMULTIPLEX, MULTIPLEX); 
	sendCommandByte(SSD1306_SETDISPLAYOFFSET, 0x00);
	sendCommand(SSD1306_SETSTARTLINE | 0x00);
	sendCommandByte(SSD1306_CHARGEPUMP, CHARGEPUMP); 
	sendCommandByte(SSD1306_MEMORYMODE, 0x00);
	sendCommand(SSD1306_SEGREMAP | 0x1);
	sendCommand(SSD1306_COMSCANDEC);
	sendCommandByte(SSD1306_SETCOMPINS, COMPINS);
	sendCommandByte(SSD_SET_CONTRASTLEVEL, CONTRAST);
	sendCommandByte(SSD1306_SETPRECHARGE, PRECHARGE);
   	sendCommandByte(SSD1306_SETVCOMDETECT, 0x40);
	sendCommand(SSD1306_DISPLAYALLON_RESUME);
	sendCommand(SSD1306_Normal_Display);
    sendCommandByte2(0x21, 0, 127); 
    sendCommandByte2(0x22, 0, 7); 
	stopscroll();
	sendCommand(SSD_DISPLAY_ON);

    usleep(5000);
    clearDisplay();
}

void invertDisplay(uint8_t i) 
{
    if (i) {
        sendCommand(SSD_INVERSE_DISPLAY);
    } else {
        sendCommand(SSD1306_Normal_Display);
    }
}

void startscrollright(uint8_t start, uint8_t stop)
{
	sendCommand(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	sendCommand(0X00);
	sendCommand(start);
	sendCommand(0X00);
	sendCommand(stop);
	sendCommand(0X01);
	sendCommand(0XFF);
	sendCommand(SSD_ACTIVATE_SCROLL);
}

void startscrollleft(uint8_t start, uint8_t stop)
{
	sendCommand(SSD1306_LEFT_HORIZONTAL_SCROLL);
	sendCommand(0X00);
	sendCommand(start);
	sendCommand(0X00);
	sendCommand(stop);
	sendCommand(0X01);
	sendCommand(0XFF);
	sendCommand(SSD_ACTIVATE_SCROLL);
}

void startscrolldiagright(uint8_t start, uint8_t stop)
{
	sendCommand(SSD1306_SET_VERTICAL_SCROLL_AREA);	
	sendCommand(0X00);
	sendCommand(LCDHEIGHT);
	sendCommand(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	sendCommand(0X00);
	sendCommand(start);
	sendCommand(0X00);
	sendCommand(stop);
	sendCommand(0X01);
	sendCommand(SSD_ACTIVATE_SCROLL);
}

void startscrolldiagleft(uint8_t start, uint8_t stop)
{
	sendCommand(SSD1306_SET_VERTICAL_SCROLL_AREA);	
	sendCommand(0X00);
	sendCommand(LCDHEIGHT);
	sendCommand(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	sendCommand(0X00);
	sendCommand(start);
	sendCommand(0X00);
	sendCommand(stop);
	sendCommand(0X01);
	sendCommand(SSD_ACTIVATE_SCROLL);
}

void stopscroll(void)
{
	sendCommand(SSD_DEACTIVATE_SCROLL);
}

void display(void) 
{
    int i, j;
    uint8_t buf[BYTE_PER_LCDWIDTH+1];
    uint8_t *pbuf = frameBuf;

    sendCommand(SSD1306_SETLOWCOLUMN  | 0x0); // low col = 0
    sendCommand(SSD1306_SETHIGHCOLUMN | 0x0); // hi col = 0
    sendCommand(SSD1306_SETSTARTLINE  | 0x0); // line #0    
    
    for (i=0; i < 8; i++) {
        sendCommandByte2(0xB0 + i, 0, 0x10);
        for (j = 0; j < 8; j++) {
            buf[0] = 0x40;
            memcpy(&buf[1], pbuf, BYTE_PER_LCDWIDTH);
            mraa_i2c_write(i2c, buf, sizeof(buf));
            pbuf += BYTE_PER_LCDWIDTH;
        }
    }
}

void clearDisplay(void) 
{
    int i, j;
    uint8_t buf[BYTE_PER_LCDWIDTH+2];

    sendCommand(SSD1306_SETLOWCOLUMN  | 0x0); // low col = 0
    sendCommand(SSD1306_SETHIGHCOLUMN | 0x0); // hi col = 0
    sendCommand(SSD1306_SETSTARTLINE  | 0x0); // line #0    

    buf[0] = 0x40;
    memset(&buf[1], 0, BYTE_PER_LCDWIDTH+1); // Add extra 4bit(+1)
    for (i=0; i < 8; i++) {
        sendCommandByte2(0xB0 + i, 0, 0x10);
        for (j = 0; j < 8; j++) {
            mraa_i2c_write(i2c, buf, sizeof(buf));
        }
    }
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
