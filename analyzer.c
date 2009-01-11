#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "main.h"
#include "system.h"
#include "network.h"

void data_analyzer(void) 
{
	/*struct load_average load;*/
	struct network_traffic traffic;
	sleep(5);
	FILE* data_file;

	printf("elo\n");

	if ((data_file = fopen("data/current_traffic.dat", "r")) == NULL )
	{
		perror("file opening problem");
		exit(-1);
	}

	fscanf(data_file, "%f,%f\n", (float *)&(traffic.in), (float *)&(traffic.out));
	fclose(data_file);
	printf("%g,%g\n", traffic.in, traffic.out);


}
