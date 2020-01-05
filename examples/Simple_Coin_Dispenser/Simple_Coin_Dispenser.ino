#include <Wire.h>
#include <Coin_Dispenser.h>
#define SLAVE_ADDRESS 0x78

Coin_Dispenser dispenser1;
void setup() {
  Wire.begin(27, 26);
  dispenser1.begin(SLAVE_ADDRESS);
  dispenser1.setSpeed(15);
  Serial.begin(115200);
  Serial.println("I2C Master ready!");
}

void loop() {
  delay(1000);
  uint8_t dispenseState = dispenser1.readState();
  Serial.println(String(dispenseState));
  if (dispenser1.readState() == 0) {
    Serial.println("Dispensing");
    dispenser1.dispense(1);
  }
}