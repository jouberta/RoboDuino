/*
  RoboDuino.cpp - 
  Created by Nobody in particular, Today.
  Released into the public domain.
*/

#include <Arduino.h>
#include "RoboDuino.h"


//Constructor
RoboDuino::RoboDuino()
{
}

void RoboDuino::init(String partList[][3], char pinList[][3])
{
  for (int i = 0; sizeof(partList); i++)
  {
    for (int j = 0; sizeof(partList[i]); j++) {
      _partList[i][j]   = partList[i][j];
    }
  }
  for (int i = 0; sizeof(partList); i++)
  {
    for (int j = 0; sizeof(partList[i]); j++) {
      pinList[i][j]   = pinList[i][j];
    }
  }
  _setup();
}
//SerialEventHandler
void RoboDuino::doSerialEvent(char inputChar)
{

  
    // add it to the inputString:
    _inputCommand += inputChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inputChar == '\n') {
      _doCommand();
    }
}

//SerialEventHandler
void RoboDuino::_doCommand()
{
  
  char cmdtype = _inputCommand.charAt(0); // read the first letter of the command, this indicates the command type - M/motor S/servo X/switch Q/query

  if (cmdtype == 'S'){
    
    int objnumber = _inputCommand.charAt(1); //get servo number
    int trimpos = _inputCommand.indexOf('t');
    if (trimpos != -1){
      _inputCommand = _inputCommand.substring(2,trimpos);
    }
    else {
      _inputCommand = _inputCommand.substring(2);
      _inputCommand = _inputCommand.substring(0,(_inputCommand.length() - 1));
    }
    int value = objnumber;
    int pos = _inputCommand.toInt();
    
    _servos[value].write(pos);

  }
}

//Loop Event Handler
void RoboDuino::doLoopEvent()
{
  
}

//Internal setup function
void RoboDuino::_setup() 
{
  // FACTORY STUFFS
  for (int i = 0; i < 2; i++){
    //Make the interface
    if (_partList[i][INTERFACE_TYPE]== "servo") 
    {
      //interfaceInstances[i] = new Servo();
      _servos[i].attach(_pinList[i][PIN_SERVO]);
    } 
    else if(_partList[i][INTERFACE_TYPE]== "motor") 
    {
      //interfaceInstances[i] = new Servo();
      //_motors[i].attach(_pinList[i][PIN_SERVO]);      
    }
  }
}
