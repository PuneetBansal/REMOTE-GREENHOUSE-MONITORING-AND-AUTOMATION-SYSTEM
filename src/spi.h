/*
 * spi.h
 *
 *  Created on: Apr 15, 2019
 *      Author: nachi
 */

#ifndef SRC_SPI_H_
#define SRC_SPI_H_
#include "driverlib/ssi.h"

#define MASTER SSI_MODE_MASTER
#define SLAVE  SSI_MODE_SLAVE

#define STATE_SEND_TRID 0x01
#define STATE_SEND_DATA 0x02
#define STATE_GET_CTRL  0x03

typedef struct
{
    uint8_t source;
    uint16_t data;
}IBStruct;

/*
 * Function name: spi_init()
 * Description  : This function initializes the SPI module in Master or Slave mode and
 *                at the clock frequency input by the user.
 * @param       : uint32_t(mode to initialize the SPI Master or slave), uint32_t(clock rate)
 * @Comments    : Mode can only be MASTER or SLAVE
 * @return      : void
 */
void spi_init(uint32_t, uint32_t);


/*
 * Function name: spi_data_write()
 * Description  : This function takes the data to write and number of bytes to write from
 *                user and write the data using SPI protocol.
 * @param       : uint64_t(data to be written), uint8_t(no of bytes to write)
 * @Comments    : Maximum of 8 bytes of data can be written at a time.
 * @return      : void
 */
void spi_data_write(uint64_t, uint8_t);


/*
 * Function name: spi_data_read()
 * Description  : This function reads the byte value and return it to the user
 * @param       : void
 * @return      : uint8_t (data in the SPI data register is returned)
 */
uint16_t spi_data_read();


/*
 * Function name: InterBoardSPI()
 * Description  : This function contains the logic for the inter-board communication task
 * @param       : void *
 * @return      : void
 */
void InterBoardSPI(void *pvParameters);


/*
 * Function name: decode_message()
 * Description  : This function gets the source from the control message and parameter and
 *                passes the actuation message to appropriate queue of actuator.
 * @param       : uint16_t (control message received from the beagle bone)
 * @return      : void
 */
void decode_message(uint16_t ctrl_msg);
void test_function();

void self_control(IBStruct rec_msg);
#endif /* SRC_SPI_H_ */
