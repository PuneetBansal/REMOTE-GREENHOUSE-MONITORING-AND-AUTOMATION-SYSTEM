/*
 * sensor.h
 *
 *  Created on: Apr 21, 2019
 *      Author: nachi
 */

#ifndef SRC_SENSOR_H_
#define SRC_SENSOR_H_

#define TEMP_SPI_CLK 500000
/*
 * Function name: moisture_sensor_init()
 * Description  : This function initializes ADC to get the analog voltage from soil moisture
 *                sensor and convert it to the digital value.
 * @param       : void
 * @return      : void
 */
void moisture_sensor_init();


/*
 * Function name: moisture_data()
 * Description  : This function gets the digital from ADC module.
 * @param       : void
 * @return      : uint32_t(Digital converted data from ADC)
 */
uint32_t moisture_data();


/*
 * Function name: temp_sens_init()
 * Description:   This function initializes the SSI3 module of the TIVA board which is used for
 *                SPI communication with the temperature sensor.
 * @param:        uint32_t(mode for SPI), uint32_t(clock speed)
 * @comments:     The mode can be Master or Slave
 * @return:       void
 */
void temp_sens_init(uint32_t mode, uint32_t clk_speed);


/*
 * Function name: temp_data_get()
 * Description:   This function gets the data from temperature sensor data and return it.
 * @param:        void
 * @return:       uint16_t (data received from temperature sensor register to Tiva)
 */
uint16_t temp_data_get();


/*
 * Function name: TemperatureTask()
 * Description:   This function executed which contains the logic for temperature sensor task.
 * @param:        void
 * @return:       void *
 */
void TemperatureTask(void *pvParameters);


/*
 * Function name: TemperatureCallback()
 * Description:   The callback function executed when the timer elapses. The temperature task
 *                is notified when this timer is expired.
 * @param:        void
 * @return:       void
 */
void TemperatureCallback();


/*
 * Function name: SoilMoistureTask()
 * Description:   This function executed which contains the logic for soil moisture sensor task.
 * @param:        void
 * @return:       void *
 */
void SoilMoistureTask(void *pvParameters);


/*
 * Function name: MoistureCallback()
 * Description:   The callback function executed when the timer elapses. The soilMoisture task
 *                is notified when this timer is expired.
 * @param:        void
 * @return:       void
 */
void MoistureCallback();

float temperature_in_c(uint16_t hex_val);

#endif /* SRC_SENSOR_H_ */
