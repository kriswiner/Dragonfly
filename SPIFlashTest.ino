/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Pete (El Supremo), el_supremo@shaw.ca
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Read/Write W25Q128FV SPI flash (128Mbit = 16MBytes)
// 
// Pete (El Supremo)

/*
NOTES
  - this assumes that the page number referenced in page_number
    has been erased. If it hasn't, the read_back will be incorrect.
  - do not panic when the serial monitor outputs a whole series
of 255 at the end. This correct! scroll back to the beginning of the
output. It shoud like this:

Status = 0
EF, 40, 18, 0, 

Write Page 0xFFFF
time (us) = 814

Read Page 0xFFFF
time (us) = 429
0
1
2
3
4
5
6
.
.
.
etc.

It reads and prints the chip's status and then its ID.
Then it writes 0,1,2,3,...,254,255 into a 256 bytes buffer and writes
that into page_number on the chip. It then reads that back, so that
the output should be 0,1,2,3,...,254,255 (one per line). It then
erases the sector containing that page and times the operation.
You should see something like this:

Erase sector
time (us) = 90315

Read Page 0xFFFF
time (us) = 424
255
255
255
.
.
.

It should report that everything is 255 because the sector
(and the page) have been erased.


140426
  c
  - Change name to flash_test for release
  
  b
  - fix references to flash_read_page to flash_read_pages
    and flash_read_fast_page to flash_read_pages (the library
    now uses the fast method all the time).
  - remove last segment which does another read fast - obsolete.
  
140402
  a
  - read the ID
  - write 256 bytes to a page and then read them back.
  - erase the sector and read the page again - works
  
  On Teensy 3 there is/was a problem with SPI
  See: http://forum.pjrc.com/threads/7353-Bug-with-SPI
*/
// Adapted for S225FL127 by f.boesing

#include <SPI.h>

// Highest page number is 0xffff=65535
int page_number = 0xFFFF;
unsigned char w_page[256];
unsigned char r_page[256];

static const uint8_t  f_cs = 10; //chip select 

#define STAT_WIP 1
#define STAT_WEL 2

#define CMD_WRITE_STATUS_REG   0x01
#define CMD_PAGE_PROGRAM       0x02
#define CMD_READ_DATA          0x03
#define CMD_WRITE_DISABLE      0x04//not tested
#define CMD_READ_STATUS_REG    0x05
#define CMD_WRITE_ENABLE       0x06
#define CMD_READ_HIGH_SPEED    0x0B//not tested
#define CMD_SECTOR_ERASE       0x20//not tested
#define CMD_BLOCK32K_ERASE     0x52//not tested
#define CMD_RESET_DEVICE       0xF0//<<-different from winbond
#define CMD_READ_ID            0x9F
#define CMD_RELEASE_POWER_DOWN 0xAB//not tested
#define CMD_POWER_DOWN         0xB9//not tested
#define CMD_CHIP_ERASE         0xC7
#define CMD_BLOCK64K_ERASE     0xD8//not tested


//=====================================
// flash_wait_for_write is set (non_zero) after any write
// and all functions use write_pause to check that Write-In-Progress
// is clear before proceeding.
// The EXCEPTIONS are:
//    - flash_init
//    - the flash_read_status function (which would cause a recursive call)
// Note that the hard reset function does do a write_pause() first, but it
//   does its own wait when the reset command has been issued
unsigned char flash_wait_for_write = 0;


