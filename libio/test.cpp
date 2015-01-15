#include <stdio.h>
#include "mraa.hpp"
#include "oled_ssd1306.hpp"
#include "accelerometer.hpp"
using namespace putmode;
using namespace std;

static void pinModeSet(int gpioport);
int main(int argc, char *argv[]) {
    //   Accelerometer acc(6, 0x1d,  26);
    Accelerometer acc;
    Oled oled;
    Accelerometer::DataFormat dataformat;
    printf("Accelerometer Device ID:%02x\n", acc.getDeviceId());
    
    acc.setOffset(-4, 0, 0);
    acc.setTapmg(1000);  // tap = 100mg

    dataformat = acc.getDataFormat();
    dataformat.selfTest = 0;
    dataformat.fullRes = 1;
    acc.setDataFormat(dataformat);
    printf("DataFormat:%02x\n", acc.getDataFormat().c);

    acc.setOutputDataRate(Accelerometer::RATE_0050_00);
    acc.startMeasure();
    for (;;) {
        int16_t x, y, z;
        acc.getXYZData(x, y, z);
	//printf("x=%3d, y=%3d, z=%3d\n", x, y, z);
        oled.clearScreen();
        oled.putTextFormat(0, 0, "x=%3d, y=%3d, z=%3d", x, y, z);
        oled.update();
    }
    exit(0);
}

void interrupt(void *arg) {
    
}

static void pinModeSet(int gpioport)
{
     mraa_gpio_context gpio = mraa_gpio_init(gpioport);
     if (gpio == NULL) {
          fprintf(stderr, "Cannot open GPIO port:%d\n", gpioport);
     }
     mraa_gpio_mode(gpio, MRAA_GPIO_PULLUP);
     mraa_gpio_close(gpio);
}
