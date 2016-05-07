#include <Wire.h>
#define Serial SerialUSB // might not be needed in future

void setup() 
{
  
 Serial.begin(115200);
 delay(4000);

 //Wire.begin(0, 400000, true); // set master mode, I2C frequency at 400 kHz on back pads
 Wire.begin(0, 400000, false); // set master mode, I2C frequency at 400 kHz on pins 20/21
// Wire1.begin(0, 400000, false); // set master mode, I2C frequency at 400 kHz on pins A4/5
 delay(4000); 

 I2Cscan();
 
}


void loop() {
  // put your main code here, to run repeatedly:

}

// simple function to scan for I2C devices on the bus
void I2Cscan() 
{
    // scan for i2c devices
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
     error = Wire.transfer(address, NULL, 0, NULL, 0);
      

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
        Serial.print(address,HEX);
        Serial.println("  !");

        nDevices++;
    }  
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
        Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}
