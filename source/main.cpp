#include "mbed/mbed.h"
#include "uvisor-lib/uvisor-lib.h"
#include "uvisor-sectest/uvisor-sectest.h"
#include "debug.h"

/* LED is instantiated locally to isolate peripheral
 * instantiations from libc */
void led_blinker(void)
{
    DigitalOut led(LED1);

    led = 1;
    wait(1.0);
    led = 0;
    wait(1.0);
}

int main(void) 
{
    dprintf("Hello World!\n");

    while(1) {
        led_blinker();
        dprintf("New result: 0x%08X\n", 42);
    }

    return 0;
}
