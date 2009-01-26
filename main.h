#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>
#include <libconfig.h>
#include "utils.h"

/**
 * Total number of threads in application.
 */
#define THREAD_NUM 2

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
 * Structure holding global config.
 *
 * @see read_conf
 */
struct aids_global_conf
{
	/**
	 * Number of seconds every each the network data will be gathered
	 */
	int network_sleep_time;

	/**
	 * Number of network data nodes kept as 'recent'
	 */
	int network_recent;

	/**
	 * Filename where recent network data is kept.
	 */
	char *network_recent_data_filename;

	/**
	 * Filename where global network data is kept.
	 */
	char *network_global_data_filename;
	
	/**
	 * Number of seconds every each the processor load data will be gathered
	 */
	int processor_sleep_time;

	/**
	 * Number of processor load data nodes kept as 'recent'
	 */
	int processor_recent;

	/**
	 * Filename where recent processor load data is kept.
	 */
	char *processor_recent_data_filename;

	/**
	 * Filename where global processor load data is kept.
	 */
	char *processor_global_data_filename;

	/**
	 * Name of the file containing pid of spawned process.
	 *
	 * @see do_run
	 * @see eradicate
	 */
	char *pid_file;
	char *jabber_id;
	char *jabber_pass;
	char *jabber_receiver_id;
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
char *read_in_string(config_t *, char *, char *);

#endif
