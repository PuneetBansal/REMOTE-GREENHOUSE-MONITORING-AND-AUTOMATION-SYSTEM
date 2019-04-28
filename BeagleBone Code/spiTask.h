#include "myTimer.h"


#define POLL_REQ 0x0001
#define DATA_REQ 0X0002
#define DEFAULT_TRID 0x00
#define SOURCE_BITMASK 0x00ff
#define TRID_BITMASK 0xff00
uint8_t prev_trid;

void *spiTaskRoutine(void *);
int spi_init();
void spi_transfer(int , uint16_t const *, uint16_t const *, size_t);

