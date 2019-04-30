/*
 * actuator.h
 *
 *  Created on: Apr 22, 2019
 *      Author: puneet bansal and Nachiket Kelkar
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


/*
 * Function name: LCDTask()
 * Description:   This function is the task to be exectued for LCD. It waits for the
 *                data from the sensor and then displays it on the LCD.
 * @param:        void
 * @return:       void
 */
void LCDTask(void *pvParameters);


/*
 * Function name: FanTask()
 * Description:   This tasks waits for the control messages and switches ON the
 *                temperature controller fan or switches it ON based on the control
 *                message from controller node.
 * @param:        void
 * @return:       void
 */
void FanTask(void *pvParameters);


/*
 * Function name: MotorTask()
 * Description:   This function waits for the message from control node. It controls the
 *                PWM duty cycle based on the control message from controller node.
 * @param:        void
 * @return:       void
 */
void MotorTask(void *pvParameters);


/*
 * Function name: MotorCallback()
 * Description:   This function is the timer call back to generate the PWM signal
 *                for controlling the motor.
 * @param:        void
 * @return:       void
 */
void MotorCallback();

#endif /* SRC_ACTUATOR_H_ */
