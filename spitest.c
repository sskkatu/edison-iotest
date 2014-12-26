#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wand/MagickWand.h>
#include "mraa.h"
#include "iotest.h"

#define SPI_PORT 2            // SPI Port that was connected to LCD
#define GPIO_RESET_PORT 26    // GPIO-130(UART1_RX) in Edison
#define GPIO_DC_RCS_PORT 35   // GPIO-131(UART2_TX) in Edison

static void initLcd(mraa_spi_context spi);
static void resetLcd(void);
static void writeCmd(mraa_spi_context spi, uint8_t cmd);
static void writeData(mraa_spi_context spi, uint8_t cmd);
static void setCommandMode(void);
static void setDataMode(void);
static void addset(mraa_spi_context spi, int x, int y);
static void writeData16(mraa_spi_context spi, unsigned short d);
static void writeData240(mraa_spi_context spi, uint8_t *buf);
static void writeDataBlock(mraa_spi_context spi, uint8_t *buf, size_t size);
static void dspSingleColor(mraa_spi_context spi, unsigned short color);
static void drawBitmap(mraa_spi_context spi, unsigned char *bmp);

// GPIO defines and declarations
static mraa_gpio_context gpio_dc_rcs;  // GPIO OUT : DC_RCS
static mraa_gpio_context gpio_reset;   // GPIO OUT : LCD Reset
#define DC_RCS_STATUS_UNINIT  -1   // DC_RCS is Uninitialized 
#define DC_RCS_STATUS_COMMAND  0   // Command Mode (Low)
#define DC_RCS_STATUS_DATA     1   // Data Mode (High)
static int gpio_dc_rcs_status = DC_RCS_STATUS_UNINIT;  //  Latest DC_RCS Status
#define SPI_WRITE_BLOCK_MAX 4096

// LCD pixel structure (RGB = 3:3:3)
typedef union {
     struct {
          unsigned short green  : 3;
          unsigned short dummy  : 2;
          unsigned short red    : 3;
          unsigned short dummy2 : 2;
          unsigned short blue   : 3;
          unsigned short dummy3 : 2;
     } color;
     unsigned short s;
} bit16_color;

#define MHZ(x) (x * 1000000)
unsigned char *bitmap = NULL;

int spiTest(void) 
{
    mraa_spi_context spi;
	int fd;
    struct stat buf;
    printf("%d\n", sizeof(bit16_color));
    gpio_dc_rcs = mraa_gpio_init(GPIO_DC_RCS_PORT);
    mraa_gpio_dir(gpio_dc_rcs, MRAA_GPIO_OUT);
    gpio_reset = mraa_gpio_init(GPIO_RESET_PORT);
    mraa_gpio_dir(gpio_reset, MRAA_GPIO_OUT);
    spi = mraa_spi_init(SPI_PORT);
    if (spi == NULL) {
    	fprintf(stderr, "Cannot open SPI port:%d\n", SPI_PORT);
        exit(1);
    }
    resetLcd();
    mraa_spi_frequency(spi, MHZ(5));
    mraa_spi_mode(spi, MRAA_SPI_MODE0);
    initLcd(spi);
    printf("initLcd Done.\n");
    fd = open("lena.bmp", O_RDONLY);
    if (fd == -1) {
         fprintf(stderr, "file not found!\n");
         exit(1);
    }
    fstat(fd, &buf);
    bitmap = malloc(buf.st_size);
    read(fd, bitmap, buf.st_size);

    while (1) {
         bit16_color col;
         printf("1) GREEN\n");
         col.s = 0x0000;
         col.color.green = 0x07;
         dspSingleColor(spi, col.s);
         printf("2) RED\n");
         col.s = 0x0000;
         col.color.red = 0x07;
         dspSingleColor(spi, col.s);
         printf("3) BLUE\n");
         col.s = 0x0000;
         col.color.blue = 0x07;
         dspSingleColor(spi, col.s);
         printf("4) YELLOW\n");
         col.s = 0x0000;
         col.color.green = 0x07;
         col.color.red = 0x07;
         dspSingleColor(spi, col.s);
         printf("5) CYAN\n");
         col.s = 0x0000;
         col.color.green = 0x07;
         col.color.blue = 0x07;
         dspSingleColor(spi, col.s);
         printf("6) PURPLE\n");
         col.s = 0x0000;
         col.color.red = 0x07;
         col.color.blue = 0x07;
         dspSingleColor(spi, col.s);
         printf("7) WHITE\n");
         col.s = 0x0000;
         col.color.red = 0x07;
         col.color.green = 0x07;
         col.color.blue = 0x07;
         dspSingleColor(spi, col.s);
         printf("8) BLACK\n");
         col.s = 0x0000;
         dspSingleColor(spi, col.s);
         printf("9) Draw Bitmap\n");
         drawBitmap(spi, bitmap);
    }
    return 1;
}

