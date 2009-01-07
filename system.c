#include "system.h"

int load_average(struct load_average *avg)
{
	int r, i;
	double load_avg[3];

	r = getloadavg(load_avg, 3);
	if (r == -1)
	{
		return -1;
	} else
	{
		avg -> measures = r;
		for (i = 0; i < r; i++)
			avg -> data[i] = load_avg[i];
		return 1;
	}
}
