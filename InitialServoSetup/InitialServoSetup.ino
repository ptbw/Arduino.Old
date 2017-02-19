/*
Copyright (c) 2014 Phil Willis
All rights reserved.

Based on Mini Driver Firmware:
Copyright (c) 2014, Dawn Robotics Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following discl0aimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the Dawn Robotics Ltd nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <Servo.h>			// include Servo library

const uint8_t VERSION_MAJOR = 0;
const uint8_t VERSION_MINOR = 1;
const uint16_t FIRMWARE_ID = 0x5058;    // Firmware - PW

Servo tiltServo;
Servo panServo;

const int MAXUP = 5;
const int MID = 54;
const int MAXDOWN = 70;

const int MAXLEFT = 30;
const int CENTRE = 96;
const int MAXRIGHT = 150;

uint8_t PanServoAngle = CENTRE;
uint8_t TiltServoAngle = MID;


const uint16_t MAX_MSG_SIZE = 32;
const uint16_t MSG_START_BYTES = 0xFFFF;
const uint16_t MSG_ID_POS = 2;
const uint16_t MSG_SIZE_POS = 3;
const uint16_t MSG_HEADER_SIZE = 2 + 1 + 1; // Start bytes + Id + size

const uint8_t COMMAND_ID_GET_FIRMWARE_INFO = 1;
const uint8_t COMMAND_ID_SET_OUTPUTS = 2;

const uint8_t RESPONSE_ID_FIRMWARE_INFO = 1;
const uint8_t RESPONSE_ID_INVALID_COMMAND = 2;
const uint8_t RESPONSE_ID_INVALID_CHECK_SUM = 3;
const uint8_t RESPONSE_ID_BATTERY_READING = 4;    // Not used any more
const uint8_t RESPONSE_ID_SENSOR_READINGS = 5;

const int LEFT_MOTOR_DIR_PIN = 7;
const int LEFT_MOTOR_PWM_PIN = 9;
const int RIGHT_MOTOR_DIR_PIN = 8;
const int RIGHT_MOTOR_PWM_PIN = 10;
const uint32_t NUM_TICKS_PER_MOTOR_WAVE = 100;
const unsigned long MOTOR_COMMAND_TIMEOUT_MS = 2000;

const int TILT_SERVO = 6;
const int PAN_SERVO = 11;
const int ULTRA_SONIC_SENSOR_PIN = 12;
const int BATTERY_VOLTAGE_PIN = A7;

const int LINE_LEFT_PIN = 5;
const int LINE_RIGHT_PIN = 4;

const int ULTRASONIC_READ_INTERVAL = 500;
const int NO_ULTRASONIC_SENSOR = 99999;
const uint32_t MICROSECONDS_PER_CM = 58;
const uint32_t MAX_RANGE = 400;

int CurMotorWaveTick = 0;

int led = 13;

int distance = 99999;                // cm
unsigned long lastReadUltraSonic = 0;
unsigned long pulseStartTime = 0;        // ms
bool isReading = false;

int SENSOR_READ_INTERVAL = 500;
unsigned long lastReadSensors = 0;
unsigned long lastCommandTime = 0;


//------------------------------------------------------------------------------
enum MotorDirectionEnum
{
  Forwards,
  Backwards
};

enum MessageStateEnum
{
  WaitingForMessage,
  ReceivingMessage
};

MessageStateEnum MessageState = WaitingForMessage;

uint8_t MsgBuffer[ MAX_MSG_SIZE ];
uint16_t NumMsgBytesReceived = 0;

uint8_t getMessageId() {
  return MsgBuffer[ MSG_ID_POS ];
}
uint8_t getMessageSize() {
  return MsgBuffer[ MSG_SIZE_POS ];
}

uint8_t LeftMotorDutyCycle = 0;
uint8_t RightMotorDutyCycle = 0;
MotorDirectionEnum LeftMotorDirection = Forwards;
MotorDirectionEnum RightMotorDirection = Forwards;

void receiveMessages()
{
  bool isMessageReceived = false;
  int numBytesAvailable = Serial.available();

  // While there are bytes available and we have not received a complete message continue to extract them from the serial buffer
  while ( numBytesAvailable > 0 && !isMessageReceived )
  {
    switch ( MessageState )
    {
      case WaitingForMessage:
        {
          int numBytesToRead = max( min( numBytesAvailable, MSG_HEADER_SIZE - NumMsgBytesReceived ), 0 );
          int numBytesRead = Serial.readBytes( (char*)&MsgBuffer[ NumMsgBytesReceived ], numBytesToRead );
          NumMsgBytesReceived += numBytesRead;

          if ( NumMsgBytesReceived == MSG_HEADER_SIZE )
          {
            if ( *((uint16_t*)MsgBuffer) == MSG_START_BYTES )
            {
              // We have a message header
              MessageState = ReceivingMessage;
            }
            else
            {
              // Discard the first byte as it is not part of a message
              MsgBuffer[ 0 ] = MsgBuffer[ 1 ];
              MsgBuffer[ 1 ] = MsgBuffer[ 2 ];
              MsgBuffer[ 2 ] = MsgBuffer[ 3 ];
              NumMsgBytesReceived = 3;
            }
          }

          break;
        }
      case ReceivingMessage:
        {
          int numBytesToRead = max( min( numBytesAvailable, getMessageSize() - NumMsgBytesReceived ), 0 );
          int numBytesRead = Serial.readBytes( (char*)&MsgBuffer[ NumMsgBytesReceived ], numBytesToRead );
          NumMsgBytesReceived += numBytesRead;

          if ( getMessageSize() == NumMsgBytesReceived )
          {
            processMessage();
            isMessageReceived = true;

            // Prepare for next message
            NumMsgBytesReceived = 0;
            MessageState = WaitingForMessage;
          }

          break;
        }
      default:
        {
          // We should never get here, but just in case, return to eMS_WaitingForMessage
          NumMsgBytesReceived = 0;
          MessageState = WaitingForMessage;
        }
    }
    numBytesAvailable = Serial.available();
  }
}


void processMessage()
{
  // Check the checksum of the message
  uint8_t calculatedCheckSum = calculateCheckSum( MsgBuffer, getMessageSize() );

  if ( calculatedCheckSum != MsgBuffer[ getMessageSize() - 1 ] )
  {
    sendInvalidCheckSumResponse();
    return;
  } return; return;

  // Handle the command Id
  bool isCommandHandled = false;
  switch ( getMessageId() )
  {
    case COMMAND_ID_GET_FIRMWARE_INFO:
      {
        sendFirmwareInfoResponse();
        isCommandHandled = true;
        break;
      }
    case COMMAND_ID_SET_OUTPUTS:
      {
        if ( getMessageSize() == 9 )
        {
          int leftMotorSpeed = (int)MsgBuffer[ 4 ] - 128;
          int rightMotorSpeed = (int)MsgBuffer[ 5 ] - 128;
          PanServoAngle = constrain( MsgBuffer[ 6 ], MAXLEFT, MAXRIGHT );
          TiltServoAngle = constrain( MsgBuffer[ 7 ], MAXUP, MAXDOWN );

          LeftMotorDirection = ( leftMotorSpeed >= 0 ? Forwards : Backwards );
          RightMotorDirection = ( rightMotorSpeed >= 0 ? Forwards : Backwards );
          LeftMotorDutyCycle = constrain( abs( leftMotorSpeed ), 0, 100 );
          RightMotorDutyCycle = constrain( abs( rightMotorSpeed ), 0, 100 );
          lastCommandTime = millis();

          isCommandHandled = true;
        }
        break;
      }
  }
  if ( !isCommandHandled )
  {
    sendInvalidCommandResponse();
    digitalWrite(led, LOW);
  }
}

void sendData( uint8_t* msgData, uint8_t msgSize )
{
  const uint8_t MSG_SIZE = msgSize;
  msgData[ 3 ] = MSG_SIZE;
  msgData[ MSG_SIZE - 1 ] = calculateCheckSum( msgData, MSG_SIZE );

  digitalWrite(led, HIGH);
  Serial.write( msgData, MSG_SIZE );
  digitalWrite(led, LOW);
}

void sendMessage( int batteryReading, uint8_t digitalReadings, int analogReadings[ 6 ], int lastDistance, int32_t leftEncoderReading, int32_t rightEncoderReading )
{
  uint8_t msgData[] =
  {
    0xFF, 0xFF, RESPONSE_ID_SENSOR_READINGS, 0, // Header
    0, 0,
    (batteryReading >> 8) & 0xFF, batteryReading & 0xFF,
    digitalReadings,
//    (analogReadings[ 0 ] >> 8) & 0xFF, analogReadings[ 0 ] & 0xFF,
//    (analogReadings[ 1 ] >> 8) & 0xFF, analogReadings[ 1 ] & 0xFF,
//    (analogReadings[ 2 ] >> 8) & 0xFF, analogReadings[ 2 ] & 0xFF,
//    (analogReadings[ 3 ] >> 8) & 0xFF, analogReadings[ 3 ] & 0xFF,
//    (analogReadings[ 4 ] >> 8) & 0xFF, analogReadings[ 4 ] & 0xFF,
//    (analogReadings[ 5 ] >> 8) & 0xFF, analogReadings[ 5 ] & 0xFF,
    (lastDistance >> 8) & 0xFF, lastDistance & 0xFF,
    (leftEncoderReading >> 24) & 0xFF, (leftEncoderReading >> 16) & 0xFF, (leftEncoderReading >> 8) & 0xFF, leftEncoderReading & 0xFF,
    (rightEncoderReading >> 24) & 0xFF, (rightEncoderReading >> 16) & 0xFF, (rightEncoderReading >> 8) & 0xFF, rightEncoderReading & 0xFF,
    0
  };

  sendData( msgData, sizeof(msgData) );

}

void sendFirmwareInfoResponse()
{
  uint8_t msgData[] =
  {
    0xFF, 0xFF, RESPONSE_ID_FIRMWARE_INFO, 0,   // Header
    FIRMWARE_ID >> 8,  FIRMWARE_ID & 0xFF,
    VERSION_MAJOR, VERSION_MINOR, 0
  };

  sendData( msgData, sizeof(msgData) );
}

//------------------------------------------------------------------------------
void sendInvalidCommandResponse()
{
  uint8_t msgData[] =
  {
    0xFF, 0xFF, RESPONSE_ID_INVALID_COMMAND, 0, 0
  };

  sendData( msgData, sizeof(msgData) );
}

//------------------------------------------------------------------------------
void sendInvalidCheckSumResponse()
{
  uint8_t msgData[] =
  {
    0xFF, 0xFF, RESPONSE_ID_INVALID_CHECK_SUM, 0, 0
  };

  sendData( msgData, sizeof(msgData) );
}

//------------------------------------------------------------------------------
uint8_t calculateCheckSum( const uint8_t* pData, uint8_t msgSize )
{
  uint32_t sum = 0;

  // Use all of the data apart from the message start bytes and the byte
  // that will store the checksum
  for ( uint8_t i = 2; i < msgSize - 1; i++ )
  {
    sum += pData[ i ];
  }

  return (uint8_t)(~sum);
}

//------------------------------------------------------------------------------
ISR( TIMER2_COMP_vect )        // Interrupt service routine for motor PWM
{
  CurMotorWaveTick++;

  digitalWrite( LEFT_MOTOR_PWM_PIN, ( CurMotorWaveTick > LeftMotorDutyCycle ? LOW : HIGH ) );
  digitalWrite( RIGHT_MOTOR_PWM_PIN, ( CurMotorWaveTick > RightMotorDutyCycle ? LOW : HIGH ) );

  if ( CurMotorWaveTick >= 100 )
  {
    CurMotorWaveTick = 0;
  }
}

void setup()
{

  // Set up timer 2 in CTC mode with a prescaler of clk/32, so with the chip
  // running at 16MHz this gives 500,000 clock ticks per second. By varying
  // the value of OCR2 we will generate an interrupt from roughly 8000 times a
  // second to 2000 times a second which gives a motor PWM frequency of 80Hz
  // to 20Hz. Lower PWM frequencies give choppier movement
  noInterrupts();
  TCCR2 = (1 << 3) | 3;      // Activate timer in CTC mode with a prescaler of clk/32
  OCR2 = 0xFF;
  TIMSK |= (1 << OCIE2);     // Activate timer 2 output compare interrupt
  interrupts();

  tiltServo.attach(TILT_SERVO);		// servo control pin at D6
  panServo.attach(PAN_SERVO);		// servo control pin at D11
  pinMode(led, OUTPUT);
  pinMode(ULTRA_SONIC_SENSOR_PIN, INPUT);

  tiltServo.write(MID);
  panServo.write(CENTRE);
  digitalWrite(led, LOW);

  pinMode( LEFT_MOTOR_DIR_PIN, OUTPUT );
  pinMode( LEFT_MOTOR_PWM_PIN, OUTPUT );
  pinMode( RIGHT_MOTOR_DIR_PIN, OUTPUT );
  pinMode( RIGHT_MOTOR_PWM_PIN, OUTPUT );

  // Make all sensor pins inputs
 
  pinMode( LINE_LEFT_PIN, INPUT );
  pinMode( LINE_RIGHT_PIN, INPUT );
  
  pinMode( A0, INPUT );
  pinMode( A1, INPUT );
  pinMode( A2, INPUT );
  pinMode( A3, INPUT );
  pinMode( A4, INPUT );
  pinMode( A5, INPUT );
  Serial.begin(57600,SERIAL_8N1);
}

void loop()
{

  // Read any commands from the serial connection
  receiveMessages();

  unsigned long curTime = millis();

  // Update the motor directions
  digitalWrite( LEFT_MOTOR_DIR_PIN, ( Forwards == LeftMotorDirection ? HIGH : LOW ) );
  digitalWrite( RIGHT_MOTOR_DIR_PIN, ( Forwards == RightMotorDirection ? HIGH : LOW ) );

  //LeftEncoder.setGoingForward( Forwards == LeftMotorDirection );
  //RightEncoder.setGoingForward( Forwards == RightMotorDirection );

  // Update the servo angles
  panServo.write( PanServoAngle );
  tiltServo.write( TiltServoAngle );

  // Periodically kick off an ultrasonic sensor read
  if ( curTime - lastReadUltraSonic >= ULTRASONIC_READ_INTERVAL && !isReading )
  {
    // Start to measure the range
    pinMode( ULTRA_SONIC_SENSOR_PIN, OUTPUT );
    digitalWrite( ULTRA_SONIC_SENSOR_PIN, LOW );
    delayMicroseconds( 2 );
    digitalWrite( ULTRA_SONIC_SENSOR_PIN, HIGH );
    delayMicroseconds( 5 );
    digitalWrite( ULTRA_SONIC_SENSOR_PIN, LOW );

    unsigned long startWait = micros();

    // Wait for the response pulse to start
    pinMode( ULTRA_SONIC_SENSOR_PIN, INPUT );
    bool isPulseStarted = false;
    while ( !isPulseStarted && ((micros() - startWait) < ULTRASONIC_READ_INTERVAL) )
    {
      isPulseStarted = digitalRead( ULTRA_SONIC_SENSOR_PIN ) == HIGH;
    }

    if ( isPulseStarted )
    {
      pulseStartTime = micros();
      isReading = true;
    }
    else
    {
      // Looks like no sensor is attached. Wait a bit before trying again
      distance = NO_ULTRASONIC_SENSOR;
      lastReadUltraSonic = curTime;
    }
  } 

  // Check the status of an ongoing ultrasonic read
  if ( isReading && digitalRead( ULTRA_SONIC_SENSOR_PIN ) == LOW )
  {
    // Pulse has ended
    unsigned long duration = micros() - pulseStartTime;

    distance = duration / MICROSECONDS_PER_CM;
    if ( distance > MAX_RANGE )
    {
      distance = MAX_RANGE;
    }

    lastReadUltraSonic = curTime;
    isReading = false;
  }

  // Periodically read from the sensors and send the readings back
  if ( curTime - lastReadSensors >= SENSOR_READ_INTERVAL )
  {
    // Read in the battery voltage
    int batteryReading = analogRead( BATTERY_VOLTAGE_PIN );
    


    uint8_t digitalReadings = 0;
    int analogReadings[ 6 ] = { 0 };
        
    digitalReadings |= (( digitalRead( LINE_LEFT_PIN ) == HIGH ? 1 : 0 ) << 0);
    digitalReadings |= (( digitalRead( LINE_RIGHT_PIN ) == HIGH ? 1 : 0 ) << 1);

    if( false )
    {
        Serial.print("Battery: ");
        Serial.println(batteryReading);
        Serial.print("Digital Left: ");
        Serial.println((digitalRead( LINE_LEFT_PIN ) == HIGH ? 1 : 0 ));
        Serial.print("Digital Right: ");
        Serial.println((digitalRead( LINE_RIGHT_PIN ) == HIGH ? 1 : 0 ));
    }
    else    
      sendMessage( batteryReading, digitalReadings, analogReadings, distance, 0, 0 );

    lastReadSensors = curTime;
  }
}
