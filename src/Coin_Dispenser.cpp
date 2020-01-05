#include <Wire.h>
#include "Coin_Dispenser.h"

/**
	Returns the 8 bit value at the specified register address.

	@param addr Register address
	@return value at address
*/
uint8_t Coin_Dispenser::readRegister(uint8_t addr){
	Wire.beginTransmission(i2caddr);
	Wire.write(addr);
	Wire.endTransmission();
	Wire.requestFrom(i2caddr, 1);
	return Wire.read();
}

/**
	Writes the 8 bit value to the specified register address.

	@param regAddr Register address
	@param regValue Value to write
*/
void Coin_Dispenser::writeRegister(uint8_t regAddr, uint8_t regValue){
	Wire.beginTransmission(i2caddr);
	Wire.write(regAddr);
	Wire.write(regValue);
	Wire.endTransmission();
}

////////////////////////////////////////////////////////////////////////////////

/**
	Save the slave address of the coin dispenser,
	this function does not start i2c.

	@param addr Slave address
*/
void Coin_Dispenser::begin(uint8_t i2caddr) {
	this->i2caddr = i2caddr;
}

/**
	Tell the coin dispenser to dispense count number of coins

	@param count Number of coins to be dispensed
*/
void Coin_Dispenser::dispense(uint8_t count) {
	writeRegister(REG_DISPENSE, count);
}

/**
	Set the speed of the coin dispenser servo

	@param speed From 0-15
*/
void Coin_Dispenser::setSpeed(uint8_t speed) {
	writeRegister(REG_DISPENSE_SPEED, speed);
}

/**
	Read the state of coin dispenser

	@return state
*/
uint8_t Coin_Dispenser::readState(){
	return readRegister(REG_DISPENSE_STATE);
}

/**
	Read the number of coins queued to be dispensed

	@return Number of coins
*/
uint8_t Coin_Dispenser::readQueuedCount(){
	return readRegister(REG_DISPENSE_QUEUED);
}