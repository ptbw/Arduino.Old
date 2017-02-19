/*
WiiClassic Test Code

This code prints the current controller status to the serial port.
Button pressed calls poll whether the button was pressed since the last update call.
as a result, it will just briefly print the last button pressed once.

Tim Hirzel May 2008

*/

#include <SoftwareSerial.h>
#define rxPin 2
#define txPin 3
SoftwareSerial blueToothSerial(rxPin, txPin);

#include "Wire.h"

#include "WiiClassic.h"


WiiClassic wiiClassy = WiiClassic();

void setup() {
  setpowerpins();  
  Wire.begin();
  Serial.begin(9600);
  setupBlueToothConnection(); 
  
  wiiClassy.begin();
  wiiClassy.update();
}

void setupBlueToothConnection()
{
  blueToothSerial.begin(9600);
  blueToothSerial.print("\r\nAT+VERSION?\r\n");
  delay(2000); // This delay is required.
  Serial.println("2 seconds passed");
  blueToothSerial.flush();
}

static void setpowerpins()
{
#define pwrpin PORTC3
#define gndpin PORTC2
    DDRC |= _BV(pwrpin) | _BV(gndpin);
    PORTC &=~ _BV(gndpin);
    PORTC |=  _BV(pwrpin);
    delay(100);  // wait for things to stabilize        
}

void loop() {
  delay(1); // 1ms is enough to not overload the wii Classic, 100ms seems to ease the serial terminal a little
  wiiClassy.update();

  // 
  /* 
  // PRINT RAW BYTES  FOR DEBUG
   // STATUS IS FIRST FOUR BYTES
   for (int i = 0; i < 4; i ++) {
   for (int j = 7; j >= 0; j--) {
   if ((wiiClassy.getRawStatus()[i] & (1 << j)) == 0) {
   Serial.print("0");
   }
   else{
   Serial.print("1");
   }
   }
   Serial.println();
   }
   // BUTTONS IS NEXT TWO BYTES
   for (int i = 0; i < 2; i ++) {
   for (int j = 7; j >= 0; j--) {
   if ((wiiClassy.getRawButtons()[i] & (1 << j)) == 0) {
   Serial.print("0");
   }
   else{
   Serial.print("1");
   }
   }
   Serial.println();
   }
  Serial.println("---");
   */

  // Serial.print("Buttons:");


  if (wiiClassy.leftShoulderPressed()) {
    Serial.println("LS.");
  }
  if (wiiClassy.rightShoulderPressed()) {
    Serial.println("RS.");
  }
  if (wiiClassy.lzPressed()) {
    Serial.println("lz.");
  }
  if (wiiClassy.rzPressed()) {
    Serial.println("rz.");
  }
  if (wiiClassy.leftDPressed()) {
    Serial.println("LD.");
  }

  if (wiiClassy.rightDPressed()) {
    Serial.println("RD.");
  }

  if (wiiClassy.upDPressed()) {
    Serial.println("UD.");
  }
  if (wiiClassy.downDPressed()) {
    Serial.println("DD.");
  }

  if (wiiClassy.selectPressed()) {
    Serial.println("select.");
  }

  if (wiiClassy.homePressed()) {
    Serial.println("home.");
  }
  if (wiiClassy.startPressed()) {
    Serial.println("start.");
  }

  if (wiiClassy.xPressed()) {
    Serial.println("x.");
  }

  if (wiiClassy.yPressed()) {
    Serial.println("y.");
  }

  if (wiiClassy.aPressed()) {
    Serial.println("a.");
  }

  if (wiiClassy.bPressed()) {
    Serial.println("b.");
  }

  if (wiiClassy.leftShoulderPressed()) {
    Serial.println();
    Serial.print("right shoulder: ");
    Serial.println(wiiClassy.rightShouldPressure());
    Serial.print(" left shoulder: ");
  
    Serial.println(wiiClassy.leftShouldPressure());
    Serial.print("  left stick x: ");
  
    Serial.println(wiiClassy.leftStickX());
    Serial.print("  left stick y: ");
  
    Serial.println(wiiClassy.leftStickY());
    Serial.print(" right stick x: ");
  
    Serial.println(wiiClassy.rightStickX());
    Serial.print(" right stick y: ");
  
    Serial.println(wiiClassy.rightStickY());
    Serial.println("---");
  }   
  blueToothSerial.print(wiiClassy.leftStickX());
  blueToothSerial.print("\t");
  blueToothSerial.print(wiiClassy.leftStickY());
  blueToothSerial.print("\t");  
  blueToothSerial.print(wiiClassy.rightStickX());
  blueToothSerial.print("\t");  
  blueToothSerial.print(wiiClassy.rightStickY());
  blueToothSerial.print("\t");  
  blueToothSerial.print(wiiClassy.homePressed());
  blueToothSerial.print("\n\r");

  //pulse = (int)500+8*wiiClassy.leftStickX();
  //updateServo();
}
