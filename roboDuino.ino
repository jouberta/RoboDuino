
#include "RoboDuino.h"
#include <MemoryFree.h>

RoboDuino robo;

void setup() {
  //Init serial connection
  Serial.begin(9600);
  // Configure Part List details
  struct PartList partList[NUM_PARTS] = {
    {TYPE_BUTTON,"S",A0},
    {TYPE_BUTTON,"W",A3},
    {TYPE_BUTTON,"A",A2},
    {TYPE_BUTTON,"D",A1},
    {TYPE_SERVO, "R", 10},
    {TYPE_SERVO, "L", 9}
  };
  robo.init(partList);  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  robo.doLoopEvent();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char readChar = (char)Serial.read();
    robo.doSerialEvent(readChar);
  }
}
