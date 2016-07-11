#include "RoboDuino.h"

String interfaceList[2][3];
char pinList[2][3];
RoboDuino robo;

void setup() {
  //Init serial connection
  Serial.begin(9600);
  
  interfaceList[0][INTERFACE_NAME] = "A Name";
  interfaceList[0][INTERFACE_TYPE] = "servo";
  interfaceList[0][INTERFACE_DESCRIPTION] = "A description of a servo";
  
  interfaceList[1][INTERFACE_NAME] = "The second";
  interfaceList[1][INTERFACE_TYPE] = "servo";
  interfaceList[1][INTERFACE_DESCRIPTION] = "Not another description yes of a servo";
  
  
  pinList[0][PIN_SERVO] = 9; //CHANGE ME!
  pinList[1][PIN_SERVO] = 10; //CHANGE ME!

  robo.init(interfaceList, pinList);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void serialEvent() {
  // reads bytes from serial until a newline is detected
  // serial_received then become true so the main program knows there is a command to process
  while (Serial.available()) {
    // get the new byte:
    robo.doSerialEvent((char)Serial.read());
  }
}
