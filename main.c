#include "network.h"

int main(int argc, char **argv)
{
	int i;

	for (i = 0; i < 1000; i++)
	{
		network_usage("en1");
	}
	return 0;
}
