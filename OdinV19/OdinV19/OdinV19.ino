#include <U8g2lib.h>
#include <EEPROM.h>         // FOR THE MEMORY STORAGE FROM SESSION TO SESSION
#include "HX711.h"          // OPERATIONAL AMPLIFIER TO EXADURATE THE SIGNAL FROM THE LOAD CELLS TO 2 BYTE DATA ON 2 PINS
#include <math.h>           // GENERAL MATH FUNCTIONS 
#include <elapsedMillis.h>  // TIMING FUNCTIONS
#include <avr/pgmspace.h>

// v13 notesCode changes for Odin 
// DONE reverse twisting polarity on menus but not on value setting
// DONE Twist or push to abort while it is running
// DONE stop running should work TEST
// - DONE redundant check for bag fell
// - DONE only stop motors and water on fallen bag side not both
// - DONE better finish screen
// v14
// - DONE code for foot switch
// v15
// - DONE ignore bag sensors
// v16
// - DONE combo pellets and auger speed are max until 60% of the way towards the pellet total
// v17
// - allow escape to menu if you've stopped the process and are paused
// v18
// - allow water only mode
// v19
// - fix LED testing so it can be run twice testledindex wasn't set back to zero

#define FOOTPEDALLEFT A5
#define FOOTPEDALRIGHT A7
#define MAINFONT u8g2_font_haxrcorp4089_tr
#define BIGFONT u8g2_font_helvB12_tr
#define SCREENWIDTH 128
#define SCREENHEIGHT 64
#define VLEADING 10
#define SCREENITEMS 5
// bag dropping variables. how much weight needs to go away from the max to count as a bag drop
// and how many cycles do we need to see a low weight before we stop

#define DROPPEDWEIGHTCONFIDENCETHRESHOLD 2
byte bagDropConfidenceCounter=0;
#define bagWidth 4
#define triangleWidth 6
#define triangleHeight 26
#define Bagger2VStart 32
#define runXInset 14

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// DEBUG
#define serialActive 0
#define Debug 0


// NOTES
// timers needed for scales
// timer 0 for pin 13 and 4
// timer 1 for pin 12 and 11 // problem for PWM of both motor controllers
// timer 2 for pin 10 and 9 //
// timer 3 for pin 5 and 3 and 2 >>> use for other motor
// timer 4 for pin 8 and 7 and 6
// 13,12,11,3,2,1
// Digital pins available for interupts
// 2, 3, 18, 19, 20, 21


// ENCODER
volatile int lastEncoded = 0;

//int lastMSB = 0;
//int lastLSB = 0;

// HARDWARE CONFIGURATION
// Opportunistic status field which says if you can use both motors simultaneously
#define motorsAreASharedResource 1
#define oneMotorAtATime 0         // just use a single motor at a time, zero when sure that you have sufficient power for both motors
#define baggerCount 2        // how many baggers are on the machine, could be 4 someday
#define rotaryChangeThreshold 16    // how many times does it need to be rotating before actually exiting out of operation
bool runScreenChanged = 0;
bool useBagSensors = 1; // default is yes
bool noWaterMode=0; // default is off which means normal is using water
bool menuScreenChanged = 0;
volatile byte rotaryCounter = 0;
byte rotaryThrottle = 0;
#define rotarySpeed 4

volatile int rotaryChangeCounter =0;

