#include <Wire.h>


// define global variables here

int lspeed=0;
int rspeed=0;
int svpos[8]={1500,1500,1500,1500,1500,1500,1500,1500};                 // defines the center  position for each servo in uS

int movement;
int mdir=5;


//======================================================================== Initialize I/O pins, libraries and servos ==========================================
void setup()
{
  Wire.begin();
}

void loop()
{ 
  delay(50);
  I2Ccommand();
  movement+=mdir;
  if(movement>250 || movement<-250) mdir=-mdir;
  lspeed=movement;
  rspeed=movement;
  for(byte i=0;i<8;i++)
  {
    svpos[i]=movement*2+1500;
  }
}

//======================================================================== Send I2C Data ========================================================================
void I2Ccommand()
{
  Wire.beginTransmission(1);                                            // initialize communications with slave device #1 - Mini Driver motor and servo controller
  Wire.write(highByte(lspeed));                                         // transmit left  motor speed high byte first
  Wire.write( lowByte(lspeed));                                         // transmit left  motor speed low  byte
  Wire.write(highByte(rspeed));                                         // transmit right motor speed high byte first
  Wire.write( lowByte(rspeed));                                         // transmit right motor speed low  byte

  for(byte i=0;i<8;i++)                                                 // transmit servo positions for servos 0-7
  {
    Wire.write(highByte(svpos[i]));                                     // transmit servo position high byte first
    Wire.write( lowByte(svpos[i]));                                     // transmit servo position low  byte
  }
  
  Wire.endTransmission();                                               // end communications with slave device
}
  
  
  
