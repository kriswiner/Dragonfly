/*
  Turns any rgb led even one on digital pins not capable of PWM into a 16 million (2^24) color led

  This example code is in the public domain.

  by Thomas Roell,  modified February 20, 2018 by Kris Winer
 */

 #define redLed   25
 #define greenLed 13
 #define blueLed  38

volatile uint16_t red   = 128;
volatile uint16_t green = 128;
volatile uint16_t blue  = 128;

uint16_t accum[3];

static armv7m_timer_t led_timer;

static void led_callback(void) {
   accum[0] += red;   if (accum[0] >= 255) { accum[0] -= 255; digitalWrite(redLed, LOW); }   else { digitalWrite(redLed, HIGH); }
   accum[1] += green; if (accum[1] >= 255) { accum[1] -= 255; digitalWrite(greenLed, LOW); } else { digitalWrite(greenLed, HIGH); }
   accum[2] += blue;  if (accum[2] >= 255) { accum[2] -= 255; digitalWrite(blueLed, LOW); }  else { digitalWrite(blueLed, HIGH); }

   armv7m_timer_start(&led_timer, 1);
}


void setup() {
  // initialize rgb leds as outputs and test function; in this case active LOW
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, LOW); delay(1000); digitalWrite(redLed, HIGH); delay(1000); 
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, LOW); delay(1000); digitalWrite(greenLed, HIGH); delay(1000); 
  pinMode(blueLed, OUTPUT);
  digitalWrite(blueLed, LOW); delay(1000); digitalWrite(blueLed, HIGH); delay(1000); 
  
  armv7m_timer_create(&led_timer, (armv7m_timer_callback_t)led_callback);
  armv7m_timer_start(&led_timer, 1);
}


void loop() {
   for (red   = 0, blue  = 255, green = 0; red   <= 255; red++,   blue--  ) { delay(30); } 
   for (green = 0, red   = 255, blue  = 0; green <= 255; green++, red--   ) { delay(30); }
   for (blue  = 0, green = 255, red   = 0; blue  <= 255; blue++,  green-- ) { delay(30); }  
}
