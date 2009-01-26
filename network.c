#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "network.h"
#include "main.h"
#include "connection.h"

extern struct aids_global_conf aids_conf;
/**
 * Method to gather network data to a structure.
 *
 * @param dev string name of network device which shall be sniffed.
 * @param traffic pointer to an initialized structure to which the data will be saved.
 * @param filter filter description
 *
 * \todo Modify network sniffer to gather in and out data separately.
 * \todo Add IP address recognition.
 */
void network_usage(const char *dev, struct network_traffic *traffic, const char *filter)
{
	pcap_t *handle;
	struct network_stats stats;
	struct pcap_pkthdr header;
	char *net; /* the network address */
	char errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32 netp;
	bpf_u_int32 maskp;
	struct bpf_program fp;
	struct in_addr addr;
	int ret;
	long time_cond;
	struct timeval t1, t2;

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
		logger(stderr, ERROR, "inet_itoa fail");
		return;
	}

	/* printf("NET: %s\n", net); */

	handle = pcap_open_live(dev, 99999, 1, 500, errbuf);
	if (handle == NULL)
	{
		logger(stderr, ERROR, "Couldn't open device %s: %s", dev, errbuf);
		return;
	}

	memset(&stats, 0, sizeof(struct network_stats));
	
	/*
	 * Setting the filter
	 */
	if(pcap_compile(handle, &fp, filter, 0, netp) == -1)
	{
		logger(stderr, ERROR, "[network.c] Error compiling the filter");
		exit(-1);
	}

	if(pcap_setfilter(handle, &fp) == -1)
	{
		logger(stderr, ERROR, "[network.c] Error setting the filter");
		exit(-1);
	}

	time_cond = 0;
	stats.length = 0;
	while (time_cond < 1000000)
	{
		gettimeofday(&t1, NULL);
		memset(&header, 0, sizeof(struct pcap_pkthdr));
		pcap_next(handle, &header);

		stats.length += header.caplen;
		gettimeofday(&t2, NULL);
		time_cond += (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;
	}

	pcap_close(handle);

	/*
	 * return kb/s
	 */
	traffic -> in = stats.length / 1024.0;
	traffic -> out = stats.length / 1024.0;
}

/**
 * A method for computing the statistical data of the accumulated network traffic data.
 *
 * @param network_stats The network statistics to be processed.
 * @param avg Initialized structure for math stats.
 */
void generate_traffic_stats(struct network_traffic network_stats[], struct average_stats *avg)
{
	double single_var;
	int i;

	for(i = 0; i < aids_conf.network_recent; i++)
	{
		avg -> average += network_stats[i].in;
	}
	avg -> average /= aids_conf.network_recent;

	for(i = 0; i < aids_conf.network_recent; i++)
	{
		single_var = (network_stats[i].in - avg -> average);
		single_var *= single_var;
		avg -> variance += single_var;
	}
	avg -> variance /= aids_conf.network_recent - 1;
	avg -> deviation = sqrt(avg -> variance);
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
	struct average_stats avg;
	int i;
	FILE *f;

	while (1)
	{
		f = fopen(aids_conf.network_global_data_filename, "a");
		if (f == NULL)
		{
			logger(stderr, ERROR, "[network.c] couldn't open file %s for appending", aids_conf.network_global_data_filename);
			exit(1);
		}
		for(i = 0; i < aids_conf.network_recent; i++)
		{
			network_usage("en1", &traffic, "dst 192.168.1.100 or src 192.168.1.100 or src 192.168.1.109 or dst 192.168.1.109");
			logger(stdout, DEBUG, "[network.c] in: %.3g, out %.3g\n", traffic.in, traffic.out);
			memcpy(&recent_traffic[i], &traffic, sizeof(struct network_traffic));
			sleep(aids_conf.network_sleep_time - 1);
		}
		generate_traffic_stats(recent_traffic, &avg);
		logger(f, DEBUG, "avg:%.3g, var:%.3g, dev:%.3g", avg.average, avg.variance, avg.deviation);
		logger(stdout, DEBUG, "[network.c] avg:%.3g, var:%.3g, dev:%.3g", avg.average, avg.variance, avg.deviation);
		if (avg.average < avg.deviation)
		{
			send_message("Warning! Network usage weird!");
			logger(stdout, WARN, "Warning, deviance is large");
		}
		fclose(f);
	}
}
