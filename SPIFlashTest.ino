#include <SPI.h>

#define CSPIN  A1

void setup() 
{
  Serial.begin(115200);
  delay(2000);
  Serial.println("Serial initialized!");
  
  // comment these out if using the normal SPI pins
//  SPI.setSCK(14);  // Audio shield has SCK on pin 14
//  SPI.setMOSI(7);  // Audio shield has MOSI on pin 7

 // SPI.setMOSI(11);
 // SPI.setMISO(12);
//  SPI.setSCK(13);
 // pinMode(14, INPUT_PULLUP);
 // pinMode(15, INPUT_PULLUP);
 
  SPI.begin();
  pinMode(CSPIN, OUTPUT);
  digitalWrite(CSPIN, HIGH);
 // while (!Serial) ; 
  delay(5000);
  Serial.print("ID bytes: ");
  uint8_t id[4];
  SPI.beginTransaction(SPISettings(50000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CSPIN, LOW);
  SPI.transfer(0x9F);
  id[0] = SPI.transfer(0);
  id[1] = SPI.transfer(0);
  id[2] = SPI.transfer(0);
  id[3] = SPI.transfer(0);
  digitalWrite(CSPIN, HIGH);
  SPI.endTransaction();
  Serial.print(id[0], HEX); Serial.print(" "); Serial.print(id[1], HEX);  Serial.print(" ");  Serial.print(id[2], HEX); Serial.print(" ");  Serial.println(id[3], HEX);

  Serial.println("Winbond W25Q80BLUX1G    Chip ID = EF, 40, 14, 0");
  Serial.println("Macronix MX25L12835FZNI Chip ID = 0xC2 0x20 0x18 0xC2");
  Serial.println("Spansion S25FL127S      Chip ID = 0x01, 0x20, 0x18, 0x4D ");

}

void loop() {
}
