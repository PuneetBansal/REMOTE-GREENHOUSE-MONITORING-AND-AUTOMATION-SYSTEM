mainTask: mainTask.o
	arm-linux-gnueabihf-gcc mainTask.c spiTask.c myTimer.c decisionTask.c mq.c loggerTask.c gpio.c -o mainTask -pthread -lrt
spi: spi.c
	gcc spi.c -o spi	
spi_cc: spi.c gpio.c
	arm-linux-gnueabihf-gcc spi.c gpio.c -o spi
spi_ec: spiExample.c 
	arm-linux-gnueabihf-gcc spiExample.c -o spi_ec
gpiotest: gpiotest.c gpio.c
	arm-linux-gnueabihf-gcc gpiotest.c gpio.c -o gpiotest
clean: 
	rm *.o