static void drawBitmapfromFile(mraa_spi_context spi, const char* imageFilePath)
{
     MagickWand *wand = NewMagickWand();
     PixelWand **pixelwand;
     PixelIterator *pixelIter;
     int i;

     MagickWandGenesis();
     wand = NewMagickWand();

     if (MagickReadImage(wand, imageFilePath) == MagickFalse) {
          fprintf(stderr, "file \"%s\" not found!", imageFilePath);
          exit(1);
     }
     pixelIter = NewPixelIterator(wand);
     addset(spi, 0, 0);
     for (i=0; i < 320; i++) {
//          pixelwand = PixelGetNextIteratorRow(pixelIter, );
     }
}


static void drawBitmap(mraa_spi_context spi, unsigned char *bmp) 
{
     int i, j;
     unsigned char b, g, r;
     bit16_color pixel;
     unsigned short buf[240];
     addset(spi, 0, 0);

     for (i=0; i < 320; i++) {
          for (j=0; j < 240; j++) {
               b = *bmp++;
               g = *bmp++;
               r = *bmp++;
               pixel.s = 0;

               pixel.color.blue = b / 32;
               pixel.color.green = g / 32;
               pixel.color.red = r / 32;
               buf[240-j] = pixel.s;
          }
          writeData240(spi, (unsigned char*)buf);
     }
}

static void dspSingleColor(mraa_spi_context spi, unsigned short color)
{
	bit16_color buf[SPI_WRITE_BLOCK_MAX / sizeof(bit16_color)];
    unsigned int i;
    memset(buf, 0, sizeof(buf));
    for (i=0; i < sizeof(buf) / sizeof(bit16_color); i++) {
         buf[i].s = color;
    }
    
    addset(spi, 0, 0);
    for (i = 0; i < 320*240; i += SPI_WRITE_BLOCK_MAX / sizeof(bit16_color)) {
         writeDataBlock(spi, (uint8_t*)buf, sizeof(buf));
    }
}

