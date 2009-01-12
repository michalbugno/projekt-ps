#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "main.h"
#include "system.h"
#include "network.h"

void data_analyzer(void) 
{
	/*struct load_average load;*/
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
		printf("%d\n", (int)buffer.st_size);
		fscanf(data_file, "%lf,%lf\n", &(traffic.in), &(traffic.out));
		fclose(data_file);
		printf("%lf,%lf\n", traffic.in, traffic.out);
	}
}
