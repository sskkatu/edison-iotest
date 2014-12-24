#include <stdio.h>
#include <unistd.h>
#include "mraa.h"
#include "iotest.h"

#define GPIO_UART1_TX  35 // GP131
#define GPIO_UART1_RX  26 // GP130

#define TEST_GPIO_PORT GPIO_UART1_TX

int gpioTest(void) 
{
     int i, j, val = 0;
     mraa_gpio_context gpio = mraa_gpio_init(TEST_GPIO_PORT);
     if (gpio == NULL) {
          fprintf(stderr, "cannout open gpio port:%d\n", TEST_GPIO_PORT);
          exit(1);
     }
     mraa_gpio_dir(gpio, MRAA_GPIO_OUT);

//     for (val = 0, i = 0; i < 1000; i ++) {
     for (i=0; i < 10000; i++) {
          mraa_gpio_write(gpio, val = 1 - val);
          usleep(1000000);

/*
          for (j=0; j < 100; j++) {
               duty(gpio, i % 100);
          }
*/
     }
     mraa_gpio_close(gpio);
}

void duty(mraa_gpio_context gpio, int duty) 
{
     const int BASE = 100;
     int during_on = BASE * duty / 100;
     mraa_gpio_write(gpio, 1);
     usleep(during_on);
     mraa_gpio_write(gpio, 0);
     usleep(BASE - during_on);
}

          
