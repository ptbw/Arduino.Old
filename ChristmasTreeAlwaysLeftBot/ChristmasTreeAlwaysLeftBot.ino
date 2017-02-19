#include <Servo.h> 

Servo left, right;
static int leftCentre = 75;
static int rightCentre = 75;
static int centreZone = 50;
static int leftPin = 10;
static int rightPin = 9;

static int pingPin = A1; // Analog pin 1 used for the Ping Pin (Trig)
static int inPin = A2; // Analog pin 2 used for the Echo Pin (Echo)

unsigned long duration, inches;
int indec, cmdec;
int inchconv = 147; // ratio between puls width and inches
int cmconv = 59; // ratio between pulse width and cm

static int delta = 10;

static int idealWidth = 150;
static int minWidth = 50;
static int widthHysterisis = 20;

int A = 2;
int B = 3;
int C = 4;
int D = 5;
double total_illumination_time = -1;

const double illumination_time_default = 0.001;

int L0 = 1;
int L1 = 2;
int L2 = 4;
int L3 = 8;
int L4 = 16;
int L5 = 32;
int L6 = 64;
int ALL = 1+2+4+8+16+32+64;
int NO_LEDS = 0;

bool started = false;
int led = 1;

void SingleLedOn(int n)
{
  int anode;
  int cathode;
  
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  pinMode(C, INPUT);
  pinMode(D, INPUT);
  
  if(n==1)
  {
    anode = C;
    cathode = A;
  }
  else if (n==2)
  {
    anode = C;
    cathode = D;
  }
  else if (n==4)
  {
    anode = D;
    cathode = C;
  }
  else if (n==8)
  {
    anode = D;
    cathode = B;
  }
  else if (n==16)
  {
    anode = B;
    cathode = D;
  }
  else if (n==32)
  { 
    anode = A;
    cathode = B;
  }
  else if (n==64)
 {
   anode = B;
   cathode = A;
  }
  else if (n==128)
  {
    anode = A;
    cathode = C;
  }
  else
    return;  //Invalid
  
  pinMode(anode, OUTPUT);
  pinMode(cathode, OUTPUT);
  
  digitalWrite(anode, HIGH);
  digitalWrite(cathode, LOW);
}

void LedsOnAndWait( int leds, double waitTime )
{
  for( int j = 0; j < int(waitTime/total_illumination_time); j++ )
  {
    for( int i = 0; i < 8; i++ )
    {
      SingleLedOn(leds & (1<<i));
      delay(illumination_time_default);
    }    
  }
}    

void turnOn( int theLed )
{
  SingleLedOn(0);
  
  switch(theLed)
  {        
    case 1:
      SingleLedOn(L0);
      break;
    case 2:      
      SingleLedOn(L1);
      break;
    case 3:
      SingleLedOn(L2);
      break;
    case 4:
      SingleLedOn(L3);
      break;
    case 5:
      SingleLedOn(L4);
      break;
    case 6:
      SingleLedOn(L5);
      break;
    case 7:
      SingleLedOn(L6);      
      break;
  }
}  

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Basebot says hello!\n");
  pinMode(pingPin, OUTPUT);
  pinMode(inPin, INPUT);
  left.attach(leftPin);
  right.attach(rightPin);
  
  total_illumination_time = 8 * illumination_time_default;
  
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  
  pinMode(7, INPUT);
  
  SingleLedOn(0);
  led = 1;
  allStop();
}

unsigned long previousMillis = 0;
const long interval = 250;

void loop() {
  unsigned long currentMillis = millis();
   
  int sensorValue = digitalRead(7);
  //Serial.println(sensorValue);
  if( sensorValue == 1 )
  {
   started = !started;
   delay(1000);
  }
  

  turnOn(led);
//  if(currentMillis - previousMillis >= interval) {
//    previousMillis = currentMillis;   
//    
//    if( started ) turnOn(led);
//    else 
//  }  
  
  if( started )
  {
      int cm, lcm, rcm;
      forward();
      cm = getDistance();
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
  else
    allStop();
  
  led++; 
  if( led > 7 ) led = 1;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / cmconv;
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

void forward() {
  Serial.println("Forwards");
  right.write(180);
  left.write(0);
}
void reverse() {
  Serial.println("Run away!");
  right.write(0);
  left.write(180);
}
void turnLeft() {
  Serial.println("Left!");
  left.write(90);
  right.write(180);
}
void turnRight() {
  Serial.println("Right!");
  right.write(180);
  left.write(90);
}
void allStop() {
  Serial.println("Stop");
  right.write(90);
  left.write(90);
}
