#include "common.h"

void sys_info(const char *msg)
{
	fprintf(stderr, "warning: %s\n", msg);
}

void sys_err(const char *msg)
{
	fprintf(stderr, "warning: ");
	perror(msg);
}

void sys_exit(const char *msg)
{
	fprintf(stderr, "error: ");
	perror(msg);
	exit(-1);
}

void sys_debug(const char *msg)
{
	fprintf(stdout, "%s\n", msg);
}
