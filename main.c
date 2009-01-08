#include <unistd.h>
#include <signal.h>
#include "network.h"
#include "main.h"
#include "system.h"

int kill_master(const char *filename)
{
	FILE *file;
	int pid;

	file = fopen(filename, "r");
	if (file == NULL)
	{
		perror("Couldn't open file");
		return -1;
	}

	fscanf(file, "%d", &pid);
	if (eradicate(pid) != 0)
	{
		printf("Couldn't kill process %d\n", pid);
		return -1;
	}
	fclose(file);
	unlink(filename);
	return 1;
}

int eradicate(const int pid)
{
	return kill((pid_t)pid, SIGKILL) != 0;
}

int main(int argc, char **argv)
{
	dispatch_from_args(argc, argv);
	return 0;
}

int dispatch_from_args(int argc, char **argv)
{
	int c;

	if (argc == 1)
	{
		do_run();
		return 1;
	} else
	{
		while ((c = getopt(argc, argv, "k")) != -1)
		{
			switch (c)
			{
				case 'k':
					printf("Killing server...\n");
					kill_master("aids.pid");
					break;

				case '?':
					printf("Usage: %s [-k]\n", argv[0]);
					break;
			}
		}
		return c;
	}

}

void do_run(void)
{
	pid_t pid;
	struct network_traffic traffic;
	struct load_average load_avg;
	FILE *f;

	pid = fork();
	if (pid < 0) exit(1);
	if (pid > 1)
	{
		f = fopen("aids.pid", "r");
		if (f != NULL)
		{
			printf("Pid file exists! Not starting server...\n");
			fclose(f);
		} else
		{
			fclose(f);
			f = fopen("aids.pid", "w");
			fprintf(f, "%d\n", pid);
			fclose(f);
		}
		exit(0);
	}

	while (1)
	{
		network_usage("en1", &traffic);
		load_average(&load_avg);
	}
}
