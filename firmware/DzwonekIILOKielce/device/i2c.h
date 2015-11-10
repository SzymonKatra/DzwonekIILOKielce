#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

void i2cInit();

uint8_t i2cStart(uint8_t address); // returns 1 if acknowledged, otherwise 0
void i2cStartWait(uint8_t address);
void i2cStop();
void i2cWrite(uint8_t data);
uint8_t i2cReadAck();
uint8_t i2cReadNack();

#endif /* I2C_H_ */