/// ARRAYS USED FOR UP TO 4 BAGGERS, NORMALLY 2
boolean bagStatus[2] = {0, 0}; // bag Status there or not there
int bagDurations[2] = {0, 0}; // stashing the time delay of the bags
byte baggerStepNumber[2] = {0, 0}; // the step array
long timers[3] = {0, 0, 0};
int motorStatus[2] = { -1, -1};
#define DEFAULTSCALECALIBRATION -202000; // about the right number
float scaleWeights[2] = {0, 0};
float scaleCalibrationTarget = 3.0000;
float maxScaleWeights[2] = {0, 0};
float scaleDryGoodsWeights[2] = {0, 0};
float scaleLastWeights[2] = {0, 0};
long timeoutPulsedMotor[2] = {0, 0};
long WaterTimerStart[2] = {0, 0};
int waterTimerWeightAtStopCommand[2] = {0, 0};
long waterTimerEnd[2] = {0, 0};
int waterTimeRan[2] = {0, 0};
float ScaleCalibrationFactors[2] = { -207820, -207820};
float scaleOverfills[2] = {1.00, 1.00};
float scaleCumulativeSubWeights[2] = {0, 0};
float scaleFinalSubWeight[2] = {0, 0};
float scaleFinalSupWeight[2] = {0, 0};
boolean subFillPhase2Stopped[2] = {0, 0};
boolean supFillPhase2Stopped[2] = {0, 0};
int scaleSameAsLastTimeCount[2] = {0, 0};
float scaleTapped[2] = {0, 0};
float waterFullShortStop[2] = {0.86, 0.86};
float waterShortStop[2] = {0.98, 0.98};
boolean waterOpen[2] = {0, 0};
boolean fullWaterOpen = 0;
int baggerStepProgressPercentage[2] = {0, 0};
float SubPhase2Part1Ratio[2] = {0, 0};
float SupPart1Ratio[2] = {0, 0};
long SubPhase2TimerEnd[2] = {0, 0};
long SubPhase2TimerStart[2] = {0, 0};
int SubPhase2TimerRan[2] = {0, 0};
int bagRemovedCounterConfidence[2] = {0, 0};
int bagConfidenceThreshold = 6; // bag is verified there 4 times before we're sure we can proceed
float SubPhase1TweakFromLastRun[2] = {0.00,0.00};
boolean redLightStatus[2] = {0, 0};
boolean greenLightStatus[2] = {0, 0};

#define bagDistanceThreshold 8 // how close does it have to be before it's there
#define gravityDelayTime 420


/// scales
/// GND DT SCK VCC
#define scale0ClockOut  A11
#define scale0DigitalOut  A10
#define scale1ClockOut  A8
#define scale1DigitalOut  A9
HX711 scale0(scale0DigitalOut, scale0ClockOut);
HX711 scale1(scale1DigitalOut, scale1ClockOut);
// stub for 4 bagger
//  HX711 scale2(scale2DigitalOut, scale2ClockOut);
//  HX711 scale3(scale3DigitalOut, scale3ClockOut);

#define scaleAveragingFactor 3 // default average times the scale is read to arrive at a number

///// TWEAKING values
float subDensityFactor[2] = {0.85,0.85}; // stop a certain percentage sooner than maximum speed value
float supDensityFactor[2] = {0.85,0.85}; // stop a certain percentage sooner than maximum speed value

// SUBSTRATE
float motorThresholdPhase1 = 0.45; // 45% of the target weight
float motorThresholdPhase2[2] = {0.96 * subDensityFactor[0],0.96 * subDensityFactor[1]};
float motorThresholdPhase3 = 0.99;
#define SubPulseTime 100 // default for how long is it on

// SUPPLEMENT
float motorThresholdPhase1Sup[2] = {0.90 * supDensityFactor[0],0.90 * supDensityFactor[1]} ; // % of the target weight where it slows down
float motorThresholdPhase2Sup = 0.96;

// SPEED
byte pelletHighSpeed = 150; // Very fast can lower this value
byte pelletLowSpeed = 50;
byte supHighSpeed = 150; // fast
byte supLowSpeed = 50; // slow speed for particular motor can be lower
#define motorAsymtoteAgressiveness 2 // how agressively does it slow down higher numbers are faster longer 1 is linear

// GRIDMODE
byte gridModifierSup = 0;
byte gridModifierMoisture = 0;

// LEARNING ALGORITHM
#define idealShortWaterTime 1
#define idealFinalSubPhaseRatio .94
#define idealFinalSupPhaseRatio .91
#define burstModeDelay 360

// TIMING - several timers 1 for each bager and a general one
elapsedMillis timer0;
elapsedMillis timer1;
elapsedMillis timer2;
#define interval 7000


///////////////////// all constants //////////////////////////////
#define prefLoadSpeed 50
// MUSIC notes would be nice to play some songs instead of just notes
#define keyChange 1
#define buzzer 24
#define noteEb 311
#define noteF 349
#define noteG 392
#define noteG2 784
#define noteA 440
#define noteAs 466
#define noteB 494
#define noteC 523
#define noteD 587
#define noteD0 293
#define noteE 659
#define noteE0 329
#define noteF2 698
#define noteF0 174

int startnote=noteEb;

#define fs 300
#define hs (.5*fs)
#define qs (.25*fs)
#define nL 1.25
#define lowNote  164
#define generalTimingDelay  1000 // general delays in the system for giving things time to react, a default we can tweak later.

