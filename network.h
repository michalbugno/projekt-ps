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

char errbuf[PCAP_ERRBUF_SIZE];

long network_usage(const char *);
void got_packet(u_char *, const struct pcap_pkthdr *, const u_char *);

#endif
