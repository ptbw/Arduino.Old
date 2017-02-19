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
  int lcm;
  //look(80);
  lcm = getDistance();
  if( lcm < 20 )
  {
        look(-90);
        delay(500);
        look(0);       
  }     
  delay(100);
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
 long microsecondsToCentimeters(long microseconds)
{
  return microseconds / cmconv;
}

