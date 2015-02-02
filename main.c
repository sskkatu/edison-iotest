#include <stdio.h>
#include <stdlib.h>
#include "mraa.h"
#include "iotest.h"

int main(int argc, const char *argv[]) 
{
     mraa_init();
     char *board_name = mraa_get_platform_name();
     printf("Board Name: %s\n", board_name);
     mraa_set_priority(99); // HIGHEST PRIORITY
     //gpioTest();
     //i2cTest();
     //spiLCDTest();
     //spiWS1812BTest();
     //i2cOLEDTest();
     i2cOLED_aitendo_Test();
     mraa_deinit();
     exit(0);
}
