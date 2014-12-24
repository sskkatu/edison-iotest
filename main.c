#include <stdio.h>
#include <stdlib.h>
#include "mraa.h"
#include "iotest.h"

int main(int argc, const char *argv[]) 
{
     mraa_init();
     char *board_name = mraa_get_platform_name();
     printf("Board Name: %s\n", board_name);
     gpioTest();
     mraa_deinit();
     return MRAA_SUCCESS;
}