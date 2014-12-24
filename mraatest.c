#include <stdio.h>
#include <stdlib.h>
#include "mraa.h"

int main(int argc, const char *argv[])
{
  char *board_name = mraa_get_platform_name();
  printf("Board Name: %s\n", board_name);
  mraa_deinit();
  return MRAA_SUCCESS;
}
