#include "i2c_lcd.h"

static void i2c_lcd_write_command(uint8_t value);
static void i2c_lcd_write_command4b(uint8_t value);


/************************************************************************
* Description : Sends a data value to the LCD by placing the RS pin
*               at HIGH and sends the Enable signal.
*               The data is sent in 4-bit, high order nibble first.
* Parameters :  value, the data to be written to the LCD
* Return :      void
************************************************************************/
void i2c_lcd_write_data(uint8_t value)
{
    // variables containing the high and low nibble of the value
    uint8_t high_nibble, low_nibble;

    // separation of the high and low nibble.
    high_nibble = value & 0xF0;
    low_nibble = (value & 0x0F) << 4;

    i2c_start();
    i2c_write(I2C_LCD_W);

    i2c_write(high_nibble | 0x0F);  // E high, RS high
    delay_us(LCD_ENABLE_DELAY);
    i2c_write(high_nibble | 0x07);  // E low, RS high
    delay_us(LCD_ENABLE_DELAY);

    i2c_write(low_nibble | 0x0F);   // E high, RS high
    delay_us(LCD_ENABLE_DELAY);
    i2c_write(low_nibble | 0x07);   // E low, RS high
    delay_us(LCD_ENABLE_DELAY);
    i2c_write(0x0F);                // E high, RS high

    i2c_stop();
}


/************************************************************************
* Description : Sends a command to the LCD by placing the RS pin
*               at LOW and sends the Enable signal.
*               The command is sent in 4-bit, high order nibble first.
* Parameters :  value, the data to be written to the LCD
* Return :      void
************************************************************************/
static void i2c_lcd_write_command(uint8_t value)
{
    // variables containing the high and low nibble of the value
    uint8_t high_nibble, low_nibble;

    // separation of the high and low nibble.
    high_nibble = value & 0xF0;
    low_nibble = (value & 0x0F) << 4;

    i2c_start();
    i2c_write(I2C_LCD_W);

    i2c_write(high_nibble | 0x0B);  // E high, RS low
    delay_us(LCD_ENABLE_DELAY);
    i2c_write(high_nibble | 0x03);  // E low, RS low
    delay_us(LCD_ENABLE_DELAY);

    i2c_write(low_nibble | 0x0B);   // E high, RS low
    delay_us(LCD_ENABLE_DELAY);
    i2c_write(low_nibble | 0x03);   // E low, RS low
    delay_us(LCD_ENABLE_DELAY);
    i2c_write(0x0B);                // E high, RS low

    i2c_stop();
}


/************************************************************************
* Description : Sends a 4-bit command to the LCD by placing the RS pin
*               at LOW and sends the Enable signal.
* Parameters :  value, the data to be written to the LCD only the high
*               order nibble is sent
* Return :      void
************************************************************************/
static void i2c_lcd_write_command4b(uint8_t value)
{
    value = value & 0xF0;       // use high order nibble

    i2c_start();
    i2c_write(I2C_LCD_W);

    i2c_write(value | 0x0B);    // put RS low and E to high
    delay_us(LCD_ENABLE_DELAY);
    i2c_write(value | 0x03);    // put RS low and E low
    delay_us(LCD_ENABLE_DELAY);
    i2c_write(0x0B);            // put RS low and E to high

    i2c_stop();
}


/************************************************************************
* Description : Initialize the LCD.  Send 4-bit command, wait to init.
* Parameters :  void
* Return :      void
************************************************************************/
void i2c_lcd_init(void)
{
    delay_ms(16);                   // wait for LCD to boot up
    i2c_lcd_write_command4b(0x30);  // 8-bit interface
    delay_ms(5);
    i2c_lcd_write_command4b(0x30);  // 8-bit interface
    delay_us(150);
    i2c_lcd_write_command4b(0x30);  // 8-bit interface

    delay_ms(50);                   // wait BF... poor
    i2c_lcd_write_command4b(0x20);  // 4-bit interface

    i2c_lcd_write_command(0x28);    // 4-bit, 2-line, 5x7 font
    i2c_lcd_write_command(0x08);    // LCD off
    i2c_lcd_write_command(0x01);    // clear display
    i2c_lcd_write_command(0x06);    // increment, no shift
    i2c_lcd_write_command(0x0C);    // display on, no cursor, no blink
    i2c_lcd_write_command(0x02);    // return home
    delay_ms(5);
}


/************************************************************************
* Description : Position the cursor to write character at specific
*               locations on the LCD
* Parameters :  line_no, the line number (0 or 1).  If an other number
*               is entered, line 0 is default.
*               Line 0 is the top.
*               char_no, the character position (0 - 15).  Default is #0.
*               Char 0 is left.
* Return :      void
************************************************************************/
void i2c_lcd_position_cursor(uint8_t line_no, uint8_t char_no)
{
    uint8_t position_command = 0x80;   // default : line #0, char #0

    if (line_no == 1)
        position_command = 0xC0;

    if (char_no <= 15)
        position_command += char_no;

    i2c_lcd_write_command(position_command);
}


/************************************************************************
* Description : Clear the LCD and position the cursor at 0,0.
* Parameters :  void
* Return :      void
************************************************************************/
void i2c_lcd_clear(void)
{
    i2c_lcd_write_command(0x01);
}
