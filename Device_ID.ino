/*
 * Dragonfly has a unique 96-bit identifier which is really 
 * just a sequence of some unique product data in the STM32L476 device memory.
 * 
 * But this can be useful when an application is using more than one Dragonfly
 * to keep track of which signals belongs to which device.
 * 
 */
 /* STM32L476 product identifier registers*/
  #define         DEVICE_ID1          (0x1FFF7590)
  #define         DEVICE_ID2          (0x1FFF7594)
  #define         DEVICE_ID3          (0x1FFF7598)

  uint32_t deviceserial0, deviceserial1, deviceserial2;

void setup() {
  Serial.begin(38400);
  delay(4000);

  deviceserial0 = *(uint32_t*)DEVICE_ID1; // read unique STM32L476 product ID registers
  deviceserial1 = *(uint32_t*)DEVICE_ID2;
  deviceserial2 = *(uint32_t*)DEVICE_ID3;

  Serial.print("Dragonfly Device ID = "); // print Dragonfly deice ID to the serial monitor
  Serial.print(deviceserial0); Serial.print("-"); 
  Serial.print(deviceserial1); Serial.print("-");
  Serial.println(deviceserial2);

}

void loop() {
  // put your main code here, to run repeatedly:

}