// LIMIT RANGES FOR WHAT THE OPERATOR CAN SET IT TO ///////////////////
//allowable ranges of weight (x10 in pounds 150=15 but we divide later by 10 to get decimal pounds)
#define weightHigh 150 // 15 pounds
#define weightLow 10 // 1 pound
//allowable supplement mixture rates for twin screw unit
#define supplementMixMin 0
#define supplementMixMax 50
//allowable ranges of moisture
#define moistureHigh 78
#define moistureLow 50
//allowable ranges of residual moisture
#define residualMoistureSubHigh 50
#define residualMoistureSubLow 2
byte residualMoistureSub = 7;
#define residualMoistureSupHigh 50
#define residualMoistureSupLow 2
byte residualMoistureSup = 7;


// how much weight of each is required -- set defaults
float weightOfSubstrateToDispense = 1.6;
float weightOfSupplementToDispense = 0.4;
float weightOfWaterToDispense = 3.0;

float realSub = weightOfSubstrateToDispense / 10.00;
float realSup = weightOfSupplementToDispense / 10.00;
float realWater = weightOfWaterToDispense / 10.00;


/////////////////////////////  flags for status  /////////////////////////
byte currentlyRunning = 0;
byte currentlyRunningSubProcessId = 0;
byte currentlyRunningWaterProcessId = 0;
boolean processPaused = 0;
boolean whichMotor = 0;
boolean whichSideFillingNow = 0;
boolean footPedalLastStatus[2] = {0,0};

// VIBRATION
int scaleSameAsLastTimeCountThresholdToVibrate = 6; // how many loops at the same weight before we shake
#define vibrationDurationWhenNoProgress 4000 // seconds * 1000
boolean vibratorStatus = 0; // default off
boolean vibrationMode = 0; // default off

boolean quickMode = 0;

/// DEFAULTS
byte substrateType = 0; // default to HWFP, // 1 sawdust // 2 combi-pellet
int moistureSetValue = 65; // default - retrieves from memory
int supplementSetValue = 20; // default - retrieves from memory
int weightSetValue = 50;
long timeout = 0;


///////////////////////////// display            /////////////////////////
byte selectedMenuLength = 0; // a field that is used for scrolling in the menus


byte prefIndex = 0;


const char subStatusMenu_string0[] PROGMEM = "CHECK BAG";
const char subStatusMenu_string1[] PROGMEM = "TARE";
const char subStatusMenu_string2[] PROGMEM = "SUBSTRATE 1st ADD";
const char subStatusMenu_string3[] PROGMEM = "SUBSTRATE 1st SLOW";
const char subStatusMenu_string4[] PROGMEM = "SUPPLEMENT ADD";
const char subStatusMenu_string5[] PROGMEM = "SUPPLEMENT SLOW";
const char subStatusMenu_string6[] PROGMEM = "SUPPLEMENT WEIGH";
const char subStatusMenu_string7[] PROGMEM = "SUBSTRATE ADD";
const char subStatusMenu_string8[] PROGMEM = "SUBSTRATE ADD SLOW";
const char subStatusMenu_string9[] PROGMEM = "SUBPSTRATE WEIGH";
const char subStatusMenu_string10[] PROGMEM = "H20 ADD FAST";
const char subStatusMenu_string11[] PROGMEM = "H20 ADD SLOW";
const char subStatusMenu_string12[] PROGMEM = "H20 WEIGH";
const char subStatusMenu_string13[] PROGMEM = "REMOVE BAG";
const char subStatusMenu_string14[] PROGMEM = "BAG MISSING";
const char subStatusMenu_string15[] PROGMEM = "ADD A BAG";


PGM_P const subStatusMenu[] PROGMEM =  
{   
  subStatusMenu_string0,
  subStatusMenu_string1,
  subStatusMenu_string2,
  subStatusMenu_string3,
  subStatusMenu_string4,
  subStatusMenu_string5,
  subStatusMenu_string6,
  subStatusMenu_string7,
  subStatusMenu_string8,
  subStatusMenu_string9,
  subStatusMenu_string10,
  subStatusMenu_string11,
  subStatusMenu_string12,
  subStatusMenu_string13,
  subStatusMenu_string14,
  subStatusMenu_string15
  };


