#include <mbed.h>
#include <memory>
#include <uvisor-lib/uvisor-lib.h>
#include "box_secure_print.h"

/* setup secret const section (Flash) */
UVISOR_SECURE_CONST char g_password[] = "password";

/* setup secret data section (SRAM) */
typedef struct {
	RawSerial *serial;
	uint8_t serial_data[sizeof(RawSerial)];
} MyConfig;
UVISOR_SECURE_BSS MyConfig g_data;

/* create ACLs for secret data section */
static const UvBoxAclItem g_box_acl[] = {
	{&g_data, sizeof(g_data), 0}
};
/* configure secure box compartnent */
UVISOR_BOX_CONFIG(g_box_acl, UVISOR_BOX_STACK_SIZE);

/* simple print function for the serial interface */
void serial_printf(const char *c)
{
	int data;

	while ((data = (*c++)) != 0) {
		g_data.serial->putc(data);
	}
}

/* print a secret message */
void secure_print(void)
{
	/* initialize serial object on first use */
	if(!g_data.serial) {
		g_data.serial = ::new((void *) &g_data.serial_data) RawSerial(USBTX, USBRX);
	}

	/* print secure string */
	serial_printf("The password is: %s\n\r");
}
