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

int i2cTest(void)
{
     mraa_i2c_context i2c = mraa_i2c_init(I2CPORT);
     if (i2c == NULL) {
          fprintf(stderr, "Cannot open i2c port:%d\n", I2CPORT);
          exit(1);

     }
     mraa_i2c_stop(i2c);
     return 0;
}