const char subStatusMenuTestMatrix_string0[] PROGMEM = "Pick Variances  ";
const char subStatusMenuTestMatrix_string1[] PROGMEM = "Var: Sup1%,H201%";
const char subStatusMenuTestMatrix_string2[] PROGMEM = "Var: Sup1%,H202%";
const char subStatusMenuTestMatrix_string3[] PROGMEM = "Var: Sup1%,H203%";
const char subStatusMenuTestMatrix_string4[] PROGMEM = "Var: Sup1%,H204%";
const char subStatusMenuTestMatrix_string5[] PROGMEM = "Var: Sup2%,H201%";
const char subStatusMenuTestMatrix_string6[] PROGMEM = "Var: Sup2%,H202%";
const char subStatusMenuTestMatrix_string7[] PROGMEM = "Var: Sup2%,H203%";
const char subStatusMenuTestMatrix_string8[] PROGMEM = "Var: Sup2%,H204%";
const char subStatusMenuTestMatrix_string9[] PROGMEM = "Var: Sup3%,H201%";
const char subStatusMenuTestMatrix_string10[] PROGMEM = "Var: Sup3%,H202%";
const char subStatusMenuTestMatrix_string11[] PROGMEM = "Var: Sup3%,H203%";
const char subStatusMenuTestMatrix_string12[] PROGMEM = "Var: Sup3%,H204%";
const char subStatusMenuTestMatrix_string13[] PROGMEM = "Var: Sup4%,H201%";
const char subStatusMenuTestMatrix_string14[] PROGMEM = "Var: Sup4%,H202%";
const char subStatusMenuTestMatrix_string15[] PROGMEM = "Var: Sup4%,H203%";
const char subStatusMenuTestMatrix_string16[] PROGMEM = "Var: Sup4%,H204%";

PGM_P const subStatusMenuTestMatrix[] PROGMEM =  
{   
  subStatusMenuTestMatrix_string0,
  subStatusMenuTestMatrix_string1,
  subStatusMenuTestMatrix_string2,
  subStatusMenuTestMatrix_string3,
  subStatusMenuTestMatrix_string4,
  subStatusMenuTestMatrix_string5,
  subStatusMenuTestMatrix_string6,
  subStatusMenuTestMatrix_string7,
  subStatusMenuTestMatrix_string8,
  subStatusMenuTestMatrix_string9,
  subStatusMenuTestMatrix_string10,
  subStatusMenuTestMatrix_string11,
  subStatusMenuTestMatrix_string12,
  subStatusMenuTestMatrix_string13,
  subStatusMenuTestMatrix_string14,
  subStatusMenuTestMatrix_string15,
  subStatusMenuTestMatrix_string16
  };



String bagMessages[] = {
  "No Bag"
  ,"Bag Ready"
  ,"Add a Bag"
  ,"Bag Added"
};






byte subMenuNumber = 0;
#define menuLevel1Length 7
byte menuLevel1Index = 0;

const char MenuLevel1_string0[] PROGMEM = "[x] Exit / Resume ";   // "String 0" etc are strings to store - change to suit.
const char MenuLevel1_string1[] PROGMEM = "Run Now ";
const char MenuLevel1_string2[] PROGMEM = "Stop Running ";
const char MenuLevel1_string3[] PROGMEM = "Setup ";
const char MenuLevel1_string4[] PROGMEM = "Recipe ";
const char MenuLevel1_string5[] PROGMEM = "Preferences ";
const char MenuLevel1_string6[] PROGMEM = "Test ";

PGM_P const menuLevel1[] PROGMEM =  
{   
  MenuLevel1_string0,
  MenuLevel1_string1,
  MenuLevel1_string2,
  MenuLevel1_string3,
  MenuLevel1_string4,
  MenuLevel1_string5,
  MenuLevel1_string6
  };

char progMemBuffer[30];

//String menuLevel1[menuLevel1Length] = {
//  "[x] Exit / Resume"
//  , "Stop Running"
//  , "Run Now"
//  , "Setup"
//  , "Recipe"
//  , "Preferences"
//  , "Test"
//};

#define menuLevel2SetupLength 5
byte menuLevel2SetupIndex = 0;

