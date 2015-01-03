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
#define BRIGHTNESS_LIMIT 0x100

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
    { _ I _ _ I _ _ I _ _ I I _ _ I I _ _ I _ I _ _ I _ _ I _ I I I _ I _ _ _ I _ _ I _ I _ I I I _ _ I _ _ I I I _ _ X _ O _ Y _ Z _ },
    { _ I _ _ I _ I _ I _ I _ I _ I _ I _ I _ I _ _ I _ _ I _ I _ _ _ I _ _ _ I _ _ I _ I _ I _ _ _ I _ I _ I _ _ I _ X _ O _ Y _ Z _ },
    { _ I _ _ I _ I _ I _ I _ I _ I _ I _ I _ I _ _ I I _ I _ I _ _ _ I _ _ _ I _ _ I _ I _ I _ _ _ I _ I _ I _ _ I _ X _ O _ Y _ Z _ },
    { _ I I I I _ I I I _ I I _ _ I I _ _ I _ I _ _ I I _ I _ I I I _ I _ _ _ I _ _ I _ I _ I I I _ I I I _ I I I _ _ X _ O _ Y _ Z _ },
    { _ I _ _ I _ I _ I _ I _ _ _ I _ _ _ _ I _ _ _ I _ I I _ I _ _ _ I _ I _ I _ _ _ I _ _ I _ _ _ I _ I _ I _ _ I _ X _ O _ Y _ Z _ },
    { _ I _ _ I _ I _ I _ I _ _ _ I _ _ _ _ I _ _ _ I _ _ I _ I _ _ _ I I _ I I _ _ _ I _ _ I _ _ _ I _ I _ I _ _ I _ _ _ _ _ _ _ _ _ },
    { _ I _ _ I _ I _ I _ I _ _ _ I _ _ _ _ I _ _ _ I _ _ I _ I I I _ I _ _ _ I _ _ _ I _ _ I I I _ I _ I _ I _ _ I _ X _ O _ Y _ Z _ },
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
static uint8_t reverseByteBit2(uint8_t in);

// SPI Send Data Buffer (represent of ws2812Btest signal spec)
uint8_t buf[sizeof(Pixel)*64*2];

int spiWS1812BTest(void) 
{
    int x, y, i, j;
    uint32_t c;
    mraa_spi_context spi;
    spi = mraa_spi_init(SPI_PORT);

    if (spi == NULL) {
        fprintf(stderr, "Cannot open SPI port:%d\n", SPI_PORT);
        exit(1);
    }
    mraa_spi_frequency(spi, KHZ(2750));
    mraa_spi_mode(spi, MRAA_SPI_MODE0);
    mraa_spi_bit_per_word(spi, 32);
    mraa_spi_lsbmode(spi, 0);

#if (1)
    for (i=0;;i++) {
        for ( y = 0; y < 8; y++) {
            for ( x = 0; x < 16; x++) {
                int bl = BRIGHTNESS_LIMIT;
#if (1)
                uint8_t r = (i + (16-x)*4 + y*8)%bl;
                uint8_t g = (i + x*8 + (8-y)*4)%bl;
                uint8_t b = (i + (x+y)*12)%bl;
#endif
#if (0)
                uint8_t r = random()%bl;
                uint8_t g = random()%bl;
                uint8_t b = random()%bl;
#endif
                if (i%(bl*2) >= bl) {
                    r = r * (bl-i%bl)/bl;
                    g = g * (bl-i%bl)/bl;
                    b = b * (bl-i%bl)/bl;
                } else {
                    r = r * (i%bl)/bl;
                    g = g * (i%bl)/bl;
                    b = b * (i%bl)/bl;
                }
                setPixel(x, y, r, g, b);
            }
        }
        mraa_spi_transfer_buf(spi, buf, NULL, sizeof(buf));
        //        usleep(20000);
    }
#endif
    for (i=0; ; i++) {
        for (y=0; y < LEDMATRIX_YMAX; y++) {
            for (x=0; x < LEDMATRIX_XMAX*2; x++) {
                uint8_t c;
                int xr = x + (i % GREETINGS_WIDTH);
                int br = BRIGHTNESS_LIMIT;
                c = Greetings[y][xr % GREETINGS_WIDTH];
                switch (c) {
                case 0: setPixel(x, y, 0, 0, 0);           break; // BLACK
                case 1: setPixel(x, y, br, br, br);  break; // WHITE
                case 2: setPixel(x, y, br, 0, 0);        break; // RED
                case 3: setPixel(x, y, 0, br, br);     break; // CYAN
                case 4: setPixel(x, y, br, 0, br);     break; // MAGENTA
                case 5: setPixel(x, y, 0 ,br, 0);        break; // YELLOW
                }
            }
        }
        mraa_spi_transfer_buf(spi, buf, NULL, sizeof(buf));
        usleep(100000);
    }
    return 0;
}

static void setAllPixel(uint8_t r, uint8_t g, uint8_t b)
{
     int x, y;

     for (y=0; y < LEDMATRIX_YMAX*2; y++) {
          for (x=0; x < LEDMATRIX_XMAX; x++) {
               setPixel(x, y, r, g, b);
          }
     }
}

