#include <Servo.h>

Servo left;
Servo right;

static int leftCentre = 75;
static int rightCentre = 75;
static int centreZone = 50;
static int leftPin = 10;
static int rightPin = 9;

static int pingPin = A1; // Analog pin 1 used for the Ping Pin (Trig)
static int inPin = A2; // Analog pin 2 used for the Echo Pin (Echo)

static int delta = 10;

unsigned long duration, inches;
int indec, cmdec;
int inchconv = 147; // ratio between puls width and inches
int cmconv = 59; // ratio between pulse width and cm

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  
  Serial.print("Basebot says hello!\n");
  pinMode(pingPin, OUTPUT);
  pinMode(inPin, INPUT);
  left.attach(leftPin);
  right.attach(rightPin);
}

void loop() {
  // put your main code here, to run repeatedly:
  int cm;
  // Start moving forward
  forward();
  // Get the distance in centimetres
  cm = getDistance();
  // At less than 15 run away, turn left then start forward again.
  if(cm < 15)
  {
    reverse();
    delay(1500);
    turnLeft();
    delay(900);
    forward();
  }
  delay(10);
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / cmconv;
}

int getDistance()
{
  int rval;
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  duration = pulseIn(inPin, HIGH);  
  rval = microsecondsToCentimeters(duration);
  if (rval == 0)
    rval = 1000;
  //Serial.println(rval);
  return rval;
}

void forward() {
  //Serial.println("Forwards");
  right.write(180);
  left.write(0);
}
void reverse() {
  //Serial.println("Run away!");
  right.write(0);
  left.write(180);
}
void turnLeft() {
  //Serial.println("Left!");
  left.write(90);
  right.write(180);
}
void turnRight() {
  //Serial.println("Right!");
  right.write(180);
  left.write(90);
}
void stop() {
  //Serial.println("Stop");
  right.write(90);
  left.write(90);
}
