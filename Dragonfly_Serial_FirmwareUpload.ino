/*
 * Sketch to load SENtral firmware onto the M24512DFC EEPROM on board the Ultimate Sensor Fusion Solution board using the Dragonfly 
 * QSPI flash to hold the firmware file (vis drag and drop on a laptop) and writing via I2C commends to the EEPROM. This is all
 * done wthout having to use the intermeduiary of an SD Flash Card Reader.
 */
 #include <FS.h>
#include <Wire.h>

#define EM7180_ADDRESS            0x28               // Address of the EM7180 SENtral sensor hub#define M24512DFM_DATA_ADDRESS
#define M24512DFM_DATA_ADDRESS    0x50               // Address of the 500 page M24512DFM EEPROM data buffer, 1024 bits (128 8-bit bytes) per page
#define EM7180_AlgorithmControl   0x54
#define EM7180_PassThruControl    0xA0
#define EM7180_PassThruStatus     0x9E
#define SENTRAL0_VCC_PIN          31
#define SENTRAL0_GND_PIN          13
#define LED_PIN                   25

File file;

void setup()
{
  DOSFS.begin();
  delay(4000);
  
  // Set up LED pin
  pinMode (LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  
    // Power-up Sentral_0
  pinMode (SENTRAL0_GND_PIN, OUTPUT);
  digitalWrite(SENTRAL0_GND_PIN, LOW);
  pinMode (SENTRAL0_VCC_PIN, OUTPUT);
  digitalWrite(SENTRAL0_VCC_PIN, HIGH);
  delay(500);
  
  // Initialize Dragonfly I2C bus
  Wire.begin(TWI_PINS_20_21);
  delay(1000);
  Wire.setClock(400000);                             // Pins 20 and 21
  delay(100);
  
  Serial.begin(115200);
  
  I2Cscan();
  
  // Put EM7180 SENtral into pass-through mode
  SENtralPassThroughMode();

  I2Cscan();

  /*
// sd_file.open("/LSMM.fw", O_RDONLY);  // SENtral Reference Board v.03 w/LSM9DS0 and MS5637
// sd_file.open("/EMLPH.fw", O_RDONLY);  // SENtral Reference Board v.03 w/LSM9DS0 and LPS25H
// sd_file.open("/EMMSM.fw", O_RDONLY);  // SENtral Reference Board v.03 w/MPU9250 and MS5637
// sd_file.open("/EMMST.fw", O_RDONLY);  // SENtral Reference Board v.03 w/MPU9250 and MS5637 w/T
// sd_file.open("/EMRML.fw", O_RDONLY);  // Lavinia
// sd_file.open("/EMMPU.fw", O_RDONLY);  // SENtral Reference Board v.03 w/MPU9250 and BMP280
//   sd_file.open("/EMMPK.fw", O_RDONLY);  // Kinetrope v.03m board w/EM7180
//sd_file.open("/EMMPUN.fw", O_RDONLY);  // SENtral Reference Board v.03 w/MPU9250 and BMP280 w/accel cal
//  sd_file.open("/EMBMW.fw", O_RDONLY);  // old version
//  sd_file.open("/EMBMZ.fw", O_RDONLY);    // new version with temp-compensated mag fields
//  sd_file.open("/EMBMI.fw", O_RDONLY);  // BMI160+AK8963C
   */
   
  file = DOSFS.open("/EMMPUN.fw", "r");
  if(file)
  {
    Serial.println("File Open!");
  } else
  {
    Serial.println("File Open Failed!");
  }
  
  uint8_t buffer[128];
  uint8_t numbytes, MSadd = 0, totnum = 0;

   Serial.println("writing data to EEPROM");

   for (MSadd = 0; MSadd < 256; MSadd++)
   {
     // MS address byte, 0 to 255
     numbytes = file.read(buffer, 128); // 128 bytes per page, 512 pages
     Serial.print("first two bytes: "); Serial.print("0x"); Serial.print(buffer[0], HEX); Serial.print("0x"); Serial.println(buffer[1], HEX);
     //Serial.print("File Position = "); Serial.println(file.position());  // print number of bytes read
     Serial.print("Number of bytes = "); Serial.println(numbytes);  // print number of bytes read
     M24512DFMwriteBytes(M24512DFM_DATA_ADDRESS, MSadd, 0x00, 128, &buffer[0]); // write data starting at first byte of page MSadd
     delay(50);
     numbytes = file.read(buffer, 128); // 128 bytes per page, 512 pages
     Serial.print("first two bytes: "); Serial.print("0x"); Serial.print(buffer[0], HEX); Serial.print("0x"); Serial.println(buffer[1], HEX);
     //Serial.print("File Position = "); Serial.println(file.position());  // print number of bytes read
     Serial.print("Number of bytes = "); Serial.println(numbytes);  // print number of bytes read
     M24512DFMwriteBytes(M24512DFM_DATA_ADDRESS, MSadd, 128, 128, &buffer[0]); // write data starting at first byte of page MSadd
     digitalWrite(LED_PIN, !digitalRead(LED_PIN));
     delay(50);

     if (numbytes < 128) { break; }
     totnum++;
     digitalWrite(LED_PIN, HIGH);
     Serial.print("totnum"); Serial.println(totnum);
     Serial.print("MSadd 0x"); Serial.println(MSadd, HEX);
   }
  
   // Read first page of EEPROM
   uint8_t data[128];
   M24512DFMreadBytes(M24512DFM_DATA_ADDRESS, 0x00, 0, 128, &data[0]);
   Serial.println("EEPROM first page"); 

   for (int i = 0; i < 16; i++)
   {
     Serial.println(" ");
     
     for (int j = 0; j < 8; j++)
     {
       Serial.print(data[i*8 + j], HEX); Serial.print(" ");
     }
   }
   
   // Read second page of EEPROM
   M24512DFMreadBytes(M24512DFM_DATA_ADDRESS, 0x00, 128, 128, &data[0]);
   Serial.println("");Serial.println("EEPROM second page"); 

   for (int i = 0; i < 16; i++)
   {
     Serial.println(" ");
     
     for (int j = 0; j < 8; j++)
     {
       Serial.print(data[i*8 + j], HEX); Serial.print(" ");
     }
   }
   
   // Read third page of EEPROM
   M24512DFMreadBytes(M24512DFM_DATA_ADDRESS, 0x01, 0, 128, &data[0]);
   Serial.println("");Serial.println("EEPROM third page"); 

   for (int i = 0; i < 16; i++)
   {
     Serial.println(" ");
     for (int j = 0; j < 8; j++)
     {
       Serial.print(data[i*8 + j], HEX); Serial.print(" ");
     }
   }
  
}

void loop()
{
}

// I2C read/write functions for the MPU9250 sensors

        void writeByte(uint8_t address, uint8_t subAddress, uint8_t data) 
        {
        uint8_t temp[2];
        temp[0] = subAddress;
        temp[1] = data;
        Wire.transfer(address, &temp[0], 2, NULL, 0); 
        }

        uint8_t readByte(uint8_t address, uint8_t subAddress) 
        {
        uint8_t temp[1];
        Wire.transfer(address, &subAddress, 1, &temp[0], 1);
        return temp[0];
        }

        void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest) 
        {
        Wire.transfer(address, &subAddress, 1, dest, count); 
        }


