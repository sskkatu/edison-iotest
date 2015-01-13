#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mraa.h"
#include "iotest.h"


#define UART_PORT 1

void uartGpsTest(void) 
{
    mraa_uart_context uart;
    
    uart = mraa_uart_init(UART_PORT);
    
    if (uart == NULL) {
        fprintf(stderr, "Cannot open uart port:%d\n", UART_PORT);
    }
}