const char MenuLevel2Setup_string0[] PROGMEM = "^ RETURN ";   // "String 0" etc are strings to store - change to suit.
const char MenuLevel2Setup_string1[] PROGMEM = "Calibrate scales ";
const char MenuLevel2Setup_string2[] PROGMEM = "Reset scales ";
const char MenuLevel2Setup_string3[] PROGMEM = "Empty Sub hopper < ";
const char MenuLevel2Setup_string4[] PROGMEM = "Empty Sup hopper >";

PGM_P const menuLevel2Setup[] PROGMEM =  
{   
  MenuLevel2Setup_string0,
  MenuLevel2Setup_string1,
  MenuLevel2Setup_string2,
  MenuLevel2Setup_string3,
  MenuLevel2Setup_string4
  };

#define menuLevel2RecipeLength 8
byte menuLevel2RecipeIndex = 0;

const char MenuLevel2Recipe_string0[] PROGMEM = "^ RETURN ";   // "String 0" etc are strings to store - change to suit.
const char MenuLevel2Recipe_string1[] PROGMEM = "Moisture % ";
const char MenuLevel2Recipe_string2[] PROGMEM = "Weight in pounds ";
const char MenuLevel2Recipe_string3[] PROGMEM = "Sup Mixture % ";
const char MenuLevel2Recipe_string4[] PROGMEM = "Sub Moisture ";
const char MenuLevel2Recipe_string5[] PROGMEM = "Sup Moisture ";
const char MenuLevel2Recipe_string6[] PROGMEM = "SubStrate Type ";
const char MenuLevel2Recipe_string7[] PROGMEM = "Vibration Mode ";

PGM_P const menuLevel2Recipe[] PROGMEM =  
{   
  MenuLevel2Recipe_string0,
  MenuLevel2Recipe_string1,
  MenuLevel2Recipe_string2,
  MenuLevel2Recipe_string3,
  MenuLevel2Recipe_string4,
  MenuLevel2Recipe_string5,
  MenuLevel2Recipe_string6,
  MenuLevel2Recipe_string7
  };


#define menuLevel3CalibrationLength 6
int menuLevel3CalibrationIndex = 0;





#define menuLevel2RecipeUnitsLength 8
byte menuLevel2RecipeUnitsIndex = 0;
String menuLevel2RecipeUnits[menuLevel2RecipeLength] = {
  " "
  , "% water"
  , "pounds"
  , "% supplement"
  , "% water"
  , "% water"
  , " "
  , " "
};

#define menuLevel2PreferencesLength 9
byte menuLevel2PreferencesIndex = 0;
//String menuLevel2Preferences[menuLevel2PreferencesLength] = {
//  "^ RETURN"
//  , "Sub Density:"
//  , "Sup Density:"
//  , "Sub Hi Speed Limit:"
//  , "Sub Low Speed Limit:"
//  , "Sup Hi Speed Limit:"
//  , "Sup Lo Speed LImit:"
//};
const char MenuLevel2Preferences_string0[] PROGMEM = "^ RETURN ";   // "String 0" etc are strings to store - change to suit.
const char MenuLevel2Preferences_string1[] PROGMEM = "Sub Density: ";
const char MenuLevel2Preferences_string2[] PROGMEM = "Sup Density: ";
const char MenuLevel2Preferences_string3[] PROGMEM = "Sub Hi Speed Limit: ";
const char MenuLevel2Preferences_string4[] PROGMEM = "Sub Low Speed Limit: ";
const char MenuLevel2Preferences_string5[] PROGMEM = "Sup Hi Speed Limit: ";
const char MenuLevel2Preferences_string6[] PROGMEM = "Sup Lo Speed LImit: ";
const char MenuLevel2Preferences_string7[] PROGMEM = "Use Bag Sensors? ";
const char MenuLevel2Preferences_string8[] PROGMEM = "No Water Mode ";

PGM_P const menuLevel2Preferences[] PROGMEM =  
{   
  MenuLevel2Preferences_string0,
  MenuLevel2Preferences_string1,
  MenuLevel2Preferences_string2,
  MenuLevel2Preferences_string3,
  MenuLevel2Preferences_string4,
  MenuLevel2Preferences_string5,
  MenuLevel2Preferences_string6,
  MenuLevel2Preferences_string7,
  MenuLevel2Preferences_string8
  };





