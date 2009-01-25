#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "network.h"
#include "main.h"

extern struct aids_global_conf aids_conf;
/**
 * Method to gather network data to a structure.
 *
 * @param dev string name of network device which shall be sniffed.
 * @param traffic pointer to an initialized structure to which the data will be saved.
 * @param filter filter description
 *
 * \todo Modify network sniffer to gather in and out data separately.
 * \todo Modify so that it doesn't depend on number of packets received but works for say 1sec
 * \todo Add IP address recognition.
 */
void network_usage(const char *dev, struct network_traffic *traffic, const char *filter)
{
	pcap_t *handle;
	struct network_stats stats;
	struct pcap_pkthdr header_start, header_finish;
	char *net; /* the network address */
	char errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32 netp;
	bpf_u_int32 maskp;
	struct bpf_program fp;
	struct in_addr addr;
	int ret;
	double seconds;

	seconds = 0.0;
	ret = pcap_lookupnet(dev, &netp, &maskp, errbuf);

	if(ret == -1)
	{
		fprintf(stderr, "%s\n", errbuf);
		return;
	}

	addr.s_addr = netp;
	net = inet_ntoa(addr);

	if(net == NULL)
	{
		fprintf(stderr, "inet_itoa");
		return;
	}

	/* printf("NET: %s\n", net); */

	handle = pcap_open_live(dev, 99999, 1, 1000, errbuf);
	if (handle == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return;
	}

	memset(&stats, 0, sizeof(struct network_stats));
	
	/*
	 * Setting the filter
	 */
	if(pcap_compile(handle, &fp, filter, 0, netp) == -1)
	{
		fprintf(stderr, "[network.c] Error compiling the filter");
		exit(-1);
	}

	if(pcap_setfilter(handle, &fp) == -1)
	{
		fprintf(stderr, "[network.c] Error setting the filter");
		exit(-1);
	}

	/*
	 * time and size of first received packet
	 */
	pcap_next(handle, &header_start);
	stats.start = header_start.ts;
	stats.length += header_start.len;

	pcap_loop(handle, 98, got_packet, (u_char *)&stats);

	/*
	 * time and size of last received packet
	 */
	pcap_next(handle, &header_finish);
	stats.finish = header_finish.ts;
	stats.length += header_finish.len;

	seconds = (stats.finish.tv_sec - stats.start.tv_sec) * 1000000 + (stats.finish.tv_usec - stats.start.tv_usec);
	seconds /= 1000000;
	pcap_close(handle);

	/*
	 * return kb/s
	 */
	traffic -> in = stats.length / seconds / 1000.0;
	traffic -> out = stats.length / seconds / 1000.0;
}

/**
 * Callback used by pcap when receiving a single packet.
 *
 * @param args argument registered in pcap_loop
 * @param header structure holding packet header (most important info)
 * @param packet structure with all the info about packet
 */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	struct network_stats *stats;

	stats = (struct network_stats *)args;
	stats -> length += header -> len;
}

/**
 * A method for counting the standard deviation of the accumulated network traffic data.
 *
 * @param network_stats The network statistics to be processed.
 */
double standard_traffic_deviation(struct network_traffic network_stats[])
{
	double average = 0;
	double single_var;
	double variance = 0;
	int i;

	for(i = 0 ; i < aids_conf.network_recent; i+=1)
	{
		average += network_stats[i].in;
	}
	average /= aids_conf.network_recent;

	for(i = 0 ; i < aids_conf.network_recent ; i+=1)
	{
		single_var = (network_stats[i].in - average);
		single_var *= single_var;
		variance += single_var;
	}
	variance /= aids_conf.network_recent;

	return sqrt(variance);
}

/**
 * Used to gather network data every defined number of seconds.
 *
 * @see do_run
 */
void aids_gather_network(void)
{
	struct network_traffic traffic;
	struct network_traffic recent_traffic[aids_conf.network_recent];
	int i;
	double stdev;
	FILE *f;

	while (1)
	{
		f = fopen(aids_conf.network_global_data_filename, "a");
		if (f == NULL)
		{
			perror("[network.c] couldn't open file");
			exit(1);
		}
		for(i = 0; i < aids_conf.network_recent; i++)
		{
			network_usage("en1", &traffic, "src 192.168.1.26");
			memcpy(&recent_traffic[i], &traffic, sizeof(struct network_traffic));
			sleep(aids_conf.network_sleep_time);
		}
		stdev = standard_traffic_deviation(recent_traffic);
		fprintf(f, "%.3g\n", stdev);
		fprintf(stdout, "%.3g\n", stdev);
		fclose(f);
	}
}
