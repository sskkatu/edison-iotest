#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mraa.h"
#include "iotest.h"

#define SPI_PORT 2            // SPI Port that was connected to *LED*
#define KHZ(x) ((x)*1000)     // KHz Frequency 
#define MHZ(x) ((x)*1000000)  // MHz Frequency

// LED Matrix X/Y Size
#define LEDMATRIX_XMAX 8
#define LEDMATRIX_YMAX 8
#define LEDMATRIX_ALL  (LEDMATRIX_XMAX * LEDMATRIX_YMAX)
#define LEDMATRIX_PIXEL_PER_BIT 24

// SPI Signals number to the represent of 1-bit for ws2812Btest LED chip.
#define SIGNALS_PER_BIT 4

// One Pixel Data
typedef struct {
    uint8_t pix[SIGNALS_PER_BIT*LEDMATRIX_PIXEL_PER_BIT/8];
} Pixel;

// SPI signals defnition for the represent of 1-bit.
const int SIGNAL_BIT_ZERO[SIGNALS_PER_BIT] = { 1, 0, 0, 0 };
const int SIGNAL_BIT_ONE[SIGNALS_PER_BIT]  = { 1, 1, 1, 0 };

// Greetings banner definition
#define GREETINGS_WIDTH 64
#define _ 0,
#define I 1,
#define O 2,
#define X 3,
#define Y 4,
#define Z 5,
static const uint8_t Greetings[][GREETINGS_WIDTH] = {
    { _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ },
    { _ I _ _ I _ _ I _ _ I I _ _ I I _ _ I _ I _ _ I _ _ I _ I I I _ I _ _ _ I _ _ I _ I _ I I I _ _ I _ _ I I _ _ _ X _ O _ Y _ Z _ },
    { _ I _ _ I _ I _ I _ I _ I _ I _ I _ I _ I _ _ I _ _ I _ I _ _ _ I _ _ _ I _ _ I _ I _ I _ _ _ I _ I _ I _ I _ _ X _ O _ Y _ Z _ },
    { _ I _ _ I _ I _ I _ I _ I _ I _ I _ I _ I _ _ I I _ I _ I _ _ _ I _ _ _ I _ _ I _ I _ I _ _ _ I _ I _ I _ I _ _ X _ O _ Y _ Z _ },
    { _ I I I I _ I I I _ I I _ _ I I _ _ _ I _ _ _ I I _ I _ I I I _ I _ _ _ I _ _ _ I _ _ I I I _ I I I _ I I _ _ _ X _ O _ Y _ Z _ },
    { _ I _ _ I _ I _ I _ I _ _ _ I _ _ _ _ I _ _ _ I _ I I _ I _ _ _ I _ I _ I _ _ _ I _ _ I _ _ _ I _ I _ I _ I _ _ X _ O _ Y _ Z _ },
    { _ I _ _ I _ I _ I _ I _ _ _ I _ _ _ _ I _ _ _ I _ _ I _ I _ _ _ I I _ I I _ _ _ I _ _ I _ _ _ I _ I _ I _ I _ _ _ _ _ _ _ _ _ _ },
    { _ I _ _ I _ I _ I _ I _ _ _ I _ _ _ _ I _ _ _ I _ _ I _ I I I _ I _ _ _ I _ _ _ I _ _ I I I _ I _ I _ I _ I _ _ X _ O _ Y _ Z _ },
};
#undef _
#undef I
#undef O
#undef X
#undef Y
#undef Z

// Local functions prototype
static void    setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
static void    setAllPixel(uint8_t r, uint8_t g, uint8_t b);
static Pixel   makePixelData(uint8_t r, uint8_t g, uint8_t b);
static void    setbitPixData(Pixel *pix, int bitno, int isSetBit);
static uint8_t reverseByteBit(uint8_t in);

// SPI Send Data Buffer (represent of ws2812Btest signal spec)
uint8_t buf[sizeof(Pixel)*64];

int spiWS1812BTest(void) 
{
    int x, y, i;
    mraa_spi_context spi;
    spi = mraa_spi_init(SPI_PORT);
    if (spi == NULL) {
        fprintf(stderr, "Cannot open SPI port:%d\n", SPI_PORT);
        exit(1);
    }
    mraa_spi_frequency(spi, KHZ(3000));
    mraa_spi_mode(spi, MRAA_SPI_MODE0);
    mraa_spi_bit_per_word(spi, 32);
    mraa_spi_lsbmode(spi, 0);
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
    for (i=0; ; i++) {
        for (y=0; y < LEDMATRIX_YMAX; y++) {
            for (x=0; x < LEDMATRIX_XMAX; x++) {
                uint8_t c;
                int xr = x + (i % GREETINGS_WIDTH);
                c = Greetings[y][xr % GREETINGS_WIDTH];
                switch (c) {
                case 0: setPixel(x, y, 0, 0, 0);           break; // BLACK
                case 1: setPixel(x, y, 0xFF, 0xFF, 0xFF);  break; // WHITE
                case 2: setPixel(x, y, 0xFF, 0, 0);        break; // RED
                case 3: setPixel(x, y, 0, 0xFF, 0xFF);     break; // CYAN
                case 4: setPixel(x, y, 0xFF, 0, 0xFF);     break; // MAGENTA
                case 5: setPixel(x, y, 0xFF, 0xFF, 0);     break; // YELLOW
                }
            }
        }
        mraa_spi_transfer_buf(spi, buf, NULL, sizeof(buf));
        usleep(150000);
    }
    return 0;
}

static void setAllPixel(uint8_t r, uint8_t g, uint8_t b)
{
     int x, y;

     for (y=0; y < LEDMATRIX_YMAX; y++) {
          for (x=0; x < LEDMATRIX_XMAX; x++) {
               setPixel(x, y, r, g, b);
          }
     }
}

static void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) 
{
    int adr = y*LEDMATRIX_XMAX*sizeof(Pixel) + x*sizeof(Pixel);
    Pixel pixel = makePixelData(r, g, b);
    memcpy(buf + adr, &pixel, sizeof(pixel));
}

static Pixel makePixelData(uint8_t r, uint8_t g, uint8_t b) 
{
    int i, j, k;
    const int *signalData = NULL;
    Pixel result;
    uint32_t d = ((uint32_t)g << 16) | ((uint32_t)r << 8) | ((uint32_t)b << 0);
    memset(&result, 0, sizeof(result));
    for (i=0, j=0; i < LEDMATRIX_PIXEL_PER_BIT; i++) {
        if (d & 0x00800000) {
            signalData = SIGNAL_BIT_ONE;
        } else {
            signalData = SIGNAL_BIT_ZERO;
        }
        for (k=0; k < SIGNALS_PER_BIT; k++) {
            setbitPixData(&result, j++, signalData[k]);
        }
        d <<= 1;
    }
    return result;
}

static void setbitPixData(Pixel *pix, int bitno, int isSetBit)
{
     int idxByte = bitno / 8;
     int idxBit = bitno % 8;
     uint8_t bitmap = 0x80 >> idxBit;

     if (isSetBit) {
          (pix->pix)[idxByte] |= bitmap;
     } else {
          (pix->pix)[idxByte] &= ~bitmap;
     }
}

static uint8_t reverseByteBit(uint8_t in) 
{
    int i;
    uint8_t out = 0;

    for (i=0; i < 8; i++) {
        if (in & 0x1) {
            out |= 0x1;
        } 
        out <<= 1;
        in >>= 1;
    }
    return out;
}
