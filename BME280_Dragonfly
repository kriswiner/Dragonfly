/* BME280 Basic Example Code using Dragonfly
 *  
 by: Kris Winer
 date: April 29, 2016
 license: Beerware - Use this code however you'd like. If you 
 find it useful you can buy me a beer some time.
 
 This sketch uses SDA/SCL on pins 42/43 (back pads), respectively, and it uses the Dragonfly STM32L476RE Breakout Board.
 The BME280 is a simple but high resolution pressure/humidity/temperature sensor, which can be used in its high resolution
 mode but with power consumption of 20 microAmp, or in a lower resolution mode with power consumption of
 only 1 microAmp. The choice will depend on the application.
 
 SDA and SCL have 4K7 pull-up resistors (to 3.3V).
 
 Hardware setup:
 SDA ----------------------- 42 or 20
 SCL ----------------------- 43 or 21
 
  */
  
#include <Wire.h>  
#define Serial SerialUSB // might not be needed in future

// BME280 registers
#define BME280_HUM_LSB    0xFE
#define BME280_HUM_MSB    0xFD
#define BME280_TEMP_XLSB  0xFC
#define BME280_TEMP_LSB   0xFB
#define BME280_TEMP_MSB   0xFA
#define BME280_PRESS_XLSB 0xF9
#define BME280_PRESS_LSB  0xF8
#define BME280_PRESS_MSB  0xF7
#define BME280_CONFIG     0xF5
#define BME280_CTRL_MEAS  0xF4
#define BME280_STATUS     0xF3
#define BME280_CTRL_HUM   0xF2
#define BME280_RESET      0xE0
#define BME280_ID         0xD0  // should be 0x60
#define BME280_CALIB00    0x88
#define BME280_CALIB26    0xE1

#define BME280_ADDRESS           0x76   // Address of BMP280 altimeter when ADO = 0

#define myLed       13 // green led

enum Posr {
  P_OSR_00 = 0,  // no op
  P_OSR_01,
  P_OSR_02,
  P_OSR_04,
  P_OSR_08,
  P_OSR_16
};

enum Hosr {
  H_OSR_00 = 0,  // no op
  H_OSR_01,
  H_OSR_02,
  H_OSR_04,
  H_OSR_08,
  H_OSR_16
};

enum Tosr {
  T_OSR_00 = 0,  // no op
  T_OSR_01,
  T_OSR_02,
  T_OSR_04,
  T_OSR_08,
  T_OSR_16
};

enum IIRFilter {
  full = 0,  // bandwidth at full sample rate
  BW0_223ODR,
  BW0_092ODR,
  BW0_042ODR,
  BW0_021ODR // bandwidth at 0.021 x sample rate
};

enum Mode {
  BME280Sleep = 0,
  forced,
  forced2,
  normal
};

enum SBy {
  t_00_5ms = 0,
  t_62_5ms,
  t_125ms,
  t_250ms,
  t_500ms,
  t_1000ms,
  t_10ms,
  t_20ms,
};

// Specify BME280 configuration
uint8_t Posr = P_OSR_16, Hosr = H_OSR_16, Tosr = T_OSR_02, Mode = normal, IIRFilter = BW0_042ODR, SBy = t_62_5ms;     // set pressure amd temperature output data rate
// t_fine carries fine temperature as global value for BME280
int32_t t_fine;

float Temperature, Pressure, Humidity; // stores BME280 pressures sensor pressure and temperature
uint32_t rawPress, rawTemp;            // pressure and temperature raw count output for BME280
uint16_t rawHumidity;                  // variables to hold raw BME280 humidity value

// BME280 compensation parameters
uint8_t  dig_H1, dig_H3, dig_H6;
uint16_t dig_T1, dig_P1, dig_H4, dig_H5;
int16_t  dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9, dig_H2;

float   temperature_C, temperature_F, pressure, humidity, altitude; // Scaled output of the BME280

uint32_t delt_t = 0, count = 0, sumCount = 0, slpcnt = 0;  // used to control display output rate

