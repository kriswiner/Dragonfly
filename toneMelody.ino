/*
  Melody
 
 Plays a melody 
 
 circuit:
 * 8-ohm speaker on digital pin 3
 
 created 21 Jan 2010
 modified 30 Aug 2011
 by Tom Igoe 

This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/Tone
 
 */
 #include "pitches.h"

int tonePin = 3;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };

void setup() {

  pinMode(13, OUTPUT);

 // analogWriteFrequency(440);
  analogWrite(tonePin, 128);
  delay(2000);
  analogWrite(tonePin, 0);
 
 // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(tonePin, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);

  }
       digitalWrite(13, HIGH);
       tone(tonePin, 440);
       delay(1000);
       noTone(tonePin);
       digitalWrite(13, LOW);
       tone(tonePin, 880);
       delay(1000);
       noTone(tonePin);
       digitalWrite(13, HIGH);
       tone(tonePin, 1760);
       delay(1000);
       noTone(8);
       digitalWrite(13, LOW);
       tone(tonePin, 3520);
       delay(1000);
       noTone(tonePin);
       digitalWrite(13, HIGH);
       tone(tonePin, 4000);
       delay(3000);
       noTone(tonePin);
       digitalWrite(13, LOW);

}

void loop() {
  // no need to repeat the melody.
     for (int i = 0; i < 10; i++) {
       tone(tonePin, 3520);
       digitalWrite(13, HIGH);  
       delay(500);
       noTone(tonePin);
       digitalWrite(13, LOW);  
       delay(100);
       tone(tonePin, 3960);
       digitalWrite(13, HIGH);        
       delay(500);
       noTone(tonePin);
       digitalWrite(13, LOW);  
       delay(100);
 
     }
       
       delay(2000);
       
     for (int j = 0; j < 10; j++) {
       tone(tonePin, 3960);
       digitalWrite(13, HIGH);  
       delay(200);
       noTone(tonePin);
       digitalWrite(13, LOW);  
       delay(100);
       tone(tonePin, 3960);
       digitalWrite(13, HIGH);        
       delay(200);
       noTone(tonePin);
       digitalWrite(13, LOW);  
       delay(500);
 
     }
       
       delay(2000);
       
    for(int k = 0; k < 10; k++) {
      tone(tonePin, 1000);
       digitalWrite(13, HIGH);  
       delay(100);
       noTone(tonePin);
       digitalWrite(13, LOW);  
       delay(100);
       tone(tonePin, 2000);
       digitalWrite(13, HIGH);        
       delay(100);
       noTone(tonePin);
       digitalWrite(13, LOW);  
       delay(100);
       tone(tonePin, 3000);
       digitalWrite(13, HIGH);        
       delay(100);
       noTone(tonePin);
       digitalWrite(13, LOW);  
       delay(100);
       tone(tonePin, 4000);
       digitalWrite(13, HIGH);        
       delay(100);
       noTone(tonePin);
       digitalWrite(13, LOW);  
       delay(1000);
    }
       
      delay(2000);
 
       
}
