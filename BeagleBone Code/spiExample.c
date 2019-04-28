/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define default_trid 0x0011
#define poll 0x0001
#define giveData 0x0010;

uint16_t prev_trid;
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev1.0";
static uint32_t mode;
static uint8_t bits = 16;
static uint32_t speed = 250000;
static uint16_t delay;
static int verbose;


uint16_t default_tx1;
uint16_t default_rx1;
uint16_t default_tx;
uint16_t default_rx;


char *input_tx;


static void transfer(int fd, uint16_t const *tx, uint16_t const *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");
	
}


int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	uint16_t *tx;
	uint16_t *rx;
	int size;

	//parse_opts(argc, argv);

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	
	prev_trid=default_trid;
	uint64_t k,l;
	{
		while(1)
		{

		default_tx=poll;
	    default_rx=0;

		transfer(fd, &default_tx, &default_rx, 2/*sizeof(default_tx)*/);
		
		for(k=0;k<1;k++) //sizeof(default_tx)
		{
			//printf("tx1 is %x\n",default_tx);
		}
		
		for(k=0;k<1;k++) //sizeof(default_rx)
		{
			printf("rx1 is %x\n",default_rx);
		}
		
		//if((default_rx & 0xff00) != (prev_trid & 0xff00) )
		if(default_rx != prev_trid )
		{
			printf("Entered if condition\n");
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);

			default_tx1=giveData;
			default_rx1=0;

			transfer(fd, &default_tx1, &default_rx1, 2/*sizeof(default_tx)*/);
			for(k=0;k<1;k++) //sizeof(default_tx)
			{
				//printf("requesting valid data %x\n",default_tx1);
			}
			for(k=0;k<1;k++) //sizeof(default_rx)
			{
				printf("valid data is %x\n",default_rx1);
			}		
			prev_trid= default_rx;

		}
			
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);
			for(k=0;k<100000;k++);

		}
		
	}


	close(fd);

	return ret;
}