#define menuLevel2TestLength 9
byte menuLevel2TestIndex = 0;
//String menuLevel2Test[menuLevel2TestLength] = {
//  "^ RETURN"
//  , "Test Sub Motor"
//  , "Test Sup Motor"
//  , "Test Left Water"
//  , "Test Right Water"
//  , "Test Scales"
//  , "Test Bags"
//  , "Test Water Valves"
//  , "Test Lights"
//};

const char MenuLevel2Test_string0[] PROGMEM = "^ RETURN ";   // "String 0" etc are strings to store - change to suit.
const char MenuLevel2Test_string1[] PROGMEM = "Test Sub Motor ";
const char MenuLevel2Test_string2[] PROGMEM = "Test Sup Motor ";
const char MenuLevel2Test_string3[] PROGMEM = "Test Left Water ";
const char MenuLevel2Test_string4[] PROGMEM = "Test Right Water ";
const char MenuLevel2Test_string5[] PROGMEM = "Test Scales ";
const char MenuLevel2Test_string6[] PROGMEM = "Test Bags ";
const char MenuLevel2Test_string7[] PROGMEM = "Test Water Valves ";
const char MenuLevel2Test_string8[] PROGMEM = "Test Lights ";


PGM_P const menuLevel2Test[] PROGMEM =  
{   
  MenuLevel2Test_string0,
  MenuLevel2Test_string1,
  MenuLevel2Test_string2,
  MenuLevel2Test_string3,
  MenuLevel2Test_string4,
  MenuLevel2Test_string5,
  MenuLevel2Test_string6,
  MenuLevel2Test_string7,
  MenuLevel2Test_string8,
  
  };



int MenuSelectedItem = -1;

byte statusSubMenuIndex = 0;
#define statusSubMenuSize 16
byte subStatusMenuTestMatrixIndex = 0;
#define subStatusMenuTestMatrixSize = 17
byte subStatusMenuTestMatrixLastBagNumber = 0;
#define motorTestModesSize 11
byte motorTestModeIndex = 0;
byte runningTaskIndex = 0;
int currentTotalBagsCounter = 1; // start at 1 not zero since
byte functionsMenuIndex = 0;
byte SettingModeMenuIndex = 0;
String updateMessages[2] = {"                        ", "                        "};
String statusMessage[3] = {"                        ", "                        ", "                        "}; // the default message for what is happening (second line of text)
byte menuColumn = 0; // which level of menu are we in 0 = no menu 1 = functions menu 2 = we're in a setting options column
String DisplayText = "Initializing";
String lastTextPrintedLine0 = "zz";
String lastTextPrintedLine1 = "zzz";

// for an abandonned menu selection it returns to the main menu
byte menuTimeoutCounter = 0;
#define MenuTimeout 100

////////////////////// BUTTON INPUTS ///////////////////////////////////////////////

// PINS
// defines pins numbers
// ultrasonic rangefinding - bag sensors
#define bagTrigPinsL A0
#define bagTrigPinsR A3
#define bagEchoPinsL A1
#define bagEchoPinsR A2
#define waterPinsL 25
#define waterPinsR 27
#define fullWaterPin 29

#define supMotorDirectionPin 10
#define supMotorSpeedPin 13
#define subMotorDirectionPin 9
#define subMotorSpeedPin 11

#define supMotorDirectionPin 10
#define supMotorSpeedPin 13
#define subMotorDirectionPin 9
#define subMotorSpeedPin 11



#define vibrationPin 31
// the noise maker
#define buzzer 24


///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

bool menuMode = 0;
int percentComplete[2] = {0, 0};
String messagesLine1[2] = {"                        ", "                        "};
String messagesLine2[2] = {"                        ", "                        "};
String messagesLine3[2] = {"                        ", "                        "};
int LEDtestLightdelay = 0;

U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 5, /* data=*/ 4, /* cs=*/ 6, /* dc=*/ 7, /* reset=*/ 8);

#define lineHeight 8
#define startupLogoWidth 115
#define startupLogoHeight 61
#define LEDGreenL 32
#define LEDGreenR 36
#define LEDRedL 34
#define LEDRedR 38
byte testLEDindex = 0;


// Used for generating interrupts using CLK signal
#define PinA 2

// Used for reading DT signal
#define PinB 3

// Used for the push button switch
#define PinSW 22

// Keep track of last rotary value
int lastCount = 50;
volatile boolean RotaryBusy = 0;

// Updated by the RotaryInput (Interrupt Service Routine)
volatile int virtualPosition = 50;



