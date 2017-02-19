/*
Traffic light with crossing buzzer
 */
 
//Pins 13, 12, 11 green, yellow, red
int green = 13;
int yellow = 12;
int red = 11;
int buzzer = 10;

int pause = 4000;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(green, OUTPUT);     
  pinMode(yellow, OUTPUT);     
  pinMode(red, OUTPUT);     
  pinMode(buzzer, OUTPUT);  
  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(red, LOW);
  digitalWrite(buzzer, LOW);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(red, HIGH);    
  for( int i = 0; i < 20; i++)
  {
    tone(buzzer, 1760, 100);    
    delay(100);
    noTone(buzzer);
    delay(100);
  } 
  //delay(pause * 2);
  digitalWrite(yellow, HIGH);    
  delay(pause);
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(green, HIGH);
  delay(pause * 2);  
  digitalWrite(green, LOW);
  digitalWrite(yellow, HIGH);  
  delay(pause);
  digitalWrite(yellow, LOW);  
}
