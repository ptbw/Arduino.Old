/*
  BTSTemperatureBlink - Rich Janicki - 11/27/2012
  
 Based off of Bluetooth Slave Demo from Seeed Studio, and Adafruit's TMP36 Sensor Tutorial
  
 This demonstrates serial control over Bluetooth. I used an Android device and the app BlueTerm to send
 the commands to toggle a blinking light, or get the current temperature from the TMP36. 
  
 */
 
#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 2
#define TxD 3
#define ledPin 13 //pin we want to blink the led on
 
//temp sense
#define aref_voltage 3.3         // we tie 3.3V to ARef and measure it with a multimeter!
//TMP36 Pin Variables
int tempPin = 3;        //the analog pin the TMP36's Vout (sense) pin is connected to
boolean bLed = false;
float temperatureC =0;
float temperatureF = 0;
SoftwareSerial blueToothSerial(RxD,TxD);
 
void setup() 
{ 
  Serial.begin(9600);
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  pinMode(ledPin, OUTPUT);
  //analogReference(EXTERNAL);
  setupBlueToothConnection(); 
} 
 
void loop() 
{ 
  char recvChar;
 
  if(blueToothSerial.available()){//check if there's any data sent from the remote bluetooth shield
    recvChar = blueToothSerial.read();
    Serial.print(recvChar);
  }
  if(Serial.available()){//check if there's any data sent from the local serial terminal, you can add the other applications here
    recvChar  = Serial.read();
    blueToothSerial.print(recvChar);
  }
 
 
  switch(recvChar){
 
  case 'b':
    bLed = true;
    blueToothSerial.println("Blinking LED ON");
    break;
  case 'f':
    bLed = false;
    blueToothSerial.println("Blinking LED OFF");
    break;
//  case 't':
//    tempSense();
//    blueToothSerial.print(temperatureC); 
//    blueToothSerial.println(" tempC");
//    blueToothSerial.print(temperatureF); 
//    blueToothSerial.println(" tempF");
//    break;
  }
  if(bLed){
    blinkLed();
  }
 
} 
 
void setupBlueToothConnection()
{
  blueToothSerial.begin(9600); //Set Bluetooth Shield BaudRate to default baud rate 38400
  blueToothSerial.print("\r\nAT+VERSION?\r\n");
//  blueToothSerial.print("\r\n+AT+RESET\r\n");
//  blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
//  blueToothSerial.print("\r\n+STNA=SeeedBTSlave\r\n"); //set the bluetooth name as "SeeedBTSlave"
//  blueToothSerial.print("\r\n+STPIN=0000\r\n");//Set SLAVE pincode"0000"
//  blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  //blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  //blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  Serial.println("2 seconds passed");
  //char recstatus;
  //blueToothSerial.print("\r\n+RTSTA:XX\r\n");
  // recstatus =  blueToothSerial.read();
  //Serial.print(recstatus);
  blueToothSerial.flush();
}
 
void blinkLed(){
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);               // wait for a half second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(500);               // wait for a half second
}
void tempSense(){
 
  int tempReading = analogRead(tempPin);  
  Serial.print("Temp reading = ");
  Serial.print(tempReading);     // the raw analog reading
  // converting that reading to voltage, which is based off the reference voltage
  float voltage = tempReading * aref_voltage;
  voltage /= 1024.0; 
  // print out the voltage
  Serial.print(" - ");
  Serial.print(voltage); 
  Serial.println(" volts");
  // now print out the temperature
  temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((volatge - 500mV) times 100)
  Serial.print(temperatureC); 
  Serial.println(" degrees C");
  // now convert to Fahrenheight
  temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
 
  Serial.print(temperatureF); 
  Serial.println(" degrees F");
}
