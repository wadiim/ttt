#include "config.h"
#include "term.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void die(const char *msg)
{
	switch_to_normal_buffer();
	if (errno == 0)
	{
		fprintf(stderr, "%s: %s\n", PROGRAM_NAME, msg);
	}
	else
	{
		fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME,
				msg, strerror(errno));
	}
	exit(EXIT_FAILURE);
}
