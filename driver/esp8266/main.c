
#include "uart.h"
#include "common.h"

int main(int argc, char **argv)
{
    uart1_init();
    atk_8266_test();
}
