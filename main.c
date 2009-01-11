#include <unistd.h>
#include <signal.h>
#include "network.h"
#include "main.h"
#include "system.h"

/*
 * kills process with pid written to the file
 */
int eradicate(const char *filename)
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
	if (kill((pid_t)pid, SIGKILL) != 0)
	{
		printf("Couldn't kill process %d\n", pid);
		return -1;
	}
	fclose(file);
	unlink(filename);
	return 1;
}

int main(int argc, char **argv)
{
	dispatch_from_args(argc, argv);
	return 0;
}

/*
 * using getopt we dispatch the right code
 */
int dispatch_from_args(int argc, char **argv)
{
	int c;

	if (argc == 1)
	{
		printf("Starting aids...\n");
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
					eradicate(PID_FILE);
					break;

				case '?':
					printf("Usage: %s [-k]\n\n-k kill working aids server", argv[0]);
					break;
			}
		}
		return c;
	}

}

/*
 * function which is executed when simply starting aids
 */
void do_run(void)
{
	pid_t pid;
	struct network_traffic traffic;
	struct load_average load_avg;
	FILE *f;

	/*
	 * check if PID_FILE exists, if yes, then the server is already running
	 */
	f = fopen(PID_FILE, "r");
	if (f != NULL)
	{
		/*
		 * print info and don't do anything
		 */
		printf("Pid file exists! Not starting server... (remove pid file %s if the server is not running)\n", PID_FILE);
		fclose(f);
		exit(0);
	} else
	{
		pid = fork();
		/*
		 * fork error
		 */
		if (pid < 0) exit(1);

		/*
		 * parent process -- write pid file and exit
		 */
		if (pid > 0)
		{
			fclose(f);
			f = fopen(PID_FILE, "w");
			fprintf(f, "%d\n", pid);
			fclose(f);
			exit(0);
		} else
		{
			/*
			 * child process -- do the stuff
			 */
			read_conf(&aids_conf);
			network_usage("en1", &traffic);
			load_average(&load_avg);
		}
	}
}

int read_conf(struct aids_global_conf *conf)
{
	return 1;
}
