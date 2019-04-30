#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <time.h>
#include <signal.h>
#include <linux/spi/spidev.h>
#include "gpio.h"

uint8_t spi_handler;
uint8_t connection_handler;
uint8_t recoveryIndiacation;
uint8_t printOnlyOnce;
