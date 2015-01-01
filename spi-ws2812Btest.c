
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mraa.h"
#include "iotest.h"

#define SPI_PORT 2            // SPI Port that was connected to LCD
#define KHZ(x) (x * 1000)
#define MHZ(x) (x * 1000000)

//　Pixel Data
typedef struct {
     uint8_t pix[12];
} Pixel;

static void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
static void setAllPixel(uint8_t r, uint8_t g, uint8_t b);
static Pixel makePixelData(uint8_t r, uint8_t g, uint8_t b);
static void setbitPixData(Pixel *pix, int bitno, int fbit);
#define _ 0,
#define I 1,
#define O 2,
#define X 3,
static const uint8_t Greetings[][64] = {
    { _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ },
    { _ I _ _ I _ _ I _ _ I I I _ I I I _ I _ I _ _ I _ _ I _ I I I _ I _ _ _ I _ _ I _ I _ I I I _ _ I _ _ I I _ _ _ X _ _ O _ X _ _ },
    { _ I _ _ I _ I _ I _ I _ I _ I _ I _ I _ I _ _ I _ _ I _ I _ _ _ I _ _ _ I _ _ I _ I _ I _ _ _ I _ I _ I _ I _ _ O _ _ X _ X _ _ },
    { _ I _ _ I _ I _ I _ I _ I _ I _ I _ I _ I _ _ I _ _ I _ I _ _ _ I _ _ _ I _ _ I _ I _ I _ _ _ I _ I _ I _ I _ _ X _ _ O _ X _ _ },
    { _ I I I I _ I I I _ I I I _ I I I _ I _ I _ _ I I _ I _ I I I _ I _ I _ I _ _ I _ I _ I I I _ I I I _ I I _ _ _ O _ _ X _ X _ _ },
    { _ I _ _ I _ I _ I _ I _ _ _ I _ _ _ _ I _ _ _ I _ I I _ I _ _ _ I _ I _ I _ _ _ I _ _ I _ _ _ I _ I _ I _ I _ _ _ _ _ _ _ _ _ _ },
    { _ I _ _ I _ I _ I _ I _ _ _ I _ _ _ _ I _ _ _ I _ _ I _ I _ _ _ I _ I _ I _ _ _ I _ _ I _ _ _ I _ I _ I _ I _ _ X _ _ O _ X _ _ },
    { _ I _ _ I _ I _ I _ I _ _ _ I _ _ _ _ I _ _ _ I _ _ I _ I I I _ _ I _ I _ _ _ _ I _ _ I I I _ I _ I _ I _ I _ _ O _ _ X _ X _ _ },
};
#undef _
#undef I
#undef O
#undef X


uint8_t buf[12*64]; // 8x8(1LED=24bit*3+1) 
int spiWS1812BTest(void) 
{
    int x, y, i;
    mraa_spi_context spi;
    spi = mraa_spi_init(SPI_PORT);
    if (spi == NULL) {
        fprintf(stderr, "Cannot open SPI port:%d\n", SPI_PORT);
        exit(1);
    }
    mraa_spi_frequency(spi, KHZ(2750));
    mraa_spi_mode(spi, MRAA_SPI_MODE0);
    mraa_spi_bit_per_word(spi, 32);
    /*
    for (i=0;;i++) {
        //        setAllPixel(i & 0xFF, i & 0xFF, i & 0xFF);
        setAllPixel(255, 255, 255);
        for (j=0; j < 8; j++) {
            setPixel(j + (i%8), j, 0, 0, 0);
        }
        mraa_spi_transfer_buf(spi, buf, NULL, sizeof(buf));
        usleep(100000);
    }
    */
    while (1) {
        for (i=0; ; i++) {
            setAllPixel(0, 0, 0);
            for (y=0; y < 8; y++) {
                for (x=0; x < 8; x++) {
                    uint8_t c;
                    int xr = x + (i % 64);
                    c = Greetings[y][xr % 64];
                    switch (c) {
                    case 0: setPixel(x, y, 0, 0, 0);       break;
                    case 1: setPixel(x, y, 255, 255, 255); break;
                    case 2: setPixel(x, y, 255, 0, 0);     break;
                    case 3: setPixel(x, y, 255, 255, 255);     break;
                    }
                }
            }
            mraa_spi_transfer_buf(spi, buf, NULL, sizeof(buf));
            usleep(150000);
        }
    }
    return 0;
}


static void setAllPixel(uint8_t r, uint8_t g, uint8_t b)
{
     int x, y;


     for (y=0; y < 8; y++) {
          for (x=0; x < 8; x++) {
               setPixel(x, y, r, g, b);
          }
     }
}

static void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) 
{
    int adr = y*96 + x*12;
    Pixel pixel = makePixelData(r, g, b);
    memcpy(buf + adr, &pixel, sizeof(pixel));
}

static Pixel makePixelData(uint8_t r, uint8_t g, uint8_t b) 
{
     int i, j;
     Pixel result;
     uint32_t d24 = ((uint32_t)g << 16) | ((uint32_t)r << 8) | ((uint32_t)b << 0);
     memset(&result, 0, sizeof(result));
     for (i=0, j=0; i < 24; i++) {
          if (d24 & 0x00800000) {
              setbitPixData(&result, j++, 1);
              setbitPixData(&result, j++, 1);
              setbitPixData(&result, j++, 1);
              setbitPixData(&result, j++, 0);
          }
          else {
              setbitPixData(&result, j++, 1);
              setbitPixData(&result, j++, 0);
              setbitPixData(&result, j++, 0);
              setbitPixData(&result, j++, 0);
          }
          //          if (j % 16 == 15) {
          //               setbitPixData(&result, j++, 0);
          //          }
          d24 = d24 << 1;
     }
      return result;
}

static void setbitPixData(Pixel *pix, int bitno, int fbit)
{
     int idx8 = bitno / 8;
     int idx = bitno % 8;
     uint8_t bit = 0x80 >> idx;
     if (fbit) {
          (pix->pix)[idx8] |= bit;
     } else {
          (pix->pix)[idx8] &= ~bit;
     }
}
