#include <mbed.h>
#include "box_secure_print.h"

/* enable uvisor */
UVISOR_SET_MODE(2);

int main(void) {
    volatile int t;
    while(1) {
        for (t = 0; t < 2000000; t++);
        secure_print();
    }

    return 0;
}
