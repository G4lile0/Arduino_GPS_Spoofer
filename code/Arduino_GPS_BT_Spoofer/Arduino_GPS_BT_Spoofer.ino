// GPS Generator and Emulator
// Based on the UKHAS GPSgen Code: http://ukhas.org.uk/code:emulator
// Ported to arduino by Jim Blackhurst
// 
// ******************************************************
// * 24/AUG/2016                                        * 
// * adapted by G4lile0  for the proof of concept       *
// * Arduino bluetooth GPS spoofer for Pockemon Go      *
// * added custom character for the bearing and         *
// * added Joystick management                          *
// *  https://youtu.be/VqLSKPYKeXU                      *
// ******************************************************
//




#include <PString.h>    // from http://arduiniana.org/libraries/pstring/
#include <Time.h>       // from https://github.com/PaulStoffregen/Time
#include <LiquidCrystal.h>
#include <math.h>

// radians to degrees
#define DEGREES(x) ((x) * 57.295779513082320877)
// degrees to radians
#define RADIANS(x) ((x) / 57.295779513082320877)

#define LOG_BASE 1.4142135623730950488
#define LOG_POWER 5300.0

//#define PI 3.141592653589793

#define WIND_TURBULENCE 10 // chance of the wind changing the bearing by up to 1 degree per cycle of the simulation

//#define LAUNCH_LAT 52.213389
//#define LAUNCH_LON 0.096834
#define LAUNCH_LAT 40.4165
#define LAUNCH_LON -3.7025
#define LAUNCH_ALT 600

#define TERMINAL_VELOCITY 40 // meters per second
#define ASCENT_RATE 5 // meters per second
#define DESCENT_RATE 5 // meters per second

//should take 20secs to do 100m
//currently takes 50sec to do 100m

//http://www.engineeringtoolbox.com/air-altitude-pressure-d_462.html
//http://en.wikipedia.org/wiki/Atmospheric_pressure
#define LAUNCH_KPA 100
#define BURST_KPA 0.02
#define GPS_HZ 1

// SIM_HZ - the internal speed of the simulation
// if you are going to change this then you will need to scale simAccel acordingly
// Faster updates result in smaller distances per step being fed into the horizontal speed 
// Equation.  Due to the ATMEGA's 8bit arcitecture and 4 byte floats, it looses precision
// and Lon stops updating. 20Hz is the reccomneded value.
#define SIM_HZ 10


// DEBUG status - output either the intended NMEA sentances, or debug data
// 1 = debug, 0 = NMEA
#define DEBUG 0

// Define the pins used by the Liquid Crystal Display
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Joystick definition

#define AXES_X_PIN  A3
#define AXES_Y_PIN  A4

int Axes_X = 0 ;
int Axes_Y = 0 ;


// Status hold the current state of the sim.
// 0 = Ascent, 1 = Descent, 2 = Landed
int Status =0;

// simAccel - an artifical acceleration factor for speeds.
// This value can be used to maintain a acurate simulation of both the vertical and 
// horizontal speeds by compensating for the time it takes to process the calculations
// It can also be used to run the simulation at a fast-forward speed 
// With the sim running at 20Hz, a simAccel value of 1.4 will give accurate speeds 
float simAccel = 50;

time_t Now;
char buf[128];

float CurLon,CurLat,CurAlt,CurKPA,CurSpeed; 
unsigned long update_counter;
unsigned long output_counter;
float ascent_rate_mod = 0;
float tropo_wind_rate_mod = 0;
float strat_wind_rate_mod = 0;
//float lat_wind = 0;
//float lon_wind = 0;
float Bearing = 0.0;

float walk_Speed = 10.0;

float distancePerStep = 0.0;
float Drag;


long speedTest =0;
float msTest =0;
float speedTestResult = 0;

byte balloon[8] = {B01110, B11111, B11111, B11111, B01110, B00100, B01110, B01110};
byte chute[8] =   {B11111, B11111, B10001, B10001, B01010, B00100, B01110, B01110};
byte land[8] =    {B00000, B00000, B00000, B11111, B10101, B00100, B01110, B01110};
byte NE[8] =      {0b00000,0b00111,0b00011,0b00101,0b01000,0b10000,0b00000,0b00000};
byte SEast[8] =      {0b00000,0b00000,0b10000,0b01000,0b00101,0b00011,0b00111,0b00000};
byte SW[8] =      {0b00000,0b00000,0b00001,0b00010,0b10100,0b11000,0b11100,0b00000};
byte NW[8] =      {0b00000,0b11100,0b11000,0b10100,0b00010,0b00001,0b00000,0b00000};
byte scale_4[8] = {B11111, B00000, B00100, B00000, B00100, B00000, B00100, B00000};


