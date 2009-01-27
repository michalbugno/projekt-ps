#include <unistd.h>
#include <signal.h>
#include <libconfig.h>
#include <pthread.h>
#include <stdlib.h>
#include "network.h"
#include "connection.h"
#include "main.h"
#include "system.h"

/** \mainpage AIDS - Anomaly IDS
 *
 * \section intro What does it do?
 * AIDS is anomaly Intrusion Detection System. It runs as a daemon which gathers
 * data and writes it to files.
 *
 * Main flow:
 * <ol>
 * <li> Run daemon</li>
 * <li> while deamon is alive</li>
 * <ol>
 * <li> gather network flow</li>
 * <li> gather processor load</li>
 * <li> analyze (and send warnings if something is suspicious)</li>
 * </ol>
 * </ol>
 *
 * <p>Analysis</p>
 * Based on recent results check the standard deviation and compare it to
 * average. If it exceeds the average send warning.
 *
 * Based on last n average results check whether there is growing tendency
 * and if true, send warning.
 *
 * \author Michal Bugno Antek Piechnik
 *
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

	logger(stderr, INFO, "Killing server");
	file = fopen(filename, "r");
	if (file == NULL)
	{
		logger(stderr, ERROR, "Couldn't open file %s for reading", filename);
		return -1;
	}

	fscanf(file, "%d", &pid);
	if (kill((pid_t)pid, SIGINT) != 0)
	{
		logger(stderr, ERROR, "Couldn't kill process %d\n", pid);
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
		logger(stderr, ERROR, "Error reading config file: %s", error);
		return 2;
	}

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
	FILE *f;
	int rc, i;

	logger(stderr, INFO, "Starting aids");

	/*
	 * check if PID_FILE exists, if yes, then the server is already running
	 */
	f = fopen(aids_conf.pid_file, "r");
	if (f != NULL)
	{
		/*
		 * print info and don't do anything
		 */
		logger(stderr, ERROR, "Pid file exists! Not starting server. (remove pid file %s if the server is not running)", aids_conf.pid_file);
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

			signal(SIGINT, sigint_handler);

			rc = pthread_create(&aids_threads[THREAD_NETWORK], NULL, (void *)aids_gather_network, NULL);
			if (rc != 0)
			{
				logger(stderr, ERROR, "Couldn't create thread, exiting (%d)", rc);
				exit(-1);
			}
			rc = pthread_create(&aids_threads[THREAD_LOAD], NULL, (void *)aids_gather_processor_load, NULL);
			if (rc != 0)
			{
				logger(stderr, ERROR, "Couldn't create thread, exiting (%d)", rc);
				exit(-1);
			}

			for (i = 0; i < THREAD_NUM; i++)
				pthread_join(aids_threads[i], NULL);
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
	int network_sleep_time, processor_sleep_time, network_recent, processor_recent;

	r = config_read_file(&cfg, filename);

	if (r == CONFIG_TRUE)
	{
		aids_conf.pid_file = read_in_string(&cfg, "pid_file", "aids.pid");
		aids_conf.network_global_data_filename = read_in_string(&cfg,  "network:global_data_filename", "data/network.dat");
		aids_conf.processor_global_data_filename = read_in_string(&cfg, "processor:global_data_filename", "data/processor.dat");
		aids_conf.jabber_id = read_in_string(&cfg, "jabber_id", "anomalyids@gmail.com");
		aids_conf.jabber_pass = read_in_string(&cfg, "jabber_pass", "systemowe");
		aids_conf.jabber_receiver_id = read_in_string(&cfg, "jabber_receiver_id", "michal.bugno@gmail.com");

		network_sleep_time = config_lookup_int(&cfg, "network:sleep_time");
		if (network_sleep_time <= 0) network_sleep_time = 10;
		aids_conf.network_sleep_time = network_sleep_time;

		processor_sleep_time = config_lookup_int(&cfg, "processor:sleep_time");
		if (processor_sleep_time <= 0) processor_sleep_time = 10;
		aids_conf.processor_sleep_time = processor_sleep_time;

		network_recent = config_lookup_int(&cfg, "network:recent_data");
		if (network_recent <= 0) network_recent = 100;
		aids_conf.network_recent = network_recent;

		processor_recent = config_lookup_int(&cfg, "processor:recent_data");
		if (processor_recent <= 0) processor_recent = 100;
		aids_conf.processor_recent = processor_recent;

		return NULL;
	} else
		return config_error_text(&cfg);
}

/**
 * Reads config value from cfg present at path and returns it. If no path found
 * default_value is used.
 *
 * @param cfg config file structure
 * @param path key to look for
 * @param default_value string returned if no value found
 * @returns value of key or default_value if no path found
 */
char *read_in_string(config_t *cfg, char *path, char *default_value)
{

	const char *value;
	char *dest;
	
	value = config_lookup_string(cfg, path);
	if (value == NULL)
		value = default_value;
	dest = malloc(sizeof(char) * strlen(value));
	strcpy(dest, value);
	return dest;
}

/**
 * Catches SIGINT signal, exits all threads and quits AIDS.
 *
 * @param sig signal number
 */
void sigint_handler(int sig)
{
	int i;

	for (i = 0; i < THREAD_NUM; i++)
		if (aids_threads[i] != NULL) pthread_kill(aids_threads[i], SIGINT);
	signal(SIGINT, SIG_DFL);
	kill(getpid(), SIGINT);
}
