/* LED Blink, for Dragonfly
 
   This example code is in the public domain.
*/

#define myLed1 13 // green led
#define myLed2 25 // red   led
#define myLed3 26 // blue  led

void setup() 
{
  pinMode(myLed1, OUTPUT);
  digitalWrite(myLed1, HIGH);  // start with leds off, since active LOW
  pinMode(myLed2, OUTPUT);
  digitalWrite(myLed2, HIGH);
  pinMode(myLed3, OUTPUT);
  digitalWrite(myLed3, HIGH);
}

void loop() 
{
    digitalWrite(myLed1, !digitalRead(myLed1)); // toggle red led on
delay(1000);                                    // wait 1 second
    digitalWrite(myLed1, !digitalRead(myLed1)); // toggle red led off
    digitalWrite(myLed2, !digitalRead(myLed2));
delay(1000);
    digitalWrite(myLed2, !digitalRead(myLed2));
    digitalWrite(myLed3, !digitalRead(myLed3));
delay(1000);
    digitalWrite(myLed3, !digitalRead(myLed3));
delay(1000);
}