void setup(void)
{
  pinMode(f_cs, OUTPUT);

  SPI.begin();
  
  unsigned char id_tab[32];
  unsigned long t_start;
  
  Serial.begin(38400);
  delay(5000);
  Serial.println("Serial enabled!");

  flash_hard_reset();
//>>> Read Status Register 1 and the chip ID
  Serial.print("Status = ");
  Serial.println(flash_read_status(), HEX);
  
  // flash read doesn't work unless preceded by flash_read_status ??
  // Page 24 - a W25Q128FV should return EF, 40, 18, 0,
   
   //FB not tested if its the same for spansion
   
  flash_read_id(id_tab);
  // Just print the first four bytes
  // For now ignore the 16 remaining bytes
  Serial.print("Chip ID = ");
  for(int i = 0;i < 4;i++) {
  Serial.print(id_tab[i], HEX); Serial.print(" ");
  }
  Serial.println("\r\n");

  Serial.println("Macronix MX25L12835FZNI Chip ID = 0xC2 0x20 0x18 0xC2");
  Serial.println("Spansion S25FL127S Chip ID = 0x01, 0x20, 0x18, 0x4D ");
  Serial.println("\r\n");
 
//>>> Initialize the array to 0,1,2,3 etc.
  for(int i = 0;i < 256;i++) {
    w_page[i] = i;
  }
  
//>>> Write the page to page_number - this page MUST be in the
// erased state
  Serial.print("Write page:  0x"); Serial.println(page_number, HEX);  
  t_start = micros();
  flash_page_program(w_page,page_number);
  t_start = micros() - t_start;
  Serial.print("time (us) = "); Serial.println(t_start);

//>>> Read back page_number and print its contents which
// should be 0,1,2,3...
  Serial.print("Read Page 0x"); Serial.println(page_number, HEX);
  t_start = micros();
  flash_read_pages(r_page,page_number,1);
  t_start = micros() - t_start;
  Serial.print("time (us) = "); Serial.println(t_start);
  
  for(int i = 0;i < 256;i++) {
    Serial.print(" 0x"); Serial.print(r_page[i], HEX);
	if (i % 16==0) Serial.println();
  }
  Serial.println("");
  
//>>> Erase the sector which includes page_number
  Serial.println("Erase chip (~40 secs)");
  t_start = millis();
  flash_chip_erase(true);
  t_start = millis() - t_start;
  Serial.print("time (ms) = "); Serial.println(t_start);

//>>> Now read back the page. It should now be all 255.
  Serial.print( "Read Page 0x"); Serial.println(page_number, HEX);
  t_start = micros();
  flash_read_pages(r_page,page_number,1);
  t_start = micros() - t_start;

  Serial.print("time (us) = "); Serial.println(t_start);
  for(int i = 0;i < 256;i++) {
    Serial.print(" 0x"); Serial.print(r_page[i], HEX);
	if (i % 16==0) Serial.println();
  }
  Serial.println("");

}

void loop(void)
{

}



void write_pause(void)
{
  if(flash_wait_for_write) {
    while(flash_read_status() & STAT_WIP);
    flash_wait_for_write = 0;
  }
}

//=====================================
// convert a page number to a 24-bit address
int page_to_address(int pn)
{
  return(pn << 8);
}

//=====================================
// convert a 24-bit address to a page number
int address_to_page(int addr)
{
  return(addr >> 8);
}

//=====================================
void flash_read_id(unsigned char *idt)
{
  write_pause();
  //set control register 
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_READ_ID);
  for(int i = 0;i < 20;i++) {
    *idt++ = SPI.transfer(0x00);
  }
  digitalWrite(f_cs, HIGH);
}

//=====================================
unsigned char flash_read_status(void)
{
  unsigned char c;

// This can't do a write_pause
  digitalWrite(f_cs, LOW);  
  SPI.transfer(CMD_READ_STATUS_REG);
  c = SPI.transfer(0x00);
  digitalWrite(f_cs, HIGH);
  return(c);
}

//=====================================

void flash_hard_reset(void)
{

  // Make sure that the device is not busy before
  // doing the hard reset sequence
  // At the moment this does NOT check the
  // SUSpend status bit in Status Register 2
  // but the library does not support suspend
  // mode yet anyway
  write_pause();
  
  // Send Write Enable command
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_RESET_DEVICE );
  digitalWrite(f_cs, HIGH);
  delayMicroseconds(50);
  // Wait for the hard reset to finish
  // Don't use flash_wait_for_write here
  while(flash_read_status() & STAT_WIP);
  // The spec says "the device will take
  // approximately tRST=30 microseconds
  // to reset"
  // Give it a bit more time to be sure
//  delayMicroseconds(50);
}

//=====================================
// Tbe Typ=13sec  Max=40sec
void flash_chip_erase(boolean wait)
{
  write_pause();
  // Send Write Enable command
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_WRITE_ENABLE);
  digitalWrite(f_cs, HIGH);
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_CHIP_ERASE);
  digitalWrite(f_cs, HIGH);
  flash_wait_for_write = 1;
  if(wait)write_pause();
}

