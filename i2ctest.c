#include <stdio.h>
#include <stdlib.h>
#include "mraa.h"
#include "iotest.h"

/**
 * DataSheets : 
 * 1) 3-Axis Gyro/ST Microelectronics L3G4200DST Microelectronics L3G4200D
 *     http://aitendo3.sakura.ne.jp/aitendo_data/product_img/sensor/multi/GY80/l3g4200d.pdf
 *
 * 2) 3-Axis Accelerometer/Analog Devices ADXL345
 *    http://aitendo3.sakura.ne.jp/aitendo_data/product_img/sensor/multi/GY80/ADXL345.pdfk
 *
 * 3) 3 Axis Magnetometer/Honeywell MC5883L
 *    http://aitendo3.sakura.ne.jp/aitendo_data/product_img/sensor/multi/GY80/MC5883L.pdf
 *
 * 4) Barometer + Thermometer/Bosch BMP085
 *    http://aitendo3.sakura.ne.jp/aitendo_data/product_img/sensor/multi/GY80/BMP085.pdf
 */
#define I2CADR_GYRO    0x69 // 3-Axis Gyro 
#define I2CADR_ACCE    0x53 // 3-Axis Accelerometer
#define I2CADR_MAGNET  0x1E // 3-Axis Magnetometer
#define I2CADR_BARO    0x77 // Barometor + Thermometer
#define I2CPORT        6    // Target I2C Port
#define HZ_0010K       10000
#define HZ_0400K      400000
#define HZ_3400K     3400000

static void enablePullup(int gpioport);

int i2cTest(void)
{
     uint8_t adr;
     int i, rcvbyte;
     mraa_i2c_context i2c;


     enablePullup(6);
     enablePullup(8);

     i2c = mraa_i2c_init(I2CPORT);
     if (i2c == NULL) {
          fprintf(stderr, "Cannot open i2c port:%d\n", I2CPORT);
          exit(1);
     }
     mraa_i2c_frequency(i2c, HZ_0010K); // No Effect as of now in mraa.
     adr = I2CADR_GYRO;
     //adr = I2CADR_MAGNET;
     //adr = I2CADR_ACCE;
     //adr = I2CADR_BARO;
     mraa_i2c_address(i2c, adr);
     i = 0;
     while (1) {
         rcvbyte = mraa_i2c_read_byte_data(i2c, i&0x0F);  // WHO_AM_I in ACCE
	 usleep(10000);
         printf("(%012d) adr=%02x, cmd=%02x, result=%02x\n", i++, adr, i&0x0FF, rcvbyte);
     }
     mraa_i2c_stop(i2c);
     return 0;
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
