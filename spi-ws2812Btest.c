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
#define LEDMATRIX_X 8     // Board Martix X Size
#define LEDMATRIX_Y 8     // Board Matrix Y Size
//#define LEDMATRIX_XMAX 8 // Whole Matrix X Size
#define LEDMATRIX_XMAX 16 // Whole Matrix X Size
#define LEDMATRIX_YMAX 8  // Whole Matrix Y Size
#define BRIGHTNESS_LIMIT 0xc0

// One Pixel Data
typedef struct {
    uint32_t g;
    uint32_t r;
    uint32_t b;
} Pixel;

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
static uint32_t regulateWordBit(uint8_t in);

// SPI Send Data Buffer (represent of ws2812Btest signal spec)
Pixel buf[LEDMATRIX_XMAX*LEDMATRIX_YMAX];

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
                uint8_t r = (i + (4-x)*2 + y)*2%bl;
                uint8_t g = (i + x + (4-y)*2)*2%bl;
                uint8_t b = (i - (8 - (x + y)*2))*2%bl;
#else
                uint8_t r = 255;
                uint8_t g = 255;
                uint8_t b = 255;
#endif
                if  (i%(bl*2) >= bl) {
                    r = r * (bl-i%bl)/bl;
                    g = g * (bl-i%bl)/bl;
                    b = b * (bl-i%bl)/bl;
                } else {
                    r = r * (i%bl)/bl;
                    g = g * (i%bl)/bl;
                    b = b * (i%bl)/bl;
                }
                setPixel(x, y, r, g, b);
                //setPixel(x, y, (i+x+y) & 0x3F, ((i+x+y) >> 6)&0x3F, ((i+y) >> 12)&0x3F);
            }
        }
        mraa_spi_transfer_buf(spi, (uint8_t*)buf, NULL, sizeof(buf));
        // usleep(50000);
    }
#endif
#if (1)
    for (i=0; ; i++) {
        for (y=0; y < LEDMATRIX_YMAX; y++) {
            for (x=0; x < LEDMATRIX_XMAX; x++) {
                uint8_t c;
                int xr = x + (i % GREETINGS_WIDTH);
                int br = BRIGHTNESS_LIMIT;
                c = Greetings[y][xr % GREETINGS_WIDTH];
                switch (c) {
                case 0: setPixel(x, y, 0, 0, 0);     break; // BLACK
                case 1: setPixel(x, y, br, br, br);  break; // WHITE
                case 2: setPixel(x, y, br, 0, 0);    break; // RED
                case 3: setPixel(x, y, 0, br, br);   break; // CYAN
                case 4: setPixel(x, y, br, 0, br);   break; // MAGENTA
                case 5: setPixel(x, y, 0 ,br, 0);    break; // YELLOW
                }
            }
        }
        mraa_spi_transfer_buf(spi, (uint8_t*)buf, NULL, sizeof(buf));
       usleep(100000);
    }
#endif
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
    int adr;

    if (x >= LEDMATRIX_X) {
        y = y + LEDMATRIX_Y;
        x = x - LEDMATRIX_X;
    }
    adr = y*LEDMATRIX_X + x;
    buf[adr].g = regulateWordBit(g);
    buf[adr].r = regulateWordBit(r);
    buf[adr].b = regulateWordBit(b);
}

