#include "network.h"
#include "system.h"
#include <unistd.h>

int main(int argc, char **argv)
{
	pid_t pid;
	FILE *f;
	pid = fork();

	if(pid < 0) exit(1);
	if(pid > 1)
	{
		printf("%d\n", pid);
		exit(0);
	}
	int i, r;
	struct network_traffic traffic;
	struct load_average load_avg;

	for (i = 0; i < 50; i++)
	{
		network_usage("en1", &traffic);
		r = load_average(&load_avg);
		if (r != -1)
			printf("Usage: %.4g %.4g %.4g\n", load_avg.data[0], load_avg.data[1], load_avg.data[2]);
		printf("Average network usage: %.4g kb/s in, %.4g kb/s out\n", traffic.in, traffic.out);
	}
	return 0;
}
