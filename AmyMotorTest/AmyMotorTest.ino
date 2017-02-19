#include <Servo.h> 

Servo left, right;
static int leftCentre = 75;
static int rightCentre = 75;
static int centreZone = 50;
static int delta = 10;
static int idealWidth = 150;
static int minWidth = 50;
static int widthHysterisis = 20;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Basebot says hello!\n");
  left.attach(5);
  right.attach(6);
}

void loop() {
  // put your main code here, to run repeatedly:
  right.write(0);
  left.write(180);
  delay(1000);
  right.write(180);
  left.write(0);
  delay(1000);
  right.write(90);
  left.write(90);
  delay(1000);
 
}


void forward() {
  Serial.println("Forwards");
  right.write(rightCentre - delta);
  left.write(leftCentre + delta);
}
void back() {
  Serial.println("Run away!");
  right.write(rightCentre + delta);
  left.write(leftCentre - delta);
}
void turnLeft() {
  Serial.println("Left!");
  left.write(leftCentre - delta);
  right.write(rightCentre - delta);
}
void turnRight() {
  Serial.println("Right!");
  right.write(rightCentre + delta);
  left.write(leftCentre + delta);
}
void stop() {
  Serial.println("Stop");
  right.write(rightCentre);
  left.write(leftCentre);
}

