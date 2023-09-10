#ifndef _UTILS_H_

#define _UTILS_H_
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

void scc(int code);
void *scp(void *handler);
void *icp(void *handler);

#define DEBUG(format, args...) debug(__FILE__, __LINE__, (format), ##args)
int sq_number_sqrt(int num);

void debug(const char *file, int line, const char *format, ...);

#endif