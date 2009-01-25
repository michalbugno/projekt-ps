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
		fprintf(stderr, "inet_itoa");
		return;
	}

	/* printf("NET: %s\n", net); */

	handle = pcap_open_live(dev, 99999, 1, 500, errbuf);
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
 * A method for counting the standard deviation of the accumulated network traffic data.
 *
 * @param network_stats The network statistics to be processed.
 */
double *calculate_math(struct network_traffic network_stats[])
{
	double single_var;
	double *ret;
	int i;

	ret = malloc(sizeof(double) * 2);
	ret[0] = ret[1] = 0.0;

	for(i = 0 ; i < aids_conf.network_recent; i+=1)
	{
		ret[0] += network_stats[i].in;
	}
	ret[0] /= aids_conf.network_recent;

	for(i = 0 ; i < aids_conf.network_recent ; i+=1)
	{
		single_var = (network_stats[i].in - ret[0]);
		single_var *= single_var;
		ret[1] += single_var;
	}
	ret[1] /= aids_conf.network_recent;
	ret[1] = sqrt(ret[1]);

	return ret;
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
	double *math;
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
			network_usage("en1", &traffic, "src 192.168.1.100");
			memcpy(&recent_traffic[i], &traffic, sizeof(struct network_traffic));
			sleep(aids_conf.network_sleep_time);
		}
		math = calculate_math(recent_traffic);

		fprintf(f, "avg: %.3g stdev: %.3g\n", math[0], math[1]);
		fprintf(stdout, "avg: %.3g stdev: %.3g\n", math[0], math[1]);
		fclose(f);
	}
}
