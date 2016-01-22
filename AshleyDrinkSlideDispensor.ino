

//Stepper Setup
const int Slide_directionPin = 13;
const int Slide_stepPin = 12;

//Slide Stepper Delay and Timings
int stepdelay = 1;
int steppause = 250;



void setup() {
  pinMode(directionPin, OUTPUT);    
  pinMode(stepPin, OUTPUT);   
}

void loop()
{
  digitalWrite(stepPin, LOW);
  
  for(int i = 0; i < count; i++) {
    digitalWrite(directionPin, LOW);
    delay(stepdelay);
    digitalWrite(directionPin, HIGH);
    delay(stepdelay);
  }
  delay(steppause);
  digitalWrite(stepPin, HIGH);
  
  for(int i = 0; i < count; i++) {
    digitalWrite(directionPin, LOW);
    delay(stepdelay);
    digitalWrite(directionPin, HIGH);
    delay(stepdelay);
  }
  delay(steppause);
}
