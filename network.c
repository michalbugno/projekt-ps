#include "network.h"

long network_usage(const char *dev)
{
	pcap_t *handle;

	handle = pcap_open_live(dev, 99999, 1, 1000, errbuf);
	if (handle == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return(1);
	}

	pcap_loop(handle, 100, got_packet, NULL);
	
	return(0);
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	printf("length of portion: %d, packet lenth: %d\n", header -> caplen, header -> len);
}
