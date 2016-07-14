#include "RoboDuino.h"
String partList[NUM_PARTS][PART_PROPERTY_COUNT];
String pinList[NUM_PARTS];
RoboDuino robo;

void setup() {
  //Init serial connection
  Serial.begin(9600);
  
  /**
   * TODO: Change PART_TYPE to be an int and use Constants
   *       will need to move it to a different array of type
   *       int to save ram
   */
  partList[0][PART_NAME] = "A Name";
  partList[0][PART_TYPE] = "servo";
  partList[0][PART_DESCRIPTION] = "A description of a servo";
  partList[0][COMMAND_IDENTIFIER] = "S";
  
  
  partList[1][PART_NAME] = "The second";
  partList[1][PART_TYPE] = "servo";
  partList[1][PART_DESCRIPTION] = "Not another description yes of a servo";
  partList[1][COMMAND_IDENTIFIER] = "N";
  
  partList[2][PART_NAME] = "THe 1 and 0s";
  partList[2][PART_TYPE] = "digital_output";
  partList[2][PART_DESCRIPTION] = "Yay or nay";
  partList[2][COMMAND_IDENTIFIER] = "D";
  
  
  pinList[0] = "9"; //CHANGE ME!
  pinList[1] = "10"; //CHANGE ME!
  pinList[2] = "A1"; //CHANGE ME!

  robo.init(partList, pinList);
  
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
