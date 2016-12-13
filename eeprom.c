/**
 * I2C serial EEPROM
 */

#include "eeprom.h"

/************************************************************************
* Description : This function writes a uint8_t in the i2c serial eeprom
* Parameters :  address, the address value where the data is written
*               data, the data value to be written in eeprom
* Return :      void
************************************************************************/
void i2c_write_eeprom(uint16_t address, uint8_t data)
{
    // to know if eeprom has acknoledged (ok to write), 0 = ack, 1 = no ack
    bool ack;
    
    // To prevent writing value if it is the same value already written.
    // This will extends EEPROM life.
    if (data != i2c_read_eeprom(address))
    {
        do      // loop to eeprom ack polling
        {
            i2c_start();                    // start condition on i2c bus
            ack = i2c_write(I2C_EEPROM_W);  // send write command to eeprom
        } while(ack);                       // loop if not ack ( = 1 ) :
                                            // eeprom is not ready

        i2c_write(address >> 8);            // Send address MSB
        i2c_write(address & 0x00FF);        // Send address LSB
        i2c_write(data);                    // Send the data to be writte at the address
        i2c_stop();
    }
}


/************************************************************************
* Description : This function reads a uint8_t from i2c serial eeprom
* Parameters :  address, the address value where the data is read
* Return :      data, the data value read from eeprom
************************************************************************/
uint8_t i2c_read_eeprom(uint16_t address)
{
    uint8_t data = 0;
    bool ack;                           // to know if eeprom has acknoledged (ok to write)
                                        // 0 = ack, 1 = no ack

    do                                  // loop to eeprom ack polling
    {
        i2c_start();                    // start condition on i2c bus
        ack = i2c_write(I2C_EEPROM_W);  // send write command to eeprom
    } while(ack);                       // loop if not ack ( = 1 ) : eeprom is not ready

    i2c_write(address >> 8);            // Send address MSB
    i2c_write(address & 0x00FF);        // Send address LSB

    i2c_start();                        // new communication, we are not writing in fact...
    i2c_write(I2C_EEPROM_R);            // send read command to eeprom
    data = i2c_read(0);                 // Send the data to be written at the address
                                        // not ack to discontinue transmission
    i2c_stop();                         // stop the i2c communication

    return data;
}

