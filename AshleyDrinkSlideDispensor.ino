/*------------------------------------------------------------------------------------------------------
                              Author : Evan Richinson aka MrRetupmoc42
               
Arduino Driven Drink Slider System to Automatically Pour Drinks / Shots
   
January 20th 2016   : Stepper Code Imported and Updating for Timing / Postitional Data with Button Input Control
                   
-------------------------------------------------------------------------------------------------------*/

//Behaviour Data
bool Debug = true;

//Stepper Pin Setup
const int Slide_directionPin = 13;
const int Slide_stepPin = 12;
const int Dispense_directionPin = 11;
const int Dispense_stepPin = 10;

const int Forward_ButtonPin = 9;
const int Reverse_ButtonPin = 8;
const int Mode_ButtonPin = 7;


//Slide Stepper Delay and Timings
int Stepper_stepdelay = 1;
int Slide_Move_Delay = 100;
int Dispense_Extend_Delay = 250;
int Dispense_Retract_Delay = 120;

//Stepper Max Bounds Setup
int Slide_maxSteps = 500;
int Dispense_maxSteps = 20;

//Stepper Postiitonal Data
int Slide_Positions = 6;
int Slide_Positions_steps = Slide_maxSteps / Slide_Positions;


//Sets the Arduino Pins as Such
void setup() {
  //Sets the Stepper Pins
  pinMode(Slide_directionPin, OUTPUT);    
  pinMode(Slide_stepPin, OUTPUT);   
  pinMode(Dispense_directionPin, OUTPUT);    
  pinMode(Dispense_stepPin, OUTPUT);

  pinMode(Forward_ButtonPin, INPUT);
  pinMode(Reverse_ButtonPin, INPUT);
  pinMode(Mode_ButtonPin, INPUT);

  if (Debug) Serial.begin(9600);
}

//Main Button Input Control Code
void loop()
{
  
  
  
}

//Moves Slider from Previous to Next Postition 
void SlideMovetoPosition() {
  //Movement Code
  digitalWrite(Slide_stepPin, LOW);
  
  for(int i = 0; i < 50; i++) {
    digitalWrite(Slide_directionPin, LOW);
    delay(Stepper_stepdelay);
    digitalWrite(Slide_directionPin, HIGH);
    delay(Stepper_stepdelay);
  }
  delay(Slide_Move_Delay);
}

//Dispenses Drink ( Might need to Vary Timings per Drink Choice )
void Dispense() {
  digitalWrite(Dispense_directionPin, LOW);
  
  for(int i = 0; i < 50; i++) {
    digitalWrite(Dispense_directionPin, LOW);
    delay(Stepper_stepdelay);
    digitalWrite(Dispense_directionPin, HIGH);
    delay(Stepper_stepdelay);
  }
  delay(Dispense_Extend_Delay);

  digitalWrite(Dispense_directionPin, HIGH);
  
  for(int i = 0; i < 50; i++) {
    digitalWrite(Dispense_directionPin, LOW);
    delay(Stepper_stepdelay);
    digitalWrite(Dispense_directionPin, HIGH);
    delay(Stepper_stepdelay);
  }
  delay(Dispense_Retract_Delay);
}