//int col = 0;
//int dir = 1;
//int vdir = 1;
unsigned long lastInterruptTime = 0;

void setup(void) {
  pinMode(subMotorDirectionPin, OUTPUT);
  pinMode(supMotorDirectionPin, OUTPUT);
  digitalWrite(supMotorDirectionPin, LOW);
  digitalWrite(subMotorDirectionPin, LOW);
  pinMode(subMotorSpeedPin, OUTPUT);
  pinMode(supMotorSpeedPin, OUTPUT);
  analogWrite(supMotorSpeedPin, 0);
  analogWrite(subMotorSpeedPin, 0);
  
  pinMode(LEDGreenL, OUTPUT);
  pinMode(LEDRedL, OUTPUT);
  pinMode(LEDGreenR, OUTPUT);
  pinMode(LEDRedR, OUTPUT);


  // Rotary pulses are INPUTs
  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);

  // Switch is floating so use the in-built PULLUP so we don't need a resistor
  pinMode(PinSW, INPUT_PULLUP);
  digitalWrite(PinA, HIGH); //turn pullup resistor on
  digitalWrite(PinB, HIGH); //turn pullup resistor on

// v14
  pinMode(FOOTPEDALLEFT, INPUT_PULLUP);
  pinMode(FOOTPEDALRIGHT, INPUT_PULLUP);


  // Attach the routine to service the interrupts
  attachInterrupt(digitalPinToInterrupt(PinA), RotaryInput, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PinB), RotaryInput, CHANGE);

//    Serial.begin(19200);
//    delay(50);
//    while (Serial.available()) {
//    }
//    Serial.print(F("starting"));

  /////////////////////////////////
  /// start motors //

  analogWrite(supMotorSpeedPin, 0);
  analogWrite(subMotorSpeedPin, 0);
  digitalWrite(vibrationPin, HIGH);
  digitalWrite(waterPinsL, HIGH);
  digitalWrite(waterPinsR, HIGH);
  digitalWrite(fullWaterPin, HIGH);
  pinMode(fullWaterPin, OUTPUT);
  pinMode(waterPinsL, OUTPUT);
  pinMode(waterPinsR, OUTPUT);
  pinMode(vibrationPin, OUTPUT);
  digitalWrite(waterPinsL, HIGH);
  digitalWrite(waterPinsR, HIGH);
  digitalWrite(fullWaterPin, HIGH);
  digitalWrite(vibrationPin, HIGH);



  ///// DISPLAY //////
//  drawStartup();

  randomSeed(analogRead(0));
  timer0 = 0; // clear the timer at the end of startup
  pinMode(bagTrigPinsL, OUTPUT); // Sets the trigPin as an Output
  pinMode(bagEchoPinsL, INPUT); // Sets the echoPin as an Input
  pinMode(bagTrigPinsR, OUTPUT); // Sets the trigPin as an Output
  pinMode(bagEchoPinsR, INPUT); // Sets the echoPin as an Input


  menuColumn = 0;

  /// SCALES
  scale0.set_scale();
  TareScales(0);  //Reset the scale to 0
  scale0.set_scale(ScaleCalibrationFactors[0]); //Adjust to this calibration factor
  scale1.set_scale();
  TareScales(1);  //Reset the scale to 0
  scale1.set_scale(ScaleCalibrationFactors[1]); //Adjust to this calibration factor

  // ENSURE WATER STAYS OFF AT STARTUP
  digitalWrite(waterPinsL, HIGH);
  digitalWrite(waterPinsR, HIGH);

  drawStartup();
//  SavePrefs();
  LoadPrefs();// load the stored preferences -- before setting the scale
  drawStartup();
  Valkyyries();
  footPedalLastStatus[0] = TestFootPedal(0);// v14 set the status of the foot pedal to the current state
  delay(50);
  footPedalLastStatus[1] = TestFootPedal(1);// v14 set the status of the foot pedal to the current state

}

void loop(void) {
  TimersUpdate(); // update all the timer arrays
  checkRotaryInput();
  
  if (menuScreenChanged == 1) {
    rotaryCounter = 0; //reset the rotary
    DrawMenu();    
  } else if (menuMode == 1) {
    // just check for button or timeout
    MenuTimeoutCheck();
  } else if (menuMode == 0) { // not in the menus just show the run screen,
    // the main loop where everything happens
    DoYourJob();
  } 
}
