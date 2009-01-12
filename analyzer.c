#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "main.h"
#include "system.h"
#include "network.h"

/**
 * Method to analyze data gathered from the previously saved files
 *
 * \todo Add some analysis.
 */
void data_analyzer(void) 
{
	struct load_average load;
	struct network_traffic traffic;
	struct stat buffer;
	int status;
	FILE* data_file;

	while(1) 
	{
		sleep(5);
		if ((data_file = fopen("data/current_traffic.dat", "r")) == NULL )
		{
			perror("file opening problem");
			exit(-1);
		}
		
		status = stat("data/current_traffic.dat", &buffer);
		/* printf("%d\n", (int)buffer.st_size); */
		if( buffer.st_size > 0 ) 
		{
			fscanf(data_file, "%lf,%lf\n", &(traffic.in), &(traffic.out));
			/* printf("%lf,%lf\n", traffic.in, traffic.out); */
		}
		fclose(data_file);

		if ((data_file = fopen("data/current_load.dat", "r")) == NULL )
		{
			perror("file opening problem");
			exit(-1);
		}
		
		status = stat("data/current_load.dat", &buffer);
		/* printf("%d\n", (int)buffer.st_size); */
		if( buffer.st_size > 0 )
		{
			int measures_count = 0;
			int j;

			fscanf(data_file, "%d\n",&measures_count);
			for(j = 0; j < measures_count-1 ; j+=1)
			{
				fscanf(data_file,"%lf,", &(load.data[j]));
				/* printf("%lf", load.data[j]); */
			}
			fscanf(data_file, "%lf\n", &(load.data[measures_count-1]));
			/* printf("%lf\n", load.data[measures_count-1]); */
		}
		fclose(data_file);
	}
}
