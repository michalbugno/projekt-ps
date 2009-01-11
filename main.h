#ifndef MAIN_H
#define MAIN_H

struct aids_global_conf
{
	int network_timeout;
	int processor_timeout;
	char *pid_file;
};

struct aids_global_conf aids_conf;

int eradicate(const char *);
int dispatch_from_args(int, char **);
void do_run(void);
const char *read_conf(const char *);

#endif
