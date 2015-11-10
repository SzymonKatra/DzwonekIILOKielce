#include "i2c.h"

#include <util/twi.h>

#define i2cGetState() (TWSR & 0xF8)
#define i2cWait() while (!(TWCR & (1<<TWINT)))

void i2cInit()
{
	// maximum I2C frequency for this F_CPU = 204.8 KHz
	TWBR = 0;
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
}

uint8_t i2cStart(uint8_t address)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	i2cWait();

	i2cWrite(address);

	uint8_t state = i2cGetState();
	if (state != TW_MT_SLA_ACK && state != TW_MR_SLA_ACK) return 0;

	return 1;
}
void i2cStartWait(uint8_t address)
{
	while(!i2cStart(address));
}
void i2cStop()
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while ((TWCR & (1 << TWSTO)));
}
void i2cWrite(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2cWait();
}
uint8_t i2cReadAck()
{
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
	i2cWait();
	return TWDR;
}
uint8_t i2cReadNack()
{
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2cWait();
	return TWDR;
}
