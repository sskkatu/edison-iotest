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
 *    http://aitendo3.sakura.ne.jp/aitendo_data/product_img/sensor/multi/GY80/ADXL345.pdf
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
#define I2CADR_TOUCH   0x78
#define I2CPORT        1    // Target I2C Port
#define HZ_0010K       10000
#define HZ_0400K      400000
#define HZ_3400K     3400000

static void enablePullup(int gpioport);

int i2cTest(void)
{
     uint8_t adr;
     int i, rcvbyte;
     mraa_i2c_context i2c;
     uint8_t config[3];

     enablePullup(7);
     enablePullup(19);

     i2c = mraa_i2c_init(I2CPORT);
     if (i2c == NULL) {
          fprintf(stderr, "Cannot open i2c port:%d\n", I2CPORT);
          exit(1);
     }
     mraa_i2c_frequency(i2c, HZ_0400K); // No Effect as of now in mraa.
     adr = I2CADR_TOUCH;
     //adr = I2CADR_GYRO;
     //adr = I2CADR_MAGNET;
     //adr = I2CADR_ACCE;
     //adr = I2CADR_BARO;
     if (mraa_i2c_address(i2c, adr) != MRAA_SUCCESS) {
          fprintf(stderr, "Invalid address:%02x\n", adr);
          exit(1);
     }
     config[0] = 0x00;
     config[1] = 0xF5;
     config[2] = 0xFC;
     mraa_i2c_write(i2c, config, 3);
     i = 0x02;
     while (1) {
         rcvbyte = mraa_i2c_read_byte_data(i2c, i);
         printf("%02x\n", rcvbyte);
         usleep(10000);
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
     mraa_gpio_mode(gpio, MRAA_GPIO_STRONG);
     mraa_gpio_close(gpio);
}
