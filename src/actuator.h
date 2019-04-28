/*
 * actuator.h
 *
 *  Created on: Apr 22, 2019
 *      Author: punee
 */

#ifndef SRC_ACTUATOR_H_
#define SRC_ACTUATOR_H_

typedef struct
{
    uint8_t source;
    uint16_t sensing_data;
    uint8_t actuation_data;
    uint8_t task;
}LCDStruct;

void LCDTask(void *pvParameters);
void FanTask(void *pvParameters);
void MotorTask(void *pvParameters);
void MotorCallback();

#endif /* SRC_ACTUATOR_H_ */
