#include <Wire.h>
#include <Servo.h>

static const int sensorPin = 6; //Output from the TCRT5000 IR sensor
static const int servoPin = 7;

#define SLAVE_ADDRESS 0x78

#define REG_DISPENSE 0x01 //WRITE
#define REG_DISPENSE_QUEUED 0x02 //READ
#define REG_DISPENSE_SPEED 0x03 //WRITE
#define REG_DISPENSE_STATE 0x04 //READ

#define DISPENSE_STATE_OK 0x00
#define DISPENSE_STATE_LOW 0x01
#define DISPENSE_STATE_BUSY 0x02

uint8_t current_reg = 0x00;
uint8_t dispense_count = 0;
uint8_t dispense_speed = 0;
uint8_t dispense_state = 0;
bool dispense_low;

bool last_10_reads[10] = {};
uint8_t read_counter = 0; //0-9
unsigned long read_millis = 0;

unsigned long old_millis = 0;

Servo servo;

void setup() {
  servo.attach(servoPin);
  servo.write(180);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent); // data request to slave
  Wire.onReceive(receiveEvent); // data slave received
  pinMode(sensorPin, INPUT);
  dispense_low = digitalRead(sensorPin);
  //For debugging only
  Serial.begin(115200);
  Serial.println("I2C Slave ready!");
  old_millis = millis();
  read_millis = millis();
}
void processData(uint8_t* buff, uint8_t len) {
  current_reg = buff[0];
  switch (current_reg) {
    case REG_DISPENSE:
      dispense_count = buff[1];
      break;
    case REG_DISPENSE_SPEED:
      dispense_speed = (buff[1] < 15) ? buff[1] : 15; //Cap to 15
      Serial.println(String(dispense_speed));
      break;
    default:
      break;
  }
  /*for (int i = 0; i < len; i++)
    {
    Serial.print(buff[i]);
    }
    Serial.println();*/
  free(buff);
}
void receiveEvent(int len) {
  Serial.println("Receive event");
  if (len > 0) {
    uint8_t* buff = (uint8_t*)malloc(len);
    for (int i = 0; i < len; i++)
    {
      buff[i] = Wire.read();
    }
    processData(buff, len);
  }
}

void requestEvent() {
  uint8_t response[1] = {0x00};
  switch (current_reg) {
    case REG_DISPENSE_QUEUED:
      response[0] = dispense_count;
      break;
    case REG_DISPENSE_STATE:
      if (dispense_low) {
        dispense_state = DISPENSE_STATE_LOW;
      } else {
        if (dispense_count > 0) {
          dispense_state = DISPENSE_STATE_BUSY;
        } else {
          dispense_state = DISPENSE_STATE_OK;
        }
      }
      response[0] = dispense_state;
      break;
    default:
      break;
  }
  Wire.write(response, sizeof(response));
  Serial.println("Request event");
}
uint16_t servoSweep = 0; //From 0 to 180 and 181 to 361
void loop() {
  uint8_t dispense_delay = 20 - dispense_speed;
  if (dispense_count > 0 && (servoSweep > 0 || !dispense_low)) {
    //Conditions are
    //dispense_count must be more than 1
    //servoSweep is in progress or dispense_low is false
    if (millis() > old_millis + dispense_delay) {
      old_millis = millis();
      if (servoSweep < 181) {
        servo.write(180 - servoSweep);
      } else {
        servo.write(servoSweep - 181);
      }
      servoSweep++;
      if (servoSweep > 361) {
        servoSweep = 0;
        dispense_count--;
      }
    }
  }

  if (millis() > read_millis + 200) {
    //This code ensures that the ir sensor provides a consistent output for 2 seconds before we are sure that the coin dispenser if filled up or empty
    read_millis = millis();
    last_10_reads[read_counter] = digitalRead(sensorPin);
    read_counter++;
    if (read_counter > 9) {
      read_counter = 0;
      bool firstRead = last_10_reads[0];
      bool consistent = true;
      for (int i = 1; i < 9; i++) {
        if (firstRead != last_10_reads[i]) {
          consistent = false;
          Serial.println("Not consistent");
        }
        break;
      }
      if (consistent) {
        dispense_low = firstRead;
        Serial.println(String(dispense_low));
      }
    }
  }
}