static void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) 
{
    int adr;

    if (x >= 8) {
        y = y + 8;
        x = x - 8;
    }
    adr = y*LEDMATRIX_XMAX*sizeof(Pixel) + x*sizeof(Pixel);

    Pixel pixel = makePixelData(r, g, b);
    memcpy(buf + adr, &pixel, sizeof(pixel));
}

static Pixel makePixelData(uint8_t r, uint8_t g, uint8_t b) 
{
    int i, j, k;
    const int *signalData = NULL;
    Pixel result;
    uint32_t d;
    r = reverseByteBit(r);
    g = reverseByteBit(g);
    b = reverseByteBit(b);
    d = ((uint32_t)g << 16) | ((uint32_t)r << 8) | ((uint32_t)b << 0);
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
    if (in >= BRIGHTNESS_LIMIT) { // limitter
        in = BRIGHTNESS_LIMIT;
    }
    static const uint8_t BRIGHTNESS_TABLE[256] = {
        0x00, 0x40, 0x80, 0xc0, 0x10, 0x50, 0x90, 0xd0,
        0x20, 0x60, 0xa0, 0xe0, 0x30, 0x70, 0xb0, 0xf0, 
        0x04, 0x44, 0x84, 0xc4, 0x14, 0x54, 0x94, 0xd4, 
        0x24, 0x64, 0xa4, 0xe4, 0x34, 0x74, 0xb4, 0xf4, 
        0x08, 0x48, 0x88, 0xc8, 0x18, 0x58, 0x98, 0xd8, 
        0x28, 0x68, 0xa8, 0xe8, 0x38, 0x78, 0xb8, 0xf8, 
        0x0c, 0x4c, 0x8c, 0xcc, 0x1c, 0x5c, 0x9c, 0xdc, 
        0x2c, 0x6c, 0xac, 0xec, 0x3c, 0x7c, 0xbc, 0xfc, 
        0x01, 0x41, 0x81, 0xc1, 0x11, 0x51, 0x91, 0xd1, 
        0x21, 0x61, 0xa1, 0xe1, 0x31, 0x71, 0xb1, 0xf1, 
        0x05, 0x45, 0x85, 0xc5, 0x15, 0x55, 0x95, 0xd5, 
        0x25, 0x65, 0xa5, 0xe5, 0x35, 0x75, 0xb5, 0xf5, 
        0x09, 0x49, 0x89, 0xc9, 0x19, 0x59, 0x99, 0xd9, 
        0x29, 0x69, 0xa9, 0xe9, 0x39, 0x79, 0xb9, 0xf9, 
        0x0d, 0x4d, 0x8d, 0xcd, 0x1d, 0x5d, 0x9d, 0xdd, 
        0x2d, 0x6d, 0xad, 0xed, 0x3d, 0x7d, 0xbd, 0xfd, 
        0x02, 0x42, 0x82, 0xc2, 0x12, 0x52, 0x92, 0xd2, 
        0x22, 0x62, 0xa2, 0xe2, 0x32, 0x72, 0xb2, 0xf2, 
        0x06, 0x46, 0x86, 0xc6, 0x16, 0x56, 0x96, 0xd6, 
        0x26, 0x66, 0xa6, 0xe6, 0x36, 0x76, 0xb6, 0xf6, 
        0x0a, 0x4a, 0x8a, 0xca, 0x1a, 0x5a, 0x9a, 0xda, 
        0x2a, 0x6a, 0xaa, 0xea, 0x3a, 0x7a, 0xba, 0xfa, 
        0x0e, 0x4e, 0x8e, 0xce, 0x1e, 0x5e, 0x9e, 0xde, 
        0x2e, 0x6e, 0xae, 0xee, 0x3e, 0x7e, 0xbe, 0xfe, 
        0x03, 0x43, 0x83, 0xc3, 0x13, 0x53, 0x93, 0xd3, 
        0x23, 0x63, 0xa3, 0xe3, 0x33, 0x73, 0xb3, 0xf3, 
        0x07, 0x47, 0x87, 0xc7, 0x17, 0x57, 0x97, 0xd7, 
        0x27, 0x67, 0xa7, 0xe7, 0x37, 0x77, 0xb7, 0xf7, 
        0x0b, 0x4b, 0x8b, 0xcb, 0x1b, 0x5b, 0x9b, 0xdb, 
        0x2b, 0x6b, 0xab, 0xeb, 0x3b, 0x7b, 0xbb, 0xfb, 
        0x0f, 0x4f, 0x8f, 0xcf, 0x1f, 0x5f, 0x9f, 0xdf, 
        0x2f, 0x6f, 0xaf, 0xef, 0x3f, 0x7f, 0xbf, 0xff,    
    };
    return BRIGHTNESS_TABLE[in];
    /* This table is generating by code of followings: */
    /*
    int i;
    uint8_t out = 0;
    uint8_t result = 0;

    for (i=0; i < 8; i++) {
        out <<= 1;
        out |= (in & 0x01);
        in >>= 1;
    }
    for (i=0; i < 4; i++) {
        int b1, b2;
        b1 = out & 0x80 ? 1 : 0;
        b2 = out & 0x40 ? 1 : 0;
        out <<= 2;
        result <<= 2;
        result |= b2 ? 0x02 : 0x00;
        result |= b1 ? 0x01 : 0x00;
    }
    return result;
    */
}
