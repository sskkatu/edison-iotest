#include <stdio.h>
#include <unistd.h>
#include "mraa.h"
#include "iotest.h"

#define GPIO_UART1_TX  35 // GP131
#define GPIO_UART1_RX  26 // GP130
#define GPIO_PORT GPIO_UART1_RX  // GPIO Port for Testing
#define SIGNAL_INVERSION_TIME 5

int gpioTest(void) 
{
    int i, val = 0;
     mraa_gpio_context gpio = mraa_gpio_init(GPIO_PORT);
     if (gpio == NULL) {
          fprintf(stderr, "Cannot open gpio port:%d\n", GPIO_PORT);
          exit(1);
     }
     mraa_gpio_dir(gpio, MRAA_GPIO_OUT);

     for (i=0, val = 0; i < 100; i++) {
          mraa_gpio_write(gpio, val = 1 - val);
          sleep(SIGNAL_INVERSION_TIME);
     }
     mraa_gpio_close(gpio);
     return 0;
}
