#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <logging.h>

void logger(const char *str, FILE *logfile, bool terminal) {
    // get the current (ish) time and chop off the newline
    time_t current_time = time(NULL);
    char* time_str = ctime(&current_time);
    time_str[strlen(time_str)-1] = '\0';
    // if terminal is set, we print
    if (terminal) {
        printf("%s: %s\n", time_str, str);
    }
    // if logfile is not null we also write to file
    if (logfile != NULL) {
        fprintf(logfile, "%s: %s\n", time_str, str);
    }
}

void dataLogger(const char *str, FILE *logfile, bool terminal) {
    // if terminal is set, we print
    if (terminal) {
        printf("%s", str);
    }
    // if logfile is not null we also write to file
    if (logfile != NULL) {
        fprintf(logfile, "%s", str);
    }
}

void error(const char * str) {
    // just print for now
    printf("err: %s\n", str);
}