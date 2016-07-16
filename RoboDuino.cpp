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
void RoboDuino::init(Input inputList[NUM_INPUTS], Output outputList[NUM_OUTPUTS]) 
{
  // Set all the counters to zero
  _servoCounter = 0;

    
  // FACTORY STUFFS FOR OUTPUTS
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    //set the parts list
    _outputList[i] = outputList[i];
    //Make the interface
    if (_outputList[i].Type == TYPE_SERVO) 
    {
      _servos[_servoCounter].attach(_outputList[i].PinNumber);
      _partListToTypeRelation[i] = _servoCounter;
      _servoCounter++;
    } 
    else if(_outputList[i].Type== TYPE_AOUT || _outputList[i].Type== TYPE_DOUT)
    {
      pinMode(_outputList[i].PinNumber, OUTPUT);
      _partListToTypeRelation[i] = i;
    }
    else if(_outputList[i].Type== TYPE_MOTOR) 
    {
      //interfaceInstances[i] = new Servo();
      //_motors[i].attach(_pinList[i][PIN_SERVO]); 
    } 
    
  }
  
  // FACTORY STUFFS FOR INPUTS
  for (int i = 0; i < NUM_INPUTS; i++) 
  {
    //set the parts list
    _inputList[i] = inputList[i];
    if (_inputList[i].Type == TYPE_BUTTON || 
        _inputList[i].Type == TYPE_BUTTON_INC || 
        _inputList[i].Type == TYPE_BUTTON_DEC 
    ) 
    {
      pinMode(_inputList[i].PinNumber, INPUT_PULLUP);
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
    // TODO: make new line configurable
    if (inputChar == '\n') {
      _doCommand();
      _inputCommand = "";
    }
}

//SerialEventHandler
void RoboDuino::_doCommand()
{
  int seperatorPosition = _inputCommand.indexOf(":");
  int valueLength = _inputCommand.length() - (2 + seperatorPosition);
  
  String cmdID = _inputCommand.substring(0, seperatorPosition); // read the first letter of the command, this indicates the command type - M/motor S/servo X/switch Q/query
  String commandValue = _inputCommand.substring(seperatorPosition + 1, seperatorPosition + valueLength + 1);
  
  for(int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (_outputList[i].CommandIdentifier == cmdID)
    {
      if (_outputList[i].Type == TYPE_SERVO)
      {
          int servoNumber = _partListToTypeRelation[i];
          _outputList[i].Value = commandValue.toInt();
      }
      else if(_outputList[i].Type == TYPE_DOUT)
      {
          int partNumber = _partListToTypeRelation[i];
          digitalWrite(_outputList[partNumber].PinNumber, commandValue.toInt());
      }
      else if(_outputList[i].Type == TYPE_AOUT)
      {
          int partNumber = _partListToTypeRelation[i];
          analogWrite(_outputList[partNumber].PinNumber, commandValue.toInt());
      }
    }
  }
  
}

//Loop Event Handler
void RoboDuino::doLoopEvent()
{
  for(int i = 0; i < NUM_INPUTS; i++)
  {
    if ( _inputList[i].Type == TYPE_BUTTON_INC || _inputList[i].Type == TYPE_BUTTON_DEC)
    {
      if (digitalRead(_inputList[i].PinNumber) == LOW) {
        if (_inputList[i].Type == TYPE_BUTTON_INC) 
        {
          _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value + 1;            
        }
        if (_inputList[i].Type == TYPE_BUTTON_DEC) 
        {
          _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value - 1;            
        }
      }
    }
  }
  for(int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (_outputList[i].Type == TYPE_SERVO)
    {
        int servoNumber = _partListToTypeRelation[i];
        int currentValue = _servos[servoNumber].read();
        if (currentValue < _outputList[i].Value) 
        {
          _servos[servoNumber].write(currentValue + 1);
          delay(SERVO_DELAY);
        } 
        else if (currentValue > _outputList[i].Value) 
        {
          _servos[servoNumber].write(currentValue - 1);    
          delay(SERVO_DELAY);
        }
    }
  }
}
