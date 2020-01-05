#ifndef _Coin_Dispenser_H_
#define _Coin_Dispenser_H_

class Coin_Dispenser {
public:
	void begin(uint8_t i2caddr = 0x78);

	void dispense(uint8_t count);
	void setSpeed(uint8_t speed);
	
	uint8_t readState();
	uint8_t readQueuedCount();
private:
	uint8_t i2caddr;
	uint8_t speed = 0;
	uint8_t readRegister(uint8_t addr);
	void writeRegister(uint8_t addr, uint8_t value);
};

//Registers

#define REG_DISPENSE 0x01 //WRITE
#define REG_DISPENSE_QUEUED 0x02 //READ
#define REG_DISPENSE_SPEED 0x03 //WRITE
#define REG_DISPENSE_STATE 0x04 //READ

#define DISPENSE_STATE_OK 0x00
#define DISPENSE_STATE_LOW 0x01
#define DISPENSE_STATE_BUSY 0x02

#endif