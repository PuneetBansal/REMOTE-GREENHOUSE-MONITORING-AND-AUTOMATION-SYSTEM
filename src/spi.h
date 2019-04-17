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
#define SLAVE SSI_MODE_SLAVE

/*
 * Function name: spi_init()
 * Description:   This function initializes the SPI module in Master or Slave mode and
 *                at the clock frequency input by the user.
 * @param:        uint32_t(mode to initialize the SPI Master or slave), uint32_t(clock rate)
 * @Comments:     Mode can only be MASTER or SLAVE
 * @return:       void
 */
void spi_init(uint32_t mode, uint32_t clk_speed);


/*
 * Function name: spi_data_write()
 * Description:   This function takes the data to write and number of bytes to write from
 *                user and write the data using SPI protocol.
 * @param:        uint64_t(data to be written), uint8_t(no of bytes to write)
 * @Comments:     Maximum of 8 bytes of data can be written at a time.
 * @return:       void
 */
void spi_data_write(uint64_t data_to_write, uint8_t no_of_bytes);


/*
 * Function name: spi_data_read()
 * Description:   This function reads the byte value and return it to the user
 * @param:        void
 * @return:       uint8_t (data in the SPI data register is returned)
 */
uint8_t spi_data_read();

#endif /* SRC_SPI_H_ */
