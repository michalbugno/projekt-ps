#ifndef NETWORK_H
#define NETWORK_H

#include <pcap.h>
#include <string.h>

struct network_stats
{
	struct timeval start;
	struct timeval finish;
	unsigned int length;
};

struct network_traffic
{
	double in;
	double out;
};

char errbuf[PCAP_ERRBUF_SIZE];

void network_usage(const char *, struct network_traffic *);
void got_packet(u_char *, const struct pcap_pkthdr *, const u_char *);
void aids_gather_network(void);

#endif
