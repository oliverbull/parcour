#include <stdbool.h>
#include <stdio.h>

// logging methods
void logger(const char *str, FILE *logfile, bool terminal);
void dataLogger(const char *str, FILE *logfile, bool terminal);
void error(const char *str);