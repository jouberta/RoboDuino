#include "RoboDuino.h"
RoboDuino robo;

void setup() {
  //Init serial connection
  Serial.begin(9600);

  // Configure Part List details
  struct PartList partList[NUM_PARTS] = {
    {"Light",TYPE_DOUT,"A light output on or off","T",A1},
    {"Right Servo", TYPE_SERVO, "A description of a servo", "R", 9},
    {"Left Servo", TYPE_SERVO, "A description of a servo", "L", 10}
  };
  robo.init(partList);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(50);
}

void serialEvent() {
  // reads bytes from serial until a newline is detected
  // serial_received then become true so the main program knows there is a command to process
  while (Serial.available()) {
    // get the new byte:
    char readChar = (char)Serial.read();
    robo.doSerialEvent(readChar);
  }
}
