#ifndef SYSTEM_H
#define SYSTEM_H

struct load_average
{
	unsigned int measures;
	double data[3];
};

int load_average(struct load_average *);
void aids_gather_processor_load(void);

#endif
