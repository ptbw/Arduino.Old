#include <Servo.h> 

Servo left, right;
static int leftCentre = 75;
static int rightCentre = 75;
static int centreZone = 50;
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Basebot says hello!\n");
  left.attach(5);
  right.attach(6);
  
  total_illumination_time = 8 * illumination_time_default;
  
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  
}

void loop() {
  
//  digitalWrite(A, HIGH);
//  digitalWrite(B, LOW);      
//  digitalWrite(C, LOW);      
//  digitalWrite(D, LOW);      
//  delay(300);
//
//  digitalWrite(A, LOW);   
//  digitalWrite(B, HIGH);
//  digitalWrite(C, LOW);      
//  digitalWrite(D, LOW);      
//  delay(300); 
//  
//  digitalWrite(A, LOW);   
//  digitalWrite(B, LOW);
//  digitalWrite(C, HIGH);      
//  digitalWrite(D, LOW);      
//  delay(300); 
     
//  LedsOnAndWait(L0, 0.3); //LED 0 on for 0.3 seconds
//  LedsOnAndWait(L1, 0.3); //LED 1 on for 0.3 seconds
//  LedsOnAndWait(L2, 0.3); // etc.
//  LedsOnAndWait(L3, 0.3);
//  LedsOnAndWait(L4, 0.3);
//  LedsOnAndWait(L5, 0.3);
//  LedsOnAndWait(L6, 0.3);
  //delay(1000);
 
  SingleLedOn(L0);
  delay(1000);
  SingleLedOn(L1);
  delay(1000);
  SingleLedOn(L2);
  delay(1000);
  SingleLedOn(L3);
  delay(1000);
  SingleLedOn(L4);
  delay(1000);
  SingleLedOn(L5);
  delay(1000);
  SingleLedOn(L6);
  delay(1000);
}