void setup()
{
  Serial.begin(115200);
  delay(1000);
  
  Wire.begin(0, 400000, true); // set master mode, I2C frequncy at 400 kHz
   
//  I2Cscan(); // should detect BME280 at 0x76

  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, LOW); // start with the led on since active LOW
  
  // Read the WHO_AM_I register of the BME280 this is a good test of communication
  byte f = readByte(BME280_ADDRESS, BME280_ID);  // Read WHO_AM_I register for BME280
  Serial.print("BME280 "); 
  Serial.print("I AM "); 
  Serial.print(f, HEX); 
  Serial.print(" I should be "); 
  Serial.println(0x60, HEX);
  Serial.println(" ");
  
  delay(1000); 

  if(f == 0x60) {
   
  writeByte(BME280_ADDRESS, BME280_RESET, 0xB6); // reset BME280 before initilization
  delay(100);

  BME280Init(); // Initialize BME280 altimeter
  Serial.println("Calibration coeficients:");
  Serial.print("dig_T1 ="); 
  Serial.println(dig_T1);
  Serial.print("dig_T2 ="); 
  Serial.println(dig_T2);
  Serial.print("dig_T3 ="); 
  Serial.println(dig_T3);
  Serial.print("dig_P1 ="); 
  Serial.println(dig_P1);
  Serial.print("dig_P2 ="); 
  Serial.println(dig_P2);
  Serial.print("dig_P3 ="); 
  Serial.println(dig_P3);
  Serial.print("dig_P4 ="); 
  Serial.println(dig_P4);
  Serial.print("dig_P5 ="); 
  Serial.println(dig_P5);
  Serial.print("dig_P6 ="); 
  Serial.println(dig_P6);
  Serial.print("dig_P7 ="); 
  Serial.println(dig_P7);
  Serial.print("dig_P8 ="); 
  Serial.println(dig_P8);
  Serial.print("dig_P9 ="); 
  Serial.println(dig_P9);
  Serial.print("dig_H1 ="); 
  Serial.println(dig_H1);
  Serial.print("dig_H2 ="); 
  Serial.println(dig_H2);
  Serial.print("dig_H3 ="); 
  Serial.println(dig_H3);
  Serial.print("dig_H4 ="); 
  Serial.println(dig_H4);
  Serial.print("dig_H5 ="); 
  Serial.println(dig_H5);
  Serial.print("dig_H6 ="); 
  Serial.println(dig_H6);
 }
 
  else Serial.println(" BME280 not functioning!");
  
}
 

void loop()
{  
   // Serial print and/or display at 1 s rate independent of data rates
    delt_t = millis() - count;
    if (delt_t > 1000) { // update LCD once per second independent of read rate

    rawPress =  readBME280Pressure();
    pressure = (float) BME280_compensate_P(rawPress)/25600.; // Pressure in mbar
    altitude = 145366.45f*(1.0f - powf((pressure/1013.25f), 0.190284f));
    
    rawTemp =   readBME280Temperature();
    temperature_C = (float) BME280_compensate_T(rawTemp)/100.;
    
    rawHumidity =   readBME280Humidity();
    humidity = (float) BME280_compensate_H(rawHumidity)/1024.;
 
      Serial.println("BME280:");
      Serial.print("Altimeter temperature = "); 
      Serial.print( temperature_C, 2); 
      Serial.println(" C"); // temperature in degrees Celsius
      Serial.print("Altimeter temperature = "); 
      Serial.print(9.*temperature_C/5. + 32., 2); 
      Serial.println(" F"); // temperature in degrees Fahrenheit
      Serial.print("Altimeter pressure = "); 
      Serial.print(pressure, 2);  
      Serial.println(" mbar");// pressure in millibar
      Serial.print("Altitude = "); 
      Serial.print(altitude, 2); 
      Serial.println(" feet");
      Serial.print("Altimeter humidity = "); 
      Serial.print(humidity, 1);  
      Serial.println(" %RH");// pressure in millibar
      Serial.println(" ");
      
      
      digitalWrite(myLed, !digitalRead(myLed)); // toggle red led
      count = millis(); 
    }
    
}

//===================================================================================================================
//====== Set of useful function to access acceleration. gyroscope, magnetometer, and temperature data
//===================================================================================================================
uint32_t readBME280Temperature()
{
  uint8_t rawData[3];  // 20-bit pressure register data stored here
  readBytes(BME280_ADDRESS, BME280_TEMP_MSB, 3, &rawData[0]);  
  return (uint32_t) (((uint32_t) rawData[0] << 16 | (uint32_t) rawData[1] << 8 | rawData[2]) >> 4);
}

uint32_t readBME280Pressure()
{
  uint8_t rawData[3];  // 20-bit pressure register data stored here
  readBytes(BME280_ADDRESS, BME280_PRESS_MSB, 3, &rawData[0]);  
  return (uint32_t) (((uint32_t) rawData[0] << 16 | (uint32_t) rawData[1] << 8 | rawData[2]) >> 4);
}

uint16_t readBME280Humidity()
{
  uint8_t rawData[3];  // 20-bit pressure register data stored here
  readBytes(BME280_ADDRESS, BME280_HUM_MSB, 2, &rawData[0]);  
  return (uint16_t) (((uint16_t) rawData[0] << 8 | rawData[1]) );
}

