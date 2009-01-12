#include <unistd.h>
#include <pthread.h>
#include "network.h"
#include "main.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>


extern struct aids_global_conf aids_conf;

/**
 * Method to gather network data to a structure.
 *
 * @param dev string name of network device which shall be sniffed.
 * @param traffic pointer to an initialized structure to which the data will be saved.
 *
 * \todo Modify network sniffer to gather in and out data separately.
 * \todo Modify so that it doesn't depend on number of packets received but works for say 1sec
 * \todo Add IP address recognition.
 */
void network_usage(const char *dev, struct network_traffic *traffic)
{
	pcap_t *handle;
	struct network_stats stats;
	struct pcap_pkthdr header_start, header_finish;
	char *net; /* the network address */
	char errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32 netp;
	bpf_u_int32 maskp;

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

	printf("NET: %s\n", net);

	handle = pcap_open_live(dev, 99999, 1, 1000, errbuf);
	if (handle == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return;
	}

	memset(&stats, 0, sizeof(struct network_stats));

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
 * Used to gather network data every defined number of seconds.
 *
 * @see do_run
 */
void aids_gather_network(void)
{
	struct network_traffic traffic;
	int i;
	FILE* data_file;
	struct stat buffer;
	int status;


	while (1)
	{
		data_file = fopen(aids_conf.network_recent_data_filename, "w");
		if (data_file == NULL)
		{
			perror("[network.c] Couldn't open file for writing");
			pthread_exit(NULL);
		}
		fclose(data_file);
		
		for(i = 0; i < aids_conf.network_recent ; i += 1)
		{
			network_usage("en1", &traffic);
			data_file = fopen(aids_conf.network_recent_data_filename, "a");
			if (data_file == NULL)
			{
				perror("[network.c] Couldn't open file for writing");
				pthread_exit(NULL);
			}
			fprintf(data_file, "%lf,%lf\n", traffic.in, traffic.out);
			fclose(data_file);
			sleep(aids_conf.network_sleep_time);
		}

		struct network_traffic traffic_all[aids_conf.network_recent];

		if ((data_file = fopen(aids_conf.network_recent_data_filename, "r")) == NULL)
		{
			perror("[network.c] Couldn't open file for writing");
			exit(-1);
		}
		status = stat(aids_conf.network_recent_data_filename, &buffer);

		double in_average = 0;

		if(buffer.st_size > 0)
		{
			for ( i = 0 ; i < aids_conf.network_recent ; i += 1)
			{
				fscanf(data_file, "%lf,%lf\n", &(traffic_all[i].in), &(traffic_all[i].out));
				/* printf("%lf,%lf\n", traffic.in, traffic.out); */
				in_average += traffic_all[i].in;
			}
			in_average /= aids_conf.network_recent;
			printf("Average: %lf, Tests: %d\n", in_average, aids_conf.network_recent);
		}
		fclose(data_file);
	}
}
