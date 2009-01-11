#ifndef MAIN_H
#define MAIN_H

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
	 * Number of seconds every each the network data will be gathered
	 */
	int processor_timeout;

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

int eradicate(const char *);
int dispatch_from_args(int, char **);
void do_run(void);
const char *read_conf(const char *);

#endif
