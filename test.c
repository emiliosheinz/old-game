#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return 0;
}