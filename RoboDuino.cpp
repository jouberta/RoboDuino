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
  // Set all the counters to zero
  _servoCounter = 0;
}

//setup function
void RoboDuino::init(Input inputList[NUM_INPUTS], Output outputList[NUM_OUTPUTS]) 
{    
  // FACTORY STUFFS FOR OUTPUTS
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    //set the parts list
    _outputList[i] = outputList[i];
    //Init the servos
    if (_outputList[i].Type == TYPE_SERVO) 
    {
      // Set the pin on the internal array of servos
      _servos[_servoCounter].attach(_outputList[i].PinNumber[PIN_SERVO]);
      // Set a reference for the internal servo object 
      // so can be referenced elsewhere.
      _partListToTypeRelation[i] = _servoCounter;
      _servoCounter++;
    } 
    else if(_outputList[i].Type== TYPE_AOUT || _outputList[i].Type== TYPE_DOUT)
    {
      int pinPosition = PIN_AOUT;
      if (_outputList[i].Type== TYPE_DOUT) {
        pinPosition = PIN_DOUT;
      }
      // Set the pinMode on the correct pin for the output
      pinMode(_outputList[i].PinNumber[pinPosition], OUTPUT);
      // Set a reference to the output
      _partListToTypeRelation[i] = i;
    }
    else if(_outputList[i].Type== TYPE_MOTOR) 
    {
      // Set up the pin modes for all 3 motor pins
      pinMode(_outputList[i].PinNumber[PIN_MOTOR_S1], OUTPUT);
      pinMode(_outputList[i].PinNumber[PIN_MOTOR_S1], OUTPUT);
      pinMode(_outputList[i].PinNumber[PIN_MOTOR_PWM], OUTPUT);
      _partListToTypeRelation[i] = i;
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
      // Set the pinmode for the inputs
      // TODO: Add pull up and standard input pins
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
  // Get the position of the command separator so can split the string
  // into command id and command value
  int seperatorPosition = _inputCommand.indexOf(":");
  int valueLength = _inputCommand.length() - (2 + seperatorPosition);

  // Split out the string to id and value
  String cmdID = _inputCommand.substring(0, seperatorPosition); // read the first letter of the command, this indicates the command type - M/motor S/servo X/switch Q/query
  String commandValue = _inputCommand.substring(seperatorPosition + 1, seperatorPosition + valueLength + 1);

  // Loop through outputs to do any changes if required
  for(int i = 0; i < NUM_OUTPUTS; i++)
  {
    // If the command id is for this output in the loop then do the command
    if (_outputList[i].CommandIdentifier == cmdID)
    {
      if (_outputList[i].Type == TYPE_SERVO)
      {
        // Servo output
        // Get the position of the servo in the internal list
        int servoNumber = _partListToTypeRelation[i];
        // Set its value to the command value
        _outputList[i].Value = commandValue.toInt();
      }
      else if(_outputList[i].Type == TYPE_DOUT)
      {
        // Digital Output
        int partNumber = _partListToTypeRelation[i];
        digitalWrite(_outputList[partNumber].PinNumber[PIN_DOUT], commandValue.toInt());
      }
      else if(_outputList[i].Type == TYPE_AOUT)
      {
        // Analogue output
        int partNumber = _partListToTypeRelation[i];
        analogWrite(_outputList[partNumber].PinNumber[PIN_AOUT], commandValue.toInt());
      }
      else if(_outputList[i].Type == TYPE_MOTOR)
      { 
        // Motor output
        // Set the value for the motor
        _outputList[i].Value = commandValue.toInt();
        // Set the changed value flag for the motor 
        _outputList[i].ValueChange = true;
      }
    }
  }
  
}

//Loop Event Handler
void RoboDuino::doLoopEvent()
{
  // Get a timestamp to check delays against
  unsigned int now = millis();
  // Read inputs
  for(int i = 0; i < NUM_INPUTS; i++)
  {
    // Increment and Decrement inputs
    if ( _inputList[i].Type == TYPE_BUTTON_INC || _inputList[i].Type == TYPE_BUTTON_DEC)
    {
      // If the input has been set for a pullup input
      // TODO: Implement standard input too!
      if (digitalRead(_inputList[i].PinNumber) == LOW) {
        // Has the INPUT_DELAY past since last run, or if millis has looped round
        if (now >= _lastRead[i] + INPUT_DELAY || now <= _lastRead[i] - INPUT_DELAY - 5)
        {
          // Increment input
          if (_inputList[i].Type == TYPE_BUTTON_INC) 
          {
            // Simple bound check (for servos atm)
            if (_outputList[_inputList[i].OutputNumber].Value <= 180) {
              // Change the objects value by increasing it one and set its changed value flag
              _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value + 1;
              _outputList[_inputList[i].OutputNumber].ValueChange = true;
            }
          }
          // Decrement input
          if (_inputList[i].Type == TYPE_BUTTON_DEC) 
          {
            // Simple bound check (for servos atm)
            if (_outputList[_inputList[i].OutputNumber].Value >= 0) {
              // Change the objects value by decreasing it one and set its changed value flag
              _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value - 1;    
              _outputList[_inputList[i].OutputNumber].ValueChange = true;        
            }
          }
          // Set the lastRead value to now
          _lastRead[i] = now;
        }
      }
    }
  }
  // Run outputs
  for(int i = 0; i < NUM_OUTPUTS; i++)
  {
    //Servo output
    if (_outputList[i].Type == TYPE_SERVO)
    {
      // Get the servo position in internal list
      int servoNumber = _partListToTypeRelation[i];
      // Get current value for servo
      int currentValue = _servos[servoNumber].read();
      // Check out SERVO_DELAY has passed or if millis has looped round
      if (now >= _lastRun[i] + SERVO_DELAY || now <= _lastRead[i] - SERVO_DELAY - 5) 
      {
        // Set the last run to now
        _lastRun[i] = now;
        if (currentValue < _outputList[i].Value) 
        {
          _servos[servoNumber].write(currentValue + 1);
        } 
        else if (currentValue > _outputList[i].Value) 
        {
          _servos[servoNumber].write(currentValue - 1);
        }          
      }
    }
    // Motor output
    if (_outputList[i].Type == TYPE_MOTOR)
    {
      // Check changed flag
      if (_outputList[i].ValueChange == true)
      {
        // Reset changed flag
        _outputList[i].ValueChange = false;
        
        // Check out MOTOR_DELAY has passed or if millis has looped round
        if (now >= _lastRun[i] + MOTOR_DELAY || now <= _lastRead[i] - MOTOR_DELAY - 5) 
        {
          // Set the last run to now
          _lastRun[i] = now;
          if (_outputList[i].Value < 0) 
          {
            // Set 'forward' speed
            //SET PINS TO REVERSE AND SET PWM VALUE
            analogWrite(_outputList[i].PinNumber[PIN_MOTOR_PWM], (_outputList[i].Value * -1));
            digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S1], HIGH);
            digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S2], LOW);
              
          } 
          else if (_outputList[i].Value > 0) 
          {
            // Set 'reverse' speed
            analogWrite(_outputList[i].PinNumber[PIN_MOTOR_PWM], _outputList[i].Value);
            //SET PINS TO foward AND SET PWM VALUE
            digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S1], LOW);
            digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S2], HIGH);
          }  
          else if (_outputList[i].Value == 0) 
          {
            // Stop the motor
            analogWrite(_outputList[i].PinNumber[PIN_MOTOR_PWM], LOW);          
          }         
        }
      }
    }
  }
}
