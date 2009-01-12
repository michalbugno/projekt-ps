#include <stdio.h>
#include <unistd.h>
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
	int i,j;
	FILE* data_file;

	while (1)
	{
		data_file = fopen(aids_conf.processor_recent_data_filename, "w");
		if (data_file == NULL)
		{
			perror("[system.c] Couldn't open file for writing");
			pthread_exit(NULL);
		}
		fclose(data_file);
		for(i = 0; i < aids_conf.processor_recent; i += 1)
		{
			load_average(&load);
			data_file = fopen(aids_conf.processor_recent_data_filename, "a");
			if (data_file == NULL)
			{
				perror("[system.c] Couldn't open file for writing");
				pthread_exit(NULL);
			}
			fprintf(data_file,"%d\n", load.measures);
			for(j = 0; j < load.measures-1 ; j+=1)
			{
				fprintf(data_file,"%lf,", load.data[j]);
			}
			fprintf(data_file, "%lf\n", load.data[load.measures-1]);
			fclose(data_file);
			sleep(aids_conf.processor_sleep_time);
		}
		
	}
}
