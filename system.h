#ifndef SYSTEM_H
#define SYSTEM_H

#include <string.h>

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

/**
 * Structure holding the statistical data on the gathered processor load
 */
struct load_stats
{
	/**
	 * The computed averages for all the 3 measures
	 */
	double averages[3];

	/**
	 * The computed variances for all the 3 measures
	 */
	double variances[3];

};

int load_average(struct load_average *);
void aids_gather_processor_load(void);

#endif
