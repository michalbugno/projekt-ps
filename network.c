#include <unistd.h>
#include <pthread.h>
#include "network.h"
#include "main.h"

extern struct aids_global_conf aids_conf;

/**
 * Method to gather network data to a structure.
 *
 * @param dev string name of network device which shall be sniffed.
 * @param traffic pointer to an initialized structure to which the data will be saved.
 *
 * \todo Modify network sniffer to gather in and out data separately.
 */
void network_usage(const char *dev, struct network_traffic *traffic)
{
	pcap_t *handle;
	struct network_stats stats;
	struct pcap_pkthdr header_start, header_finish;
	double seconds;


	seconds = 0.0;

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


	while (1)
	{
		data_file = fopen("data/current_traffic.dat", "w");
		if (data_file == NULL)
		{
			perror("[network.c] Couldn't open file data/current_traffic.dat for writing");
			pthread_exit(NULL);
		}
		fclose(data_file);
		
		for(i = 0; i < aids_conf.recent_network ; i += 1)
		{
			network_usage("en1", &traffic);
			data_file = fopen("data/current_traffic.dat", "a");
			if (data_file == NULL)
			{
				perror("[network.c] Couldn't open file data/current_traffic.dat for writing");
				pthread_exit(NULL);
			}
			fprintf(data_file, "%lf,%lf\n", traffic.in, traffic.out);
			fclose(data_file);
		}

		sleep(aids_conf.network_timeout);
	}
}
