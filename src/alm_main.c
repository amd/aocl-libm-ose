#include <stdio.h>
#include <unistd.h>

#include "version.build.h"

#define NORETURN __attribute__((noreturn))

const char service_interp[] __attribute__((section(".interp"))) = "/lib64/ld-linux-x86-64.so.2";

void 
NORETURN
alm_main(void)
{
	printf("%s\n", alm_get_build());
	_exit(0);
}

