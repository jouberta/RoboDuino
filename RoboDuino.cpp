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
        _inputList[i].Type == TYPE_TOGGLE || 
        _inputList[i].Type == TYPE_BUTTON_INC || 
        _inputList[i].Type == TYPE_BUTTON_DEC 
    ) 
    {
      // Set the pinmode for the inputs
      if (_inputList[i].Pullup == true) 
      {
        pinMode(_inputList[i].PinNumber, INPUT_PULLUP);        
      } else {
        pinMode(_inputList[i].PinNumber, INPUT);        
      }
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
  // If the command id is ? then do queries!
  if (cmdID == "?")
  {
    // Loop through outputs and send the details
    for(int i = 0; i < NUM_OUTPUTS; i++)
    {
      // If we are quering all or just this output
      if (commandValue == "" || _outputList[i].CommandIdentifier == commandValue) {
        if(_outputList[i].Type == TYPE_SERVO)
        {
          Serial.print("SERVO,");          
        }
        else if(_outputList[i].Type == TYPE_DOUT)
        {
          Serial.print("DIGITAL_OUT,");     
        }
        else if(_outputList[i].Type == TYPE_AOUT)
        {
          Serial.print("ANALOGUE_OUT,");     
        }
        else if(_outputList[i].Type == TYPE_MOTOR)
        {
          Serial.print("MOTOR,");     
        }
        // Output identifier
        Serial.print(_outputList[i].CommandIdentifier);
        Serial.print(",");
        // Output Value
        Serial.print(_outputList[i].Value);
        Serial.println();
      }
    }
    // Loop through inputs and send the details
    for(int i = 0; i < NUM_INPUTS; i++)
    {
      // If we are quering all or just this input
      if (commandValue == "" || _inputList[i].CommandIdentifier == commandValue) {
        // Output type
        if(_inputList[i].Type == TYPE_BUTTON_INC)
        {
          Serial.print("INCREMENT,");          
        }
        else if(_inputList[i].Type == TYPE_BUTTON_DEC)
        {
          Serial.print("OUTPUT,");     
        }
        else if(_inputList[i].Type == TYPE_BUTTON)
        {
          Serial.print("STANDARD,");
        }
        // Output identifier
        Serial.print(_inputList[i].CommandIdentifier);
        Serial.println();
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
    // Has the INPUT_DELAY past since last run, or if millis has looped round
    if (now >= _lastRead[i] + INPUT_DELAY || now <= _lastRead[i] - INPUT_DELAY - 5)
    {
      bool currentState = false;
      if (digitalRead(_inputList[i].PinNumber) == LOW) 
      {
        if (_inputList[i].Pullup == true)
        {
          currentState = true;
        } else {
          currentState = false;
        }
      } else if (digitalRead(_inputList[i].PinNumber) == HIGH)
      {
        if (_inputList[i].Pullup == true)
        {
          currentState = false;
        } else {
          currentState = true;
        }      
      }
      _doInput(i, currentState);
      // Set the lastRead value to now
      _lastRead[i] = now;
    }
  }
  // Run outputs
  for(int i = 0; i < NUM_OUTPUTS; i++)
  {
    //Servo output
    if (_outputList[i].Type == TYPE_SERVO)
    {
      if (now >= _lastRun[i] + SERVO_DELAY || now <= _lastRun[i] - SERVO_DELAY - 5) 
      {
        // Get the servo position in internal list
        int servoNumber = _partListToTypeRelation[i];
        // Get current value for servo
        int currentValue = _servos[servoNumber].read();
        // Check out SERVO_DELAY has passed or if millis has looped round
        if (currentValue < _outputList[i].Value) 
        {
          _servos[servoNumber].write(currentValue + 1);
        } 
        else if (currentValue > _outputList[i].Value) 
        {
          _servos[servoNumber].write(currentValue - 1);
        }
        // Set the last run to now
        _lastRun[i] = now;
      }
    }
    // Motor output
    if (_outputList[i].Type == TYPE_MOTOR)
    {
      // Check changed flag
      if (_outputList[i].ValueChange == true)
      {        
        // Check out MOTOR_DELAY has passed or if millis has looped round
        if (now >= _lastRun[i] + MOTOR_DELAY || now <= _lastRead[i] - MOTOR_DELAY - 5) 
        {
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
          // Reset changed flag
          _outputList[i].ValueChange = false;
          // Set the last run to now
          _lastRun[i] = now;
        }
      }
    }
  }
}

void RoboDuino::_doInput(int i, bool currentState)
{
  // Increment input
  if (_inputList[i].Type == TYPE_BUTTON_INC) 
  {
    if (currentState == true)
    {
      // Simple bound check (for servos atm)
      if (_outputList[_inputList[i].OutputNumber].Value <= 180) {
        // Change the objects value by increasing it one and set its changed value flag
        _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value + 1;
        _outputList[_inputList[i].OutputNumber].ValueChange = true;
      }
      Serial.println(_inputList[i].CommandIdentifier + ",on");
      _inputStates[i] = true;
    } else {
      if (_inputStates[i] == true) 
      {
        Serial.println(_inputList[i].CommandIdentifier + ",off");
        _inputStates[i] = false;
      }   
    }
  }
  // Decrement input
  if (_inputList[i].Type == TYPE_BUTTON_DEC) 
  {
    if (currentState == true)
    {
      // Simple bound check (for servos atm)
      if (_outputList[_inputList[i].OutputNumber].Value >= 0) {
        // Change the objects value by decreasing it one and set its changed value flag
        _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value - 1;    
        _outputList[_inputList[i].OutputNumber].ValueChange = true;        
      }
      Serial.println(_inputList[i].CommandIdentifier + ",on");
      _inputStates[i] = true;
    } else {
      if (_inputStates[i] == true) 
      {
        Serial.println(_inputList[i].CommandIdentifier + ",off");
        _inputStates[i] = false;
      }   
    }
  }
  // Standard Button
  if (_inputList[i].Type == TYPE_BUTTON) 
  {
    if (currentState == true) 
    {
      if (_inputStates[i] == false) 
      {
        Serial.println(_inputList[i].CommandIdentifier + ",on");
      }   
      _inputStates[i] = true;
    } else {
      if (_inputStates[i] == true) 
      {
        Serial.println(_inputList[i].CommandIdentifier + ",off");
      }   
      _inputStates[i] = false;
    }      
  }
}
