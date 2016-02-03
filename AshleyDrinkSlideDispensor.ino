/*------------------------------------------------------------------------------------------------------
                              Author : Evan Richinson aka MrRetupmoc42
               
Arduino Driven Drink Slider System to Automatically Pour Drinks / Shots
   
January 20th 2016   : Stepper Code Imported and Updating for Timing / Postitional Data with Button Input Control
                      Stepper Homing, Drink Dispense Delay based on Position

January 27th 2016   : Created Init Graphics and Arrays for Drink Dispensing.                  

Febuary 2th 2016    : Updated / Merged Graphics into Controller, Updating more of the Logic...
                   
-------------------------------------------------------------------------------------------------------*/

#include <SPI.h>       // this is needed for display
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <NewPing.h>

// For the Adafruit shield, these are the default.
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

//Screen Data----------------------------------------------------------------------------------------- 

//Behaviour Data
bool Debug = true;
int Rotation = 1;

//Movement Data-----------------------------------------------------------------------------------------

//Stepper Pin Setup
const int Slide_directionPin = 13;
const int Slide_stepPin = 12;
const int Slide_HomeLimitPin = 11;

const int Dispense_directionPin = 10;
const int Dispense_stepPin = 9;
const int Dispense_HomeLimitPin = 8;


//Stepper Max Bounds Setup
const int Slide_maxSteps = 500;
const int Dispense_maxSteps = 20;
int Slide_StepsCurrent = 0;

//Stepper Postiitonal Data
const int Slide_Positions = 6;
const int Slide_Positions_steps = Slide_maxSteps / Slide_Positions;

//Dispense Timing
int Dispense_mlPerSec = 120; // Untuned

//Slide Stepper Delay and Timings
const int Stepper_stepdelay = 1;
int Slide_Move_Delay = 100;
int Dispense_Retract_Delay = 120;

//bool Dispense = false;

int DistanceCm = 0;
int DistanceCm_Offset = 6.6;


//Array Data-----------------------------------------------------------------------------------------

int DrinkChoice = 0;
const int DrinkChoice_Max = 5;

String DrinkName[DrinkChoice_Max] = {"  Tequila Sunrise", "     Screw Driver", "  Carribean Bliss", "    Whiskey Sour", "    Blissletoe"};
//int DrinkDispenseTime[DrinkChoice_Max] = {3000 , 1500, 2000, 4123, 2392};

//                         6, 14, 22, 30, 38, 46, 
int BottleLocations[6] = { 0, 8, 16, 24, 32, 40, 48 };
int Recipy[Drink#,DrinkRecipie] = { { 2, 5, 5, 5 }, { 1, 5, 5, 5 }, { 1, 2, 5, 5, 5 } , {3, 4, 4, 4}, {1, 5, 6, 6, 6} };
//int DrinkMix[DrinkChoice_Max, 6] = { {1,2,3,4,5,6}, {1,0,0,0,0,0}, };

//----------------------------------------------------------------------------------------

void setup() {
  //Sets the Stepper Pins
  pinMode(Slide_directionPin, OUTPUT);    
  pinMode(Slide_stepPin, OUTPUT);   
  pinMode(Dispense_directionPin, OUTPUT);    
  pinMode(Dispense_stepPin, OUTPUT);

  if (Debug) {
    Serial.begin(9600);
    Serial.println("Hello World.");
  }
 
  tft.begin();
  tft.setRotation(Rotation);
  tft.fillScreen(ILI9341_BLACK);
  tft.println("Screen Started...");

  if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }
  
  if (Debug) Serial.println("Capacitive Touchscreen Started");
  tft.println("Touchscreen Enabled...");

  if (Debug) Serial.println("Performing Homing Function");
  tft.println("Homing Slides to 0 Position...");

  DispenseMovetoHome();
  tft.println("Dispense Slide Homed...");

  SlideMovetoHome();
  tft.println("Drink Slide Homed...");
  

  if (Debug) Serial.println("Ready to Pour Drinks, Master");
  tft.println("Sucessfully Initicated...");

  delay(1000);
  
  DrawScreen(); 
}


void loop(void) {
  // Wait for a touch
  if (! ctp.touched()) {
    return;
  }

  // Retrieve a point  
  TS_Point p = ctp.getPoint();

  // Print out the touch coordinates
  Serial.print("("); Serial.print(p.x);
  Serial.print(", "); Serial.print(p.y);
  Serial.println(")");

  //When Screen is Touched in the Right Area's
  if (p.x < 40 && p.y > 20) { // Top of Screen Refresh
    DrawChoiceScreen();
  }
  else if (p.x > 120 && p.x < 180 && p.y > 100 && p.y < 230) { // Enter Area
    //Dispense = true;
    //DrawScreen();
    
    tft.println("     Dispensing Drink");
    
    delay(DrinkDispenseTime[DrinkChoice]);

    Dispense();
    
    //Dispense = false;
    DrawChoiceScreen();
  }
  else if (p.x > 120 && p.x < 180 && p.y > 0 && p.y < 80) { // Forward Button
    if (DrinkChoice < (DrinkChoice_Max - 1)) {
      DrinkChoice += 1;
      DrawChoiceScreen();
    }
  }
  else if (p.x > 120 && p.x < 180 && p.y > 250 && p.y < 320) { // Back Button
    if (DrinkChoice > 0) {
      DrinkChoice -= 1;
      DrawChoiceScreen();
    }
  }

  delay(100);
}

void DispenseMovetoHome() {
  //when limit is hit set offset to ultrasonic value and real distance to 0
  DistanceCm_offset = Distance;
  DistanceCm = 0;
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

  // update where we are from home
  //GetDistance();
  
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

void DrawChoiceScreen() {
  tft.setRotation(Rotation);
  
  tft.fillScreen(ILI9341_BLACK);
  
  tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_PINK); 
    tft.setTextSize(2);
  tft.println("       Welcome to :");
  tft.println("Ashley's Drink Dispensory");
  tft.println();
  tft.println();
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(3);
  tft.println(DrinkName[DrinkChoice]); // drink name
  tft.println();
  tft.println();
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(4);
  tft.println(" <  Enter  > ");
  tft.println();
    tft.setTextColor(ILI9341_YELLOW); 
    tft.setTextSize(2);
  //if(Dispense) tft.println("     Dispensing Drink");
}


void GetDistance() {
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  DistanceCm = uS / US_ROUNDTRIP_CM; // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.print("Ping: ");
  Serial.print(DistanceCm);
  Serial.println("cm");
}
