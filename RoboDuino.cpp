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

//SerialEventHandler
void RoboDuino::doSerialEvent(char inputChar)
{  
    // add it to the inputString:
    _inputCommand += inputChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inputChar == '\n') {
      _doCommand();
      _inputCommand = "";
    }
}

//SerialEventHandler
void RoboDuino::_doCommand()
{
  
  char cmdtype = _inputCommand[0]; // read the first letter of the command, this indicates the command type - M/motor S/servo X/switch Q/query

  if (cmdtype == 'S'){
    
    int servoNumber = (int)_inputCommand[1] - 48; //get servo number

    int pos = _inputCommand.substring(2, (_inputCommand.length() - 1)).toInt();

    _servos[servoNumber].write(pos);
  }
}

//Loop Event Handler
void RoboDuino::doLoopEvent()
{
  
}

//setup function
void RoboDuino::init(String partList[][3], String pinList[]) 
{
  
  for (int i = 0; i < sizeof(partList); i++) 
  {
    //set the parts list
    _partList[i][0] = partList[i][0];
    _partList[i][1] = partList[i][1];
    _partList[i][2] = partList[i][2];
    
    _pinList[i] = pinList[i];
  }
  
  // FACTORY STUFFS
  for (int i = 0; i < NUM_PARTS; i++)
  {
    //Make the interface
    if (_partList[i][PART_TYPE] == "servo") 
    {
      _servos[i].attach(_pinList[i].toInt());
    } 
    else if(_partList[i][PART_TYPE]== "motor") 
    {
      //interfaceInstances[i] = new Servo();
      //_motors[i].attach(_pinList[i][PIN_SERVO]); 
    }
  }
}

String RoboDuino::getName(int pos1, int pos2)
{  
  return _partList[pos1][pos2];
}
