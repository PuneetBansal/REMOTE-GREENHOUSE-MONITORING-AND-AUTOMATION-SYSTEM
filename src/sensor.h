/*
 * sensor.h
 *
 *  Created on: Apr 21, 2019
 *      Author: nachi
 */

#ifndef SRC_SENSOR_H_
#define SRC_SENSOR_H_

void adc_init();
uint32_t adc_get_data();
void temp_sens_init(uint32_t mode, uint32_t clk_speed);
uint8_t temp_data_get();
void TemperatureTask(void *pvParameters);
void TemperatureCallback();
void SoilMoistureTask(void *pvParameters);
void MoistureCallback();

#endif /* SRC_SENSOR_H_ */