static uint32_t regulateWordBit(uint8_t in)
{
    static const uint32_t BRIGHTNESS_TABLE[256] = {
        0x88888888, 0x8888888e, 0x888888e8, 0x888888ee, 
        0x88888e88, 0x88888e8e, 0x88888ee8, 0x88888eee, 
        0x8888e888, 0x8888e88e, 0x8888e8e8, 0x8888e8ee, 
        0x8888ee88, 0x8888ee8e, 0x8888eee8, 0x8888eeee, 
        0x888e8888, 0x888e888e, 0x888e88e8, 0x888e88ee, 
        0x888e8e88, 0x888e8e8e, 0x888e8ee8, 0x888e8eee, 
        0x888ee888, 0x888ee88e, 0x888ee8e8, 0x888ee8ee, 
        0x888eee88, 0x888eee8e, 0x888eeee8, 0x888eeeee, 
        0x88e88888, 0x88e8888e, 0x88e888e8, 0x88e888ee, 
        0x88e88e88, 0x88e88e8e, 0x88e88ee8, 0x88e88eee, 
        0x88e8e888, 0x88e8e88e, 0x88e8e8e8, 0x88e8e8ee, 
        0x88e8ee88, 0x88e8ee8e, 0x88e8eee8, 0x88e8eeee, 
        0x88ee8888, 0x88ee888e, 0x88ee88e8, 0x88ee88ee, 
        0x88ee8e88, 0x88ee8e8e, 0x88ee8ee8, 0x88ee8eee, 
        0x88eee888, 0x88eee88e, 0x88eee8e8, 0x88eee8ee, 
        0x88eeee88, 0x88eeee8e, 0x88eeeee8, 0x88eeeeee, 
        0x8e888888, 0x8e88888e, 0x8e8888e8, 0x8e8888ee, 
        0x8e888e88, 0x8e888e8e, 0x8e888ee8, 0x8e888eee, 
        0x8e88e888, 0x8e88e88e, 0x8e88e8e8, 0x8e88e8ee, 
        0x8e88ee88, 0x8e88ee8e, 0x8e88eee8, 0x8e88eeee, 
        0x8e8e8888, 0x8e8e888e, 0x8e8e88e8, 0x8e8e88ee, 
        0x8e8e8e88, 0x8e8e8e8e, 0x8e8e8ee8, 0x8e8e8eee, 
        0x8e8ee888, 0x8e8ee88e, 0x8e8ee8e8, 0x8e8ee8ee, 
        0x8e8eee88, 0x8e8eee8e, 0x8e8eeee8, 0x8e8eeeee, 
        0x8ee88888, 0x8ee8888e, 0x8ee888e8, 0x8ee888ee, 
        0x8ee88e88, 0x8ee88e8e, 0x8ee88ee8, 0x8ee88eee, 
        0x8ee8e888, 0x8ee8e88e, 0x8ee8e8e8, 0x8ee8e8ee, 
        0x8ee8ee88, 0x8ee8ee8e, 0x8ee8eee8, 0x8ee8eeee, 
        0x8eee8888, 0x8eee888e, 0x8eee88e8, 0x8eee88ee, 
        0x8eee8e88, 0x8eee8e8e, 0x8eee8ee8, 0x8eee8eee, 
        0x8eeee888, 0x8eeee88e, 0x8eeee8e8, 0x8eeee8ee, 
        0x8eeeee88, 0x8eeeee8e, 0x8eeeeee8, 0x8eeeeeee, 
        0xe8888888, 0xe888888e, 0xe88888e8, 0xe88888ee, 
        0xe8888e88, 0xe8888e8e, 0xe8888ee8, 0xe8888eee, 
        0xe888e888, 0xe888e88e, 0xe888e8e8, 0xe888e8ee, 
        0xe888ee88, 0xe888ee8e, 0xe888eee8, 0xe888eeee, 
        0xe88e8888, 0xe88e888e, 0xe88e88e8, 0xe88e88ee, 
        0xe88e8e88, 0xe88e8e8e, 0xe88e8ee8, 0xe88e8eee, 
        0xe88ee888, 0xe88ee88e, 0xe88ee8e8, 0xe88ee8ee, 
        0xe88eee88, 0xe88eee8e, 0xe88eeee8, 0xe88eeeee, 
        0xe8e88888, 0xe8e8888e, 0xe8e888e8, 0xe8e888ee, 
        0xe8e88e88, 0xe8e88e8e, 0xe8e88ee8, 0xe8e88eee, 
        0xe8e8e888, 0xe8e8e88e, 0xe8e8e8e8, 0xe8e8e8ee, 
        0xe8e8ee88, 0xe8e8ee8e, 0xe8e8eee8, 0xe8e8eeee, 
        0xe8ee8888, 0xe8ee888e, 0xe8ee88e8, 0xe8ee88ee, 
        0xe8ee8e88, 0xe8ee8e8e, 0xe8ee8ee8, 0xe8ee8eee, 
        0xe8eee888, 0xe8eee88e, 0xe8eee8e8, 0xe8eee8ee, 
        0xe8eeee88, 0xe8eeee8e, 0xe8eeeee8, 0xe8eeeeee, 
        0xee888888, 0xee88888e, 0xee8888e8, 0xee8888ee, 
        0xee888e88, 0xee888e8e, 0xee888ee8, 0xee888eee, 
        0xee88e888, 0xee88e88e, 0xee88e8e8, 0xee88e8ee, 
        0xee88ee88, 0xee88ee8e, 0xee88eee8, 0xee88eeee, 
        0xee8e8888, 0xee8e888e, 0xee8e88e8, 0xee8e88ee, 
        0xee8e8e88, 0xee8e8e8e, 0xee8e8ee8, 0xee8e8eee, 
        0xee8ee888, 0xee8ee88e, 0xee8ee8e8, 0xee8ee8ee, 
        0xee8eee88, 0xee8eee8e, 0xee8eeee8, 0xee8eeeee, 
        0xeee88888, 0xeee8888e, 0xeee888e8, 0xeee888ee, 
        0xeee88e88, 0xeee88e8e, 0xeee88ee8, 0xeee88eee, 
        0xeee8e888, 0xeee8e88e, 0xeee8e8e8, 0xeee8e8ee, 
        0xeee8ee88, 0xeee8ee8e, 0xeee8eee8, 0xeee8eeee, 
        0xeeee8888, 0xeeee888e, 0xeeee88e8, 0xeeee88ee, 
        0xeeee8e88, 0xeeee8e8e, 0xeeee8ee8, 0xeeee8eee, 
        0xeeeee888, 0xeeeee88e, 0xeeeee8e8, 0xeeeee8ee, 
        0xeeeeee88, 0xeeeeee8e, 0xeeeeeee8, 0xeeeeeeee,
    };
    if (in >= BRIGHTNESS_LIMIT) { // limitter
        in = BRIGHTNESS_LIMIT;
    }
    return BRIGHTNESS_TABLE[in];
}
