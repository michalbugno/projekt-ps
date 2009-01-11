#ifndef MAIN_H
#define MAIN_H

#define PID_FILE "aids.pid"

struct aids_global_conf
{
	int network_timeout;
	int load_timeout;
};

struct aids_global_conf aids_conf;

int eradicate(const char *);
int dispatch_from_args(int, char **);
void do_run(void);
int read_conf(struct aids_global_conf *);

#endif
