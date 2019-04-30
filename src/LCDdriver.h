/*
 * LCDdriver.h
 *
 *  Created on: Apr 15, 2019
 *      Author: nachiket kelkar & puneet bansal
 */

#ifndef SRC_LCDDRIVER_H_
#define SRC_LCDDRIVER_H_

/*
 * Function name: lcd_init()
 * Description:   This function initializes all the GPIO pins required by the LCD.
 * @param:        void
 * @return:       void
 */
void lcd_init();


/*
 * Function name: lcd_write_data()
 * Description:   This function takes the data to write writes at the cursor position
 *                on LCD
 * @param:        char(character to be written)
 * @return:       void
 */
void lcd_write_data(char data);


/*
 * Function name: lcd_write_command()
 * Description:   This function takes the command for LCD and executes the command of LCD
 * @param:        uint8_t(command to be executed)
 * @return:       void
 */
void lcd_write_command(uint8_t command);


/*
 * Function name: latch_data()
 * Description:   This function toggles the Enable pin of LCD so that data is latched
 * @param:        void
 * @return:       void
 */
void latch_data();


/*
 * Function name: lcd_pos()
 * Description:   This function takes the row and column to postion the cursor
 * @param:        uint8_t(row of LCD), uint8_t(column of LCD)
 * @comment:      The valid row values are: 0,1,2,3 and valid position values are
 *                from 0 to 15.
 * @return:       void
 */
void lcd_pos(uint8_t row, uint8_t position);


/*
 * Function name: delay()
 * Description:   This function generates the required delay
 * @param:        uint16_t(delay value)
 * @return:       void
 */
void delay(uint16_t x);


/*
 * Function name: lcd_on()
 * Description:   This function configures the LCD and places the cursor at the home position
 * @param:        void
 * @return:       void
 */
void lcd_on();


/*
 * Function name: lcd_write_string()
 * Description:   This function accepts the string and displays it on LCD
 * @param:        char* (string to be written to LCD)
 * @return:       void
 */
void lcd_write_string(char* data);


/*
 * Function name: lcd_print_digit()
 * Description:   This function prints the numbers on thee display
 * @param:        long (number to be printed)
 * @return:       void
 */
void lcd_print_digit(long x);


/*
 * Function name: lcd_print_float()
 * Description:   This function displays the float values on the screen
 * @param:        float (float value to be printed)
 * @return:       void
 */
void lcd_print_float(float no);

#define CLEAR_DISPLAY lcd_write_command(0x01)
#define CURSOR_HOME   lcd_write_command(0x02)
#define ENTRY_MODE    lcd_write_command(0x06)
#define DISPLAY_ON    lcd_write_command(0x0C)
#define DISPLAY_OFF   lcd_write_command(0x08)
#define DISPLAY_SHIFT lcd_write_command(0x10)
#define SYSTEM_SET    lcd_write_command(0x38)


#endif /* SRC_LCDDRIVER_H_ */