//=====================================
// Tse Typ=0.6sec Max=3sec
// measured 549.024ms
// Erase the sector which contains the specified
// page number.
// The smallest unit of memory which can be erased
// is the 4kB sector (which is 16 pages)
void flash_erase_pages_sector(int pn)
{
  int address;

  write_pause(); 
  // Send Write Enable command
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_WRITE_ENABLE);
  digitalWrite(f_cs, HIGH);
  
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_SECTOR_ERASE);
  // Send the 3 byte address
  address = page_to_address(pn);
  SPI.transfer((address >> 16) & 0xff);
  SPI.transfer((address >> 8) & 0xff);
  SPI.transfer(address & 0xff);
  digitalWrite(f_cs, HIGH);
  
  // Indicate that next I/O must wait for this write to finish
  flash_wait_for_write = 1;
}

//=====================================
// Erase the 32kb block which contains the specified
// page number.
void flash_erase_pages_block32k(int pn)
{
  int address;

  write_pause();
  // Send Write Enable command
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_WRITE_ENABLE);
  digitalWrite(f_cs, HIGH);
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_BLOCK32K_ERASE);
  // Send the 3 byte address
  address = page_to_address(pn);
  SPI.transfer((address >> 16) & 0xff);
  SPI.transfer((address >> 8) & 0xff);
  SPI.transfer(address & 0xff);
  digitalWrite(f_cs, HIGH);
  // Indicate that next I/O must wait for this write to finish
  flash_wait_for_write = 1;
}

//=====================================
// Erase the 64kb block which contains the specified
// page number.
void flash_erase_pages_block64k(int pn)
{
  int address;
  
  write_pause();
  // Send Write Enable command
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_WRITE_ENABLE);
  digitalWrite(f_cs, HIGH);
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_BLOCK64K_ERASE);
  // Send the 3 byte address
  address = page_to_address(pn);
  SPI.transfer((address >> 16) & 0xff);
  SPI.transfer((address >> 8) & 0xff);
  SPI.transfer(address & 0xff);
  digitalWrite(f_cs, HIGH);
  // Indicate that next I/O must wait for this write to finish
  flash_wait_for_write = 1;
}

//=====================================
// Tpp Typ=0.64ms Max=5ms
// measured 1667us
void flash_page_program(unsigned char *wp,int pn)
{
  int address;

  write_pause(); 
  // Send Write Enable command
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_WRITE_ENABLE);
  digitalWrite(f_cs, HIGH);
  
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_PAGE_PROGRAM);
  // Send the 3 byte address
  address = page_to_address(pn);
  SPI.transfer((address >> 16) & 0xff);
  SPI.transfer((address >> 8) & 0xff);
  SPI.transfer(address & 0xff);
  // Now write 256 bytes to the page
  for(int i = 0;i < 256;i++) {
    SPI.transfer(*wp++);
  }
  digitalWrite(f_cs, HIGH);
  // Indicate that next I/O must wait for this write to finish
  flash_wait_for_write = 1;
}

//=====================================
// measured = 664us
void flash_read_pages(unsigned char *p,int pn,const int n_pages)
{
  int address;
  unsigned char *rp = p;
  
  write_pause();
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_READ_DATA);
  // Send the 3 byte address
  address = page_to_address(pn);
  SPI.transfer((address >> 16) & 0xff);
  SPI.transfer((address >> 8) & 0xff);
  SPI.transfer(address & 0xff);
  // Now read the page's data bytes
  for(int i = 0;i < n_pages * 256;i++) {
    *rp++ = SPI.transfer(0);
  }
  digitalWrite(f_cs, HIGH);
}

//=====================================
// measured = 442us for one page
// Read specified number of pages starting with pn
void flash_fast_read_pages(unsigned char *p,int pn,const int n_pages)
{
  int address;
  unsigned char *rp = p;
  
  write_pause();
// The chip doesn't run at the higher clock speed until
// after the command and address have been sent
  digitalWrite(f_cs, LOW);
  SPI.transfer(CMD_READ_HIGH_SPEED);
  // Send the 3 byte address
  address = page_to_address(pn);
  SPI.transfer((address >> 16) & 0xff);
  SPI.transfer((address >> 8) & 0xff);
  SPI.transfer(address & 0xff);
  // send dummy byte
  SPI.transfer(0);
  // Double the clock speed
//>>>PAH use DIV2 all the time so don't need this
//  SPI.setClockDivider(SPI_CLOCK_DIV2);
  // Now read the number of pages required
  for(int i = 0;i < n_pages * 256;i++) {
    *rp++ = SPI.transfer(0);
  }
  digitalWrite(f_cs, HIGH);
  // reset the clock speed
//>>>PAH use DIV2 all the time so don't need this
//  SPI.setClockDivider(SPI_CLOCK_DIV4);
}
