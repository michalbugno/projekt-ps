#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <sys/time.h>
#include "utils.h"

/**
 * Logger is used to write any kind of messages to file descriptors.
 * Useful with debugging etc.
 *
 * @param file file descriptor to which messages should be written
 * @param level level of message (defined in header: DEBUG, INFO, WARN, ERROR)
 * @param format exactly as in printf
 */
void logger(FILE *file, int level, const char *format, ...)
{
	va_list ap;
	char time[255], *level_str;
	struct timeval tp;
	struct tm *tms;

	if (level == ERROR)
		level_str = "ERROR";
	else if (level == WARN)
		level_str = "WARN";
	else if (level == INFO)
		level_str = "INFO";
	else if (level == DEBUG)
		level_str = "DEBUG";

	va_start(ap, format);
	gettimeofday(&tp, NULL);
	tms = localtime(&(tp.tv_sec));
	strftime(time, sizeof(time), "%H:%M:%S", tms);
	
	fprintf(file, "[%-5s][%s.%03d] ", level_str, time, tp.tv_usec / 1000);
	vfprintf(file, format, ap);
	fprintf(file, "\n");
	va_end(ap);
}