void BME280Init()
{
  // Configure the BME280
  // Set H oversampling rate
  writeByte(BME280_ADDRESS, BME280_CTRL_HUM, 0x07 & Hosr);
  // Set T and P oversampling rates and sensor mode
  writeByte(BME280_ADDRESS, BME280_CTRL_MEAS, Tosr << 5 | Posr << 2 | Mode);
  // Set standby time interval in normal mode and bandwidth
  writeByte(BME280_ADDRESS, BME280_CONFIG, SBy << 5 | IIRFilter << 2);
  // Read and store calibration data
  uint8_t calib[26];
  readBytes(BME280_ADDRESS, BME280_CALIB00, 26, &calib[0]);
  dig_T1 = (uint16_t)(((uint16_t) calib[1] << 8) | calib[0]);
  dig_T2 = ( int16_t)((( int16_t) calib[3] << 8) | calib[2]);
  dig_T3 = ( int16_t)((( int16_t) calib[5] << 8) | calib[4]);
  dig_P1 = (uint16_t)(((uint16_t) calib[7] << 8) | calib[6]);
  dig_P2 = ( int16_t)((( int16_t) calib[9] << 8) | calib[8]);
  dig_P3 = ( int16_t)((( int16_t) calib[11] << 8) | calib[10]);
  dig_P4 = ( int16_t)((( int16_t) calib[13] << 8) | calib[12]);
  dig_P5 = ( int16_t)((( int16_t) calib[15] << 8) | calib[14]);
  dig_P6 = ( int16_t)((( int16_t) calib[17] << 8) | calib[16]);
  dig_P7 = ( int16_t)((( int16_t) calib[19] << 8) | calib[18]);
  dig_P8 = ( int16_t)((( int16_t) calib[21] << 8) | calib[20]);
  dig_P9 = ( int16_t)((( int16_t) calib[23] << 8) | calib[22]);
  dig_H1 = calib[25];
  readBytes(BME280_ADDRESS, BME280_CALIB26, 7, &calib[0]);
  dig_H2 = ( int16_t)((( int16_t) calib[1] << 8) | calib[0]);
  dig_H3 = calib[2];
  dig_H4 = ( int16_t)(((( int16_t) calib[3] << 8) | (0x0F & calib[4]) << 4) >> 4);
  dig_H5 = ( int16_t)(((( int16_t) calib[5] << 8) | (0xF0 & calib[4]) ) >> 4 );
  dig_H6 = calib[6];
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of
// “5123” equals 51.23 DegC.
int32_t BME280_compensate_T(int32_t adc_T)
{
  int32_t var1, var2, T;
  var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8
//fractional bits).
//Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t BME280_compensate_P(int32_t adc_P)
{
  long long var1, var2, p;
  var1 = ((long long)t_fine) - 128000;
  var2 = var1 * var1 * (long long)dig_P6;
  var2 = var2 + ((var1*(long long)dig_P5)<<17);
  var2 = var2 + (((long long)dig_P4)<<35);
  var1 = ((var1 * var1 * (long long)dig_P3)>>8) + ((var1 * (long long)dig_P2)<<12);
  var1 = (((((long long)1)<<47)+var1))*((long long)dig_P1)>>33;
  if(var1 == 0)
  {
    return 0;
    // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125)/var1;
  var1 = (((long long)dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((long long)dig_P8) * p)>> 19;
  p = ((p + var1 + var2) >> 8) + (((long long)dig_P7)<<4);
  return (uint32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22integer and 10fractional bits).
// Output value of “47445”represents 47445/1024= 46.333%RH
uint32_t BME280_compensate_H(int32_t adc_H)
{
int32_t var;

var = (t_fine - ((int32_t)76800));
var = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * var)) +
((int32_t)16384)) >> 15) * (((((((var * ((int32_t)dig_H6)) >> 10) * (((var *
((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
var = (var - (((((var >> 15) * (var >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
var = (var < 0 ? 0 : var); 
var = (var > 419430400 ? 419430400 : var);
return(uint32_t)(var >> 12);
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


// I2C read/write functions for the BMP280 sensors

        void writeByte(uint8_t address, uint8_t subAddress, uint8_t data) {
        uint8_t temp[2];
        temp[0] = subAddress;
        temp[1] = data;
        Wire.transfer(address, &temp[0], 2, NULL, 0); 
        }

        uint8_t readByte(uint8_t address, uint8_t subAddress) {
        uint8_t temp[1];
        Wire.transfer(address, &subAddress, 1, &temp[0], 1);
        return temp[0];
        }

        void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest) {
        Wire.transfer(address, &subAddress, 1, dest, count); 
        }
