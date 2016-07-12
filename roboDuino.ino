#include "RoboDuino.h"
String interfaceList[NUM_PARTS][3];
String pinList[NUM_PARTS];
RoboDuino robo;

void setup() {
  //Init serial connection
  Serial.begin(9600);
  interfaceList[0][PART_NAME] = "A Name";
  interfaceList[0][PART_TYPE] = "servo";
  interfaceList[0][PART_DESCRIPTION] = "A description of a servo";
  
  interfaceList[1][PART_NAME] = "The second";
  interfaceList[1][PART_TYPE] = "servo";
  interfaceList[1][PART_DESCRIPTION] = "Not another description yes of a servo";
  
  
  pinList[0] = "9"; //CHANGE ME!
  pinList[1] = "10"; //CHANGE ME!

  robo.init(interfaceList, pinList);
  
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
