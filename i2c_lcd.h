/**
 * I2C LCD implementation connected to a PCF8574.
 */

#ifndef I2C_LCD_H
#define I2C_LCD_H

#include "common.h"

#define LCD_ENABLE_DELAY        (uint16_t)700   // Enable signal pulse width (us)

// i2c LCD
void i2c_lcd_write_data(uint8_t value);
void i2c_lcd_init(void);
void i2c_lcd_position_cursor(uint8_t line_no, uint8_t char_no);
void i2c_lcd_clear(void);

#endif /* I2C_LCD_H */
