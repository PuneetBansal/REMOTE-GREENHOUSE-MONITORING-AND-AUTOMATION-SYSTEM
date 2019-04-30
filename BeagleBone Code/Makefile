mainTask: mainTask.o
	arm-linux-gnueabihf-gcc mainTask.c spiTask.c decisionTask.c mq.c loggerTask.c gpio.c -o mainTask -pthread -lrt
clean: 
	rm *.o