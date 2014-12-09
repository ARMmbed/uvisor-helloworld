#include <mbed.h>
#include "box_secure_print.h"

int main(void) {
    volatile int t;
    while(1) {
        for (t = 0; t < 2000000; t++);
        secure_print();
    }

    return 0;
}
