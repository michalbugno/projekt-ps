#ifndef NETWORK_H
#define NETWORK_H

#include <pcap.h>
#include <string.h>

/**
 * Structure holding packet info gathered by network filter.
 */
struct network_stats
{
	/**
	 * Time when the gathering started.
	 */
	struct timeval start;

	/**
	 * Time when the gathering finished.
	 */
	struct timeval finish;

	/**
	 * Number of bytes gathered.
	 */
	unsigned int length;
};

/**
 * Structure returned by gathering method.
 */
struct network_traffic
{
	/**
	 * Incoming network usage (kb/s).
	 */
	double in;

	/**
	 * Outgoing network usage (kb/s).
	 */
	double out;
};

/**
 * Structure for containing the average statistics
 */
struct average_stats
{
	/**
	 * Average network bandwith usage
	 */
	double average;
	
	/**
	 * The variance attained
	 */
	double variance;

	/**
	 * The standard deviation, being the root square of the variance
	 */
	double deviation;
};

struct average_stats network_averages;

char errbuf[PCAP_ERRBUF_SIZE];

void network_usage(const char *, struct network_traffic *, const char *);
void got_packet(u_char *, const struct pcap_pkthdr *, const u_char *);
void aids_gather_network(void);

#endif
