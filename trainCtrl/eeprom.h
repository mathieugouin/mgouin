/**
 * I2C serial EEPROM
 */

#ifndef I2C_EEPROM_H
#define I2C_EEPROM_H

#include "common.h"

void i2c_write_eeprom(uint16_t address, uint8_t data);
uint8_t i2c_read_eeprom(uint16_t address);

#endif /* I2C_EEPROM_H */

