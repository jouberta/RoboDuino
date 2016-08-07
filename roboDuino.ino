
#include "RoboDuino.h"

RoboDuino robo;

void setup() {
  //Init serial connection
  Serial.begin(9600);
  // Configure Part List details
  struct Output outputList[NUM_OUTPUTS] = {
    {TYPE_SERVO, "R", {10, -1, -1}, 90, false},
    {TYPE_SERVO, "L", {9, -1, -1}, 90, false},
    {TYPE_MOTOR, "M", {2, 4, 3}, 0, false}
  };
  struct Input inputList[NUM_INPUTS] = {
    {TYPE_BUTTON_INC,"S",A0, 1, true},
    {TYPE_BUTTON_DEC,"W",A3, 1, true},
    {TYPE_BUTTON_INC,"A",A2, 0, true},
    {TYPE_BUTTON_DEC,"D",A1, 0, true}
  };
  // Initialise everything!
  robo.init(inputList, outputList);  
}

void loop() {
  // Do our loop event  
  robo.doLoopEvent();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char readChar = (char)Serial.read();
    // Do our serial event
    robo.doSerialEvent(readChar);
  }
}
