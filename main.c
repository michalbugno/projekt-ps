#include <unistd.h>
#include <signal.h>
#include <libconfig.h>
#include <pthread.h>
#include <stdlib.h>
#include "network.h"
#include "main.h"
#include "system.h"

/** \mainpage AIDS - Anomaly IDS
 *
 * \section intro What does it do?
 * AIDS is anomaly Intrusion Detection System. It runs as a daemon which gathers
 * data and writes it to files.
 *
 * \section authors Authors
 * Michal Bugno
 * Antek Piechnik
 *
 *
 * \todo Roughly: analize gathered data.
 */

/**
 * Kills process with pid written to file.
 *
 * @param filename name of the file where the pid is written
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

/**
 * Configuration is read from config file and right methods dispatched
 * based on arguments given to file.
 */
int dispatch_from_args(int argc, char **argv)
{
	int c;
	const char *error;

	if ((error = read_conf("aids.cfg")) != NULL)
	{
		printf("Error reading config file: %s\n", error);
		return 2;
	}

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
					eradicate(aids_conf.pid_file);
					break;

				case '?':
					printf("Usage: %s [-k]\n\n-k kill working aids server", argv[0]);
					break;
			}
		}
		return c;
	}

}

/**
 * This method fork's child process as daemon to continue the work and exits. Also pid
 * file with child's pid is created. If pid file already exists, method prints warning
 * and exits doing nothing.
 */
void do_run(void)
{
	pid_t pid;
	pthread_t threads[2];
	FILE *f;
	int rc;

	/*
	 * check if PID_FILE exists, if yes, then the server is already running
	 */
	f = fopen(aids_conf.pid_file, "r");
	if (f != NULL)
	{
		/*
		 * print info and don't do anything
		 */
		printf("Pid file exists! Not starting server... (remove pid file %s if the server is not running)\n", aids_conf.pid_file);
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
			f = fopen(aids_conf.pid_file, "w");
			fprintf(f, "%d\n", pid);
			fclose(f);
			exit(0);
		} else
		{
			/*
			 * child process -- do the stuff
			 */
			rc = pthread_create(&threads[0], NULL, (void *)aids_gather_network, NULL);
			if (rc != 0)
			{
				printf("Couldn't create thread, exiting (%d)\n", rc);
				exit(-1);
			}
			rc = pthread_create(&threads[1], NULL, (void *)aids_gather_processor_load, NULL);
			if (rc != 0)
			{
				printf("Couldn't create thread, exiting (%d)\n", rc);
				exit(-1);
			}
			pthread_join(threads[0], NULL);
			pthread_join(threads[1], NULL);
		}
	}
}

/**
 * Using libconfig a config file is parsed and options are extracted to global
 * struct aids_conf. If config file is malformed a description of the error
 * is returned.
 *
 * @param filename name of config file
 * @returns description of error in case of failure or NULL if everything goes ok
 */
const char *read_conf(const char *filename)
{
	int r;
	config_t cfg;
	const char *pid_file;
	int network_timeout, processor_timeout;

	r = config_read_file(&cfg, filename);

	if (r == CONFIG_TRUE)
	{
		pid_file = config_lookup_string(&cfg, "pid_file");
		if (pid_file == NULL)
			pid_file = "aids.pid";
		aids_conf.pid_file = malloc(sizeof(char) * strlen(pid_file));
		strcpy(aids_conf.pid_file, pid_file);

		network_timeout = config_lookup_int(&cfg, "timeouts:network");
		if (network_timeout <= 0) network_timeout = 10;
		aids_conf.network_timeout = network_timeout;

		processor_timeout = config_lookup_int(&cfg, "timeouts:processor");
		if (processor_timeout <= 0) processor_timeout = 10;
		aids_conf.processor_timeout = processor_timeout;

		return NULL;
	} else
		return config_error_text(&cfg);
}
