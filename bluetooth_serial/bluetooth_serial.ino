/*********************************************************************
** Description:
**
** This file is a sample code for your reference.
**
*********************************************************************/
#include <SoftwareSerial.h>
//#include <TimerOne.h>
#define rxPin 2
#define txPin 3
SoftwareSerial mySerial(rxPin, txPin);

void Callback()
{
    Serial.println("-------> Callback Send AT");
    mySerial.print("AT\r\n");
}
void setup()
{
    // define pin modes for tx, rx pins:
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    mySerial.begin(9600);
    Serial.begin(9600);
    Serial.println("Wait 2secs");
    delay(2000);    
    Serial.println("Setup");
    mySerial.print("at\r\n");
    mySerial.print("at\r\n");
    mySerial.print("at+reset\r\n");
    mySerial.print("at+role=0\r\n");
    delay(2000);
    Serial.println("State?");
    mySerial.print("at+state?\r\n");
    while(mySerial.available())
        Serial.print((char)mySerial.read());
    Serial.println("");
//Timer1.initialize(2000000);
//Timer1.attachInterrupt(Callback);
// setting callback is 2s
}
void loop()
{
    int i = 0;
    char someChar[32] = {0};
    // when characters arrive over the serial port...
    if(Serial.available()) {
      do{
        someChar[i++] = Serial.read();
        //As data trickles in from your serial port you are grabbing as much as you can,
        //but then when it runs out (as it will after a few bytes because the processor
        //is much faster than a 9600 baud device) you exit loop, which then restarts,
        //and resets i to zero, and someChar to an empty array.So please be sure to keep this delay
        delay(3);
      }while (Serial.available() > 0);
      mySerial.println(someChar);
      Serial.println(someChar);
    }
    
    while(mySerial.available())
        Serial.print((char)mySerial.read());          
}
  
