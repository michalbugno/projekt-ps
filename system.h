#ifndef SYSTEM_H
#define SYSTEM_H

/**
 * Structure holding processor load -- like the one from uptime shell method.
 */
struct load_average
{
	/**
	 * Number of returned measures. Typically 3 -- load over period of last 1/5/15 minutes.
	 */
	unsigned int measures;

	/**
	 * The actual load values. Not necessarily 3 (see the measures member).
	 */
	double data[3];
};

int load_average(struct load_average *);
void aids_gather_processor_load(void);

#endif