void setup()
{
 // open the serial port at 9600 bps:
 Serial.begin(115200); 

  
 // set up the LCD's number of columns and rows: 
 lcd.begin(16, 2);
 lcd.createChar(0, balloon);
 lcd.createChar(1, chute);
 lcd.createChar(2, land);
 lcd.createChar(3, NE);
 lcd.createChar(4, SEast);
 lcd.createChar(5, SW);
 lcd.createChar(6, NW);
 lcd.createChar(7, scale_4);

long tempBits = 0;                               // create a long of random bits to use as seed
 for (int i=1; i<=32 ; i++)
 {                    
   tempBits = ( tempBits | ( analogRead( 0 ) & 1 ) ) << 1;
 }
 randomSeed(tempBits);                           // seed

 Bearing = random(359);

  
 setTime(14,14,14,23,8,2016); //Hardcode the datetime, this is effectivly the 'launch' time
 Now = now(); //assign the current time to the variable 'Now'
 
 // get first waypoint co-ordinate as launch position
 CurLon = LAUNCH_LON;
 CurLat = LAUNCH_LAT; 
 CurAlt = LAUNCH_ALT;
 CurKPA = LAUNCH_KPA;

 update_counter =  millis();
 output_counter =  millis();

 lcd.clear();

 speedTest = millis();
 msTest = CurAlt;
}


void loop()
{




  Axes_X=map(analogRead(AXES_X_PIN),0,1023,-256,256);
  // this small pause is needed between reading
  // analog pins, otherwise we get the same value twice
  delay(100);
  Axes_Y=map(analogRead(AXES_Y_PIN),0,1023,256,-256);


  if (abs(Axes_X)<5 & abs(Axes_Y)<5) {

    Axes_X=0;
    Axes_Y=0;
    Bearing=0;   
   }

  walk_Speed = hypot((Axes_X/100),(Axes_Y/100)); 

//  walk_Speed = sqrt((Axes_X*Axes_X/20)+(Axes_Y*Axes_Y/20));
  
//  delay(1000);

  
   Bearing=atan2(Axes_X,Axes_Y)*4068/71;

  // if joystick is in the center.. doesn't move.
   
  if (Bearing<0) Bearing=360+Bearing;   // to adjust the degrees to Bearing

   if (abs(Axes_X)<10 & abs(Axes_Y)<10) {

    Axes_X=0;
    Axes_Y=0;
    Bearing=0;   
   }

  
  if(DEBUG) Serial.println();
  if(DEBUG) Serial.print(Axes_X);
  if(DEBUG) Serial.println();
  if(DEBUG) Serial.print(Axes_Y);
  if(DEBUG) Serial.println();
  if(DEBUG) Serial.print(Bearing);
 
  if(DEBUG) Serial.println();


 
  Now = now();
  float windOffset = random(10);
  if (random(WIND_TURBULENCE)==1) Bearing += (windOffset-5)/10;
  if (Bearing>359) Bearing = 0;
  if (Bearing<0) Bearing = 359;

  if(DEBUG) Serial.println();
  if(DEBUG) Serial.print(speedTestResult);
  if(DEBUG) Serial.println();
  if(DEBUG) Serial.print(speedTestResult);

  if(DEBUG) Serial.println();
   
  if (CurAlt>2000 && CurAlt<2100)  Bearing = random(359);
  


  LCDFlight();
    
  if (Status != 2)
  {
    if (millis() > (update_counter + (1000/SIM_HZ)))
    {
      update_alt();
      
      
      distancePerStep = (walk_Speed/SIM_HZ)*simAccel;
      
      updateWindWalk(CurLat, CurLon, Bearing, distancePerStep); 
  
      if(DEBUG) Serial.print("[t");
      if(DEBUG) Serial.print(millis()-update_counter);
      if(DEBUG) Serial.print(" :VsA ");
      if(DEBUG) Serial.print(speedTestResult);
      if(DEBUG) Serial.print("]   ");
      
      update_counter =  millis();
      
      if(DEBUG) Serial.println();
    }

    if (millis() > (output_counter + (1000/GPS_HZ)))
    {
      if (!DEBUG) Output_NEMA(Now,CurLat,CurLon,CurAlt,Bearing,CurSpeed);
      output_counter =  millis();
    }
  }




  if (millis()>=(speedTest+10000))
  {
    speedTestResult = (CurAlt-msTest)/10;
    msTest=CurAlt;   
    speedTest = millis();
  }
}
