#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdlib.h>

struct load_average
{
	unsigned int measures;
	double data[3];
};

int load_average(struct load_average *);

#endif
