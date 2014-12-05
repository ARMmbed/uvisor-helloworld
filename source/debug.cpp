#include <ctype.h>
#include <stdarg.h>
#include "mbed.h"
#include "debug.h"

#ifndef CHANNEL_DEBUG
#define CHANNEL_DEBUG 0
#endif/*CHANNEL_DEBUG*/

void dputc(char data)
{
	if(	(ITM->TCR & ITM_TCR_ITMENA_Msk) &&
		(ITM->TER & (1<<CHANNEL_DEBUG))
		)
	{
		while (ITM->PORT[CHANNEL_DEBUG].u32 == 0);
		/* TX debug character */
		ITM->PORT[CHANNEL_DEBUG].u8 = data;
	}
}

int dprintf(const char* format, ...)
{
	int res;
	char *c, buffer[MAX_STRING_SIZE];
	va_list arg;

	va_start(arg, format);
	if((res=vsnprintf(buffer, sizeof(buffer), format, arg))>0)
	{
		if(res>(int)sizeof(buffer))
			res=sizeof(buffer);

		c = buffer;
		while(res--)
			dputc(*c++);
	}
	va_end(arg);

	return res;
}
