#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include "system.h"
#include "main.h"

extern struct aids_global_conf aids_conf;

/**
 * Gathers processor load average like the one from uptime shell method.
 *
 * @param avg pointer to initialized structure where the output shall be saved.
 * @returns 0 if everything ok, -1 if getting load failed.
 */
int load_average(struct load_average *avg)
{
	int r, i;
	double load_avg[3];

	r = getloadavg(load_avg, 3);
	if (r == -1)
	{
		return -1;
	} else
	{
		avg -> measures = r;
		for (i = 0; i < r; i++)
			avg -> data[i] = load_avg[i];
		return 0;
	}
}

/**
 * Infinite loop gathering network data. This method shall be ran threaded.
 */
void aids_gather_processor_load(void)
{
	struct load_average load;
	struct load_average recent_load[aids_conf.processor_recent];
	struct load_stats avg;
	int i;
	FILE* f;

	while (1)
	{
		f = fopen(aids_conf.processor_global_data_filename, "a");
		if (f == NULL)
		{
			perror("[system.c] Couldn't open file!");
			pthread_exit(NULL);
		}
		for(i = 0; i < aids_conf.processor_recent; i += 1)
		{
			load_average(&load);
			memcpy(&recent_load[i], &load, sizeof(struct load_average));
			sleep(aids_conf.processor_sleep_time);
		}
		fclose(f);
		
	}
}