void SENtralPassThroughMode()
{
  // First put SENtral in standby mode
  uint8_t c = readByte(EM7180_ADDRESS, EM7180_AlgorithmControl);
  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, c | 0x01);

  // Verify standby status
  Serial.println("SENtral in standby mode");
  
  // Place SENtral in pass-through mode
  writeByte(EM7180_ADDRESS, EM7180_PassThruControl, 0x01); 
  if(readByte(EM7180_ADDRESS, EM7180_PassThruStatus) & 0x01)
  {
    Serial.println("SENtral in pass-through mode");
  } else
  {
    Serial.println("ERROR! SENtral not in pass-through mode!");
  }
}


// I2C communication with the M24512DFM EEPROM is a little different from I2C communication with the usual motion sensor
// since the address is defined by two bytes

void M24512DFMwriteByte(uint8_t device_address, uint8_t data_address1, uint8_t data_address2, uint8_t  data)
{
        uint8_t temp[3] = {data_address1, data_address2, data};
        Wire.transfer(device_address, &temp[0], 3, NULL, 0); 
}


void M24512DFMwriteBytes(uint8_t device_address, uint8_t data_address1, uint8_t data_address2, uint8_t count, uint8_t * dest)
{
   uint8_t temp[2 + count];
   temp[0] = data_address1;
   temp[1] = data_address2;
   for (uint8_t ii = 0; ii < count; ii++)
   { 
      temp[ii + 2] = dest[ii];
   }
   Wire.transfer(device_address, &temp[0], 2 + count, NULL, 0); 
}


uint8_t M24512DFMreadByte(uint8_t device_address, uint8_t data_address1, uint8_t data_address2)
{
        uint8_t data[1] = {0};
        uint8_t temp[2] = {data_address1, data_address2};
        Wire.transfer(device_address, &temp[0], 2, &data[0], 1); 
        return data[0];                             // Return data read from slave register
}

void M24512DFMreadBytes(uint8_t device_address, uint8_t data_address1, uint8_t data_address2, uint8_t count, uint8_t * dest)
{  
        uint8_t temp[2] = {data_address1, data_address2};     
        Wire.transfer(device_address, &temp[0], 2, &dest[0], count); 
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
