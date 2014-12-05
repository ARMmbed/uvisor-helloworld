#include <mbed/mbed.h>
#include <uvisor-secprint/uvisor-secprint.h>

int main(void) 
{
    while(1) {
        secprint();
    }

    return 0;
}
