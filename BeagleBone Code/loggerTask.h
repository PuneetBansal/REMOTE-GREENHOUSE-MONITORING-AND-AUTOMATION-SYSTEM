#include "mq.h"
#include <time.h>

void *loggerTaskRoutine(void *);
char* printTimeStamp();
void logToFile(char *, logStruct);