// iwm0003 chassis
// Uses an HC-SR04 Ultrasonic Sensor on the front steered axle
// The motors are controlled using the Adafruit Motor shield library
// which must be downloaded and installed first
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>
#include <Servo.h>


AF_DCMotor motor(1, MOTOR12_64KHZ); // create motor 

int lookPin = 10; // distace sensor servo servo 1 on motor shield
int steerPin = 9;  // rear steering servo servo 2 on sensor shield
Servo lookServo;
Servo steerServo;

int pingPin = A1; // Analog pin 1 used for the Ping Pin (Trig)
int inPin = A2; // Analog pin 2 used for the Echo Pin (Echo)
unsigned long duration, inches;
int indec, cmdec;
int inchconv = 147; // ratio between puls width and inches
int cmconv = 59; // ratio between pulse width and cm
String s1, s2, s3;
int lookCentre = 80; // centre setting adjust figure or adjust servo horn
int steerCentre = 90;  // centre setting adjust figure or adjust servo horn

void setup()
{
  Serial.begin(9600);
  pinMode(pingPin, OUTPUT);
  pinMode(inPin, INPUT);
  lookServo.attach(lookPin,1000,2000);
  look(0);
  steerServo.attach(steerPin, 1000, 2000);
  steer(0);
}

void loop()
{
  int cm, lcm, rcm;
  forward(250);
  delay(500);
  cm = getDistance();
  if(cm < 15)
  {
    halt();
    look(-80);
    lcm = getDistance();
    look(80);
    rcm = getDistance();
    look(0);
    if (lcm < rcm)
      steer(-80);
    else
      steer(80);
    reverse(250);
    delay(1500);
    steer(0);
  }
}

void steer(int angle)
{
  steerServo.write(steerCentre + angle);
  delay(800); // wait for servo to get there
}
void look (int angle)
{
  lookServo.write(lookCentre + angle);
  delay (800); //wait for servo to get there
}
int getDistance()
{
  int rval;
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  duration = pulseIn(inPin, HIGH, 38000L);  // Set timeout to 38mS
  if (duration == 0)
    duration = 1000;
  rval = microsecondsToCentimeters(duration);
  Serial.println(rval);
  return rval;
}

void forward (int spd)
{
  motor.run(FORWARD);
  motor.setSpeed(spd);  
}
 
void reverse(int spd)
{
  motor.run(BACKWARD);
  motor.setSpeed(spd);  
 }
 
void spinLeft(int spd)
{
 }
 
void spinRight(int spd)
{
}
 
void halt()
{
  motor.run(RELEASE);
  delay(10);
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / cmconv;
}


