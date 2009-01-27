#ifndef UTILS_H
#define UTILS_H

/**
 * DEBUG level for logger
 */
#define DEBUG 0

/**
 * INFO level for logger
 */
#define INFO 1

/**
 * WARNING level for logger
 */
#define WARN 2

/**
 * ERROR level for logger
 */
#define ERROR 3

void logger(FILE *, int, const char *, ...);

#endif
