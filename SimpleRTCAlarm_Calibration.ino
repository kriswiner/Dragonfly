/*
  Simple RTC Alarm for Arduino Zero and MKR1000

  Demonstrates how to set an RTC alarm for the Arduino Zero and MKR1000

  This example code is in the public domain

  http://arduino.cc/en/Tutorial/SimpleRTCAlarm

  created by Arturo Guadalupi <a.guadalupi@arduino.cc>
  25 Sept 2015
  
  modified
  21 Oct 2015
*/

#include <RTC.h>

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 20;
const byte hours = 18;

/* Change these values to set the current initial date */
const byte day = 12;
const byte month = 12;
const byte year = 16;

uint8_t Seconds, Minutes, Hours;
bool AlarmMatch = false;

// variables for frequency determination
#define MainPeriod 1000 // integrate frequency pulse count over one second to get RTC error measure
long previousMillis = 0; // will store last time of cycle end
volatile unsigned long duration = 0; // accumulate pulse width
volatile unsigned int pulsecount = 0; 
volatile unsigned long previousMicros = 0;
float Freq, sumFreq, avgFreq;
int32_t sumCount, calreg, calib, AF;

#define myLed 13
#define freqMeas 5

void setup()
{
  Serial.begin(115200);
  delay(4000);
  
  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, 1);   //Butterfly leds are active LOW
  pinMode(freqMeas, INPUT); // measure 512 Hz frequency output from RTC

  RTC.pinMode(RTC.OUTPUT_512HZ); // output 512 Hz square wave on pin PC13

  RTC.setCalibration(0); // reset calibration to zero

  RTC.setTime(hours, minutes, seconds);
  RTC.setDate(day, month, year);

//  RTC.setAlarmTime(18, 20, 10);
//  RTC.enableAlarm(RTC.MATCH_HHMMSS);
  RTC.setAlarmSeconds(10);
  RTC.enableAlarm(RTC.MATCH_SS); // alarm every minute on the second mark
  
  RTC.attachInterrupt(alarmMatch);

  attachInterrupt(freqMeas, myinthandler, RISING);  // define interrupt for SW output of M41T62 RTC

}

void loop()
{
  Calibrate();
  
  Serial.print(RTC.getMonth()); Serial.print("/"); Serial.print(RTC.getDay()); Serial.print("/"); Serial.print(RTC.getYear()); Serial.print("  ");     
  Seconds = RTC.getSeconds();
  Minutes = RTC.getMinutes();
  Hours   = RTC.getHours();     
  if(Hours < 10) {Serial.print("0"); Serial.print(Hours);} else Serial.print(Hours);
  Serial.print(":"); 
  if(Minutes < 10) {Serial.print("0"); Serial.print(Minutes);} else Serial.print(Minutes); 
  Serial.print(":"); 
  if(Seconds < 10) {Serial.print("0"); Serial.println(Seconds);} else Serial.println(Seconds); 

  if(AlarmMatch) {
       AlarmMatch = false;
       Serial.println("Alarm Match!");
       digitalWrite(myLed, 0); delay(10); digitalWrite(myLed, 1);
  }
   
   delay(1000);
}

/********************************************************************************************************/
void myinthandler()
{
  uint32_t currentMicros = micros();
  duration += currentMicros - previousMicros;
  previousMicros = currentMicros;
  pulsecount++;
}

void alarmMatch()
{
 AlarmMatch = true;
}

void Calibrate()
{
    // Calibration function
  unsigned long currentMillis = millis();
  if( currentMillis - previousMillis >= MainPeriod)
  {
    previousMillis = currentMillis;
    unsigned long _duration = duration;
    unsigned long _pulsecount = pulsecount;
    duration = 0;
    pulsecount = 0;
    Freq = 1000000.0f / ((float) _duration);
    Freq *= _pulsecount;
    Serial.println("  ");
    Serial.println(currentMillis); Serial.println(Freq); Serial.println(_pulsecount); Serial.println(_duration);
    
     // write calibration data into RTC registers
     calib = (int32_t)(1000000.0f*(Freq - 512.0f)/512.0f);  // calculate number of calibration pulses ~ ppm, each pulse is 0.954 ppm
     Serial.print((int32_t)(1000000.0f*(Freq - 512.0f)/512.0f)); Serial.println(" ppm "); 
     calib*= -1;

     // calibration register max is -511 to +512 pulses ~ -487.1 to +488.5 ppm
     if(calib < -511) calib = -511; // check not to exceed max/min values
     if(calib > +512) calib = +512;
 
      calreg = RTC.getCalibration();
      calib = calreg + calib;
      RTC.setCalibration(calib);  // clock slow, add pulses, clock fast, subtract pulses
 
     // Check calibration
      calreg = RTC.getCalibration();
      Serial.print("Calibration pulses = "); Serial.println(calreg);
     }
}