static void initLcd(mraa_spi_context spi) 
{
     writeCmd(spi, 0xcb);
     writeData(spi, 0x39);
     writeData(spi, 0x2c);
     writeData(spi, 0x00);
     writeData(spi, 0x34);
     writeData(spi, 0x02);

     writeCmd(spi, 0xcf);
     writeData(spi, 0x00);
     writeData(spi, 0xc1);
     writeData(spi, 0x30);

     writeCmd(spi, 0xe8);
     writeData(spi, 0x85);
     writeData(spi, 0x00);
     writeData(spi, 0x78);

     writeCmd(spi, 0xea);
     writeData(spi, 0x00);
     writeData(spi, 0x00);

     writeCmd(spi, 0xed);
     writeData(spi, 0x64);
     writeData(spi, 0x03);
     writeData(spi, 0x12);
     writeData(spi, 0x81);

     writeCmd(spi, 0xF7);
     writeData(spi, 0x20);

     writeCmd(spi, 0xc0);
     writeData(spi, 0x23);

     writeCmd(spi, 0xc1);
     writeData(spi, 0x10);
     
     writeCmd(spi, 0xc5);
     writeData(spi, 0x3e);
     writeData(spi, 0x28);
     
     writeCmd(spi, 0xc7);
     writeData(spi, 0x86);

     writeCmd(spi, 0x36);
     writeData(spi, 0x48);

     writeCmd(spi, 0x3A);
     writeData(spi, 0x55);

     writeCmd(spi, 0xb1);
     writeData(spi, 0x00);
     writeData(spi, 0x18);

     writeCmd(spi, 0xb6);
     writeData(spi, 0x08);
     writeData(spi, 0x82);
     writeData(spi, 0x27);

     writeCmd(spi, 0xf2);
     writeData(spi, 0x00);

     writeCmd(spi, 0x26);
     writeData(spi, 0x01);
     
     writeCmd(spi, 0xe0);
     writeData(spi, 0x0f);
     writeData(spi, 0x31);
     writeData(spi, 0x2b);
     writeData(spi, 0x0c);
     writeData(spi, 0x0e);
     writeData(spi, 0x08);
     writeData(spi, 0x4e);
     writeData(spi, 0xf1);
     writeData(spi, 0x37);
     writeData(spi, 0x07);
     writeData(spi, 0x10);
     writeData(spi, 0x03);
     writeData(spi, 0x0e);
     writeData(spi, 0x09);
     writeData(spi, 0x00);
     
     writeCmd(spi, 0xe1);
     writeData(spi, 0x00);
     writeData(spi, 0x0e);
     writeData(spi, 0x14);
     writeData(spi, 0x03);
     writeData(spi, 0x11);
     writeData(spi, 0x07);
     writeData(spi, 0x31);
     writeData(spi, 0xc1);
     writeData(spi, 0x48);
     writeData(spi, 0x08);
     writeData(spi, 0x0f);
     writeData(spi, 0x0c);
     writeData(spi, 0x31);
     writeData(spi, 0x36);
     writeData(spi, 0x0f);

     writeCmd(spi, 0x11);

     usleep(120000);

     writeCmd(spi, 0x29);
     writeCmd(spi, 0x2c);
}


static void addset(mraa_spi_context spi, int x, int y) 
{
     writeCmd(spi, 0x2a);
     writeData16(spi, (unsigned short)x);
     writeCmd(spi, 0x2b);
     writeData16(spi, (unsigned short)y);
     writeCmd(spi, 0x2c);
}


static void writeData16(mraa_spi_context spi, unsigned short d)
{
     uint8_t buf[2];
     setDataMode();
     memcpy(buf, &d, sizeof(buf));
     mraa_spi_write_buf(spi, buf, sizeof(buf));
}

static void writeData240(mraa_spi_context spi, uint8_t *buf)
{
     writeDataBlock(spi, buf, 480);
}

static void writeDataBlock(mraa_spi_context spi, uint8_t *buf, size_t size)
{
     setDataMode();
     mraa_spi_transfer_buf(spi, buf, NULL, size);
}

static void writeCmd(mraa_spi_context spi, uint8_t cmd) 
{
     setCommandMode();
     mraa_spi_write(spi, cmd);
}

static void writeData(mraa_spi_context spi, uint8_t cmd) 
{
     setDataMode();
     mraa_spi_write(spi, cmd);
}

static void resetLcd(void) 
{
     mraa_gpio_write(gpio_reset, 0);
     usleep(10000); // 100ms
     mraa_gpio_write(gpio_reset, 1);
     usleep(10000); // 100ms
}

static void setCommandMode(void) 
{
     if (gpio_dc_rcs_status == DC_RCS_STATUS_UNINIT || 
         gpio_dc_rcs_status == DC_RCS_STATUS_DATA) {
          gpio_dc_rcs_status = DC_RCS_STATUS_COMMAND;
          mraa_gpio_write(gpio_dc_rcs, gpio_dc_rcs_status);
     }
}

static void setDataMode(void) 
{
     if (gpio_dc_rcs_status == DC_RCS_STATUS_UNINIT ||
         gpio_dc_rcs_status == DC_RCS_STATUS_COMMAND) {
          gpio_dc_rcs_status = DC_RCS_STATUS_DATA;
          mraa_gpio_write(gpio_dc_rcs, gpio_dc_rcs_status);
     }
}
