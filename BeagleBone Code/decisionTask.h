#include <stdint.h>
#define TEMP_THRESHOLD 30
#define SOILMOISTURE_THRESHOLD 100

uint8_t getCommand(uint16_t);
void *decisionTaskRoutine(void *);