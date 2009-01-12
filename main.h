#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>

/**
 * Total number of threads in application.
 */
#define THREAD_NUM 3

/**
 * Number of network gather thread in global thread array.
 *
 * @see aids_threads
 */
#define THREAD_NETWORK 0

/**
 * Number of load gather thread in global thread array.
 *
 * @see aids_threads
 */
#define THREAD_LOAD 1

/**
 * Number of analyzer thread in global thread array.
 *
 * @see aids_threads
 */
#define THREAD_ANALYZER 2

/**
 * Structure holding global config.
 *
 * @see read_conf
 */
struct aids_global_conf
{
	/**
	 * Number of seconds every each the network data will be gathered
	 */
	int network_timeout;

	/**
	 * Number of seconds every each the processor load data will be gathered
	 */
	int processor_timeout;

	/**
	 * Number of network data nodes kept as 'recent'
	 */
	int recent_network;
	
	/**
	 * Number of processor load data nodes kept as 'recent'
	 */
	int recent_processor;

	/**
	 * Name of the file containing pid of spawned process.
	 *
	 * @see do_run
	 * @see eradicate
	 */
	char *pid_file;
};

/**
 * @see read_conf
 */
struct aids_global_conf aids_conf;

/**
 * Global array containing threads ran by AIDS.
 */
pthread_t aids_threads[THREAD_NUM];

int eradicate(const char *);
int dispatch_from_args(int, char **);
void do_run(void);
const char *read_conf(const char *);
void sigint_handler(int);

#endif
