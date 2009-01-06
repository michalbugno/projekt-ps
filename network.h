#ifndef NETWORK_H
#define NETWORK_H

#include <pcap.h>

char errbuf[PCAP_ERRBUF_SIZE];

long network_usage(const char *);
void got_packet(u_char *, const struct pcap_pkthdr *, const u_char *);

#endif
