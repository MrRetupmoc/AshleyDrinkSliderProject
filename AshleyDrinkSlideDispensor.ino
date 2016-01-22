/*------------------------------------------------------------------------------------------------------
                              Author : Evan Richinson aka MrRetupmoc42
               
Arduino Driven Drink Slider System to Automatically Pour Drinks / Shots
   
January 20th 2016   : Stepper Code Imported and Updating for Timing / Postitional Data with Button Input Control
                      Stepper Homing, Drink Dispense Delay based on Position
                   
-------------------------------------------------------------------------------------------------------*/

//Behaviour Data
bool Debug = true;

//Stepper Pin Setup
const int Slide_directionPin = 13;
const int Slide_stepPin = 12;
const int Dispense_directionPin = 11;
const int Dispense_stepPin = 10;

const int Slide_HomeLimitPin = 9;

const int Forward_ButtonPin = 8;
const int Reverse_ButtonPin = 7;
const int Mode_ButtonPin = 6;


//Stepper Max Bounds Setup
const int Slide_maxSteps = 500;
const int Dispense_maxSteps = 20;
int Slide_StepsCurrent = 0;

//Stepper Postiitonal Data
const int Slide_Positions = 6;
const int Slide_Positions_steps = Slide_maxSteps / Slide_Positions;

//Slide Stepper Delay and Timings
const int Stepper_stepdelay = 1;
int Slide_Move_Delay = 100;
int Dispense_Extend_Delay[Mode_ButtonPin] = {100, 200, 300, 400, 500, 600}; //Delay Pour per Drink ( Convert to Rough ml Liquid Dispense in Future? )
int Dispense_Retract_Delay = 120;



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

  if (Debug) Serial.println("Hello World.");
  if (Debug) Serial.println("Performing Homing Function");
  SlideMovetoHome();

  if (Debug) Serial.println("Ready to Pour Drinks, Master");
}

//Main Button Input Control Code
void loop()
{
  if(Forward_ButtonPin) {
    if (Debug) Serial.println("Going Forward a Drink");
    SlideMovetoPosition(false, Slide_Positions_steps);
  }
  else if(Reverse_ButtonPin) {
    if (Debug) Serial.println("Going Reverse a Drink");
    SlideMovetoPosition(true, Slide_Positions_steps);
  }
  else if(Mode_ButtonPin) {
    if (Debug) Serial.println("Dispensing Drink");
    Dispense();
  }

  delay(250);
}

//Moves Slider to Home
void SlideMovetoHome() {
  digitalWrite(Slide_stepPin, LOW);

  //Move Slider to Home
  do {
    digitalWrite(Slide_directionPin, LOW);
    delay(Stepper_stepdelay);
    digitalWrite(Slide_directionPin, HIGH);
    delay(Stepper_stepdelay);
  }
  while (digitalRead(Slide_HomeLimitPin));

  int Slide_StepsCurrent = 0;
}

//Moves Slider from Previous to Next Postition 
void SlideMovetoPosition(bool Direction, int MoveCount) {
  
  //Movement Code ( False Moves Outward, True Moves Inward )
  if (Direction) digitalWrite(Slide_stepPin, HIGH);
  else digitalWrite(Slide_stepPin, LOW);

  //If Forward and Not Over Max OR Reverse and Not Under Min
  if (Direction && (Slide_StepsCurrent + MoveCount) < Slide_maxSteps || !Direction && (Slide_StepsCurrent - MoveCount) > 0) {

    //Ouput to User
    if (Debug) Serial.println("Performing Requested Move."); 
    
    //Move Slider
    for(int i = 0; i < MoveCount; i++) {
      digitalWrite(Slide_directionPin, LOW);
      delay(Stepper_stepdelay);
      digitalWrite(Slide_directionPin, HIGH);
      delay(Stepper_stepdelay);
      
    }
    delay(Slide_Move_Delay);
    
    if (Direction) Slide_StepsCurrent += MoveCount;
    else Slide_StepsCurrent -= MoveCount;
  }
  else if (Debug) Serial.println("Didn't Perform Move, Out of Bounds?"); 
 
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

  //Dispense Proper Amount Drink Delay
  delay(Dispense_Extend_Delay[(Slide_StepsCurrent / Slide_Positions_steps)]);

  digitalWrite(Dispense_directionPin, HIGH);
  
  for(int i = 0; i < 50; i++) {
    digitalWrite(Dispense_directionPin, LOW);
    delay(Stepper_stepdelay);
    digitalWrite(Dispense_directionPin, HIGH);
    delay(Stepper_stepdelay);
  }
  delay(Dispense_Retract_Delay);
}
