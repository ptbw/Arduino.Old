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
int i = 0;

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
   forward(250);   
   delay(1000);
   halt();
   reverse(250);
   delay(1000);
   halt();
}

void forward (int spd)
{
  Serial.println(1);
  motor.run(FORWARD);
  motor.setSpeed(spd);  
}
 
void reverse(int spd)
{
  Serial.println(2);
  motor.run(BACKWARD);
  motor.setSpeed(spd);  
 }
 
 void halt()
{
  motor.run(RELEASE);
  delay(10);
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
