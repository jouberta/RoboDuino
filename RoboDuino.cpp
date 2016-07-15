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

//setup function
void RoboDuino::init(String partList[][PART_PROPERTY_COUNT], String pinList[]) 
{
  // Set all the counters to zero
  _servoCounter = 0;
  
  for (int i = 0; i < NUM_PARTS; i++) 
  {
    //set the parts list
    _partList[i][0] = partList[i][0];
    _partList[i][1] = partList[i][1];
    _partList[i][2] = partList[i][2];
    _partList[i][3] = partList[i][3];
    
    _pinList[i] = pinList[i];
  }
  
  // FACTORY STUFFS
  for (int i = 0; i < NUM_PARTS; i++)
  {
    //Make the interface
    if (_partList[i][PART_TYPE] == "servo") 
    {
      _servos[_servoCounter].attach(_pinList[i].toInt());
      _partListToTypeRelation[i] = _servoCounter;
      _servoCounter++;
    } 
    else if(_partList[i][PART_TYPE]== "analogue_out" || _partList[i][PART_TYPE]== "digital_out") 
    {
      pinMode(_pinList[i].toInt(), OUTPUT);
      _partListToTypeRelation[i] = i;
    }
    else if(_partList[i][PART_TYPE]== "motor") 
    {
      //interfaceInstances[i] = new Servo();
      //_motors[i].attach(_pinList[i][PIN_SERVO]); 
    }
  }
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
  
  char cmdID = _inputCommand[0]; // read the first letter of the command, this indicates the command type - M/motor S/servo X/switch Q/query
  int commandValue = _inputCommand.substring(1, (_inputCommand.length() - 1)).toInt();

  for(int i = 0; i < NUM_PARTS; i++)
  {
    if (_partList[i][COMMAND_IDENTIFIER] == (String)cmdID)
    {
      if (_partList[i][PART_TYPE] == "servo")
      {
          int servoNumber = _partListToTypeRelation[i];
          _servos[servoNumber].write(commandValue);
      }
      else if(_partList[i][PART_TYPE] == "digital_output")
      {
          int pinNumber = _partListToTypeRelation[i];
          digitalWrite(_pinList[pinNumber].toInt(), commandValue);
      }
      else if(_partList[i][PART_TYPE] == "analogue_output")
      {
          int pinNumber = _partListToTypeRelation[i];
          analogWrite(_pinList[pinNumber].toInt(), commandValue);
      }
    }
  }
  
}

//Loop Event Handler
void RoboDuino::doLoopEvent()
{
  
}

String RoboDuino::getName(int pos1, int pos2)
{  
  return _partList[pos1][pos2];
}
