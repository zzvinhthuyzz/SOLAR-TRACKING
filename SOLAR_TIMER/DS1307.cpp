# include "Arduino.h"
#include "Wire.h"
#include "DS1307.h"
#define I2C_ADDR 0x68
DS1307::DS1307(void)
{
  return;
}
void DS1307::begin(void)
{
  Wire.begin();
}
void DS1307::start(void)
{
  uint8_t sec;
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 1);
  sec = Wire.read();
  Wire.read();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.write(sec & 0x7F);
  Wire.endTransmission();

  return;
}
void DS1307::stop(void)
{
  uint8_t sec, d;

  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 1);
  sec = Wire.read();
  d   = Wire.read();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.write(sec | 0x80); 
  Wire.endTransmission();
  return;
}
void DS1307::get(uint8_t *day, uint8_t *month, uint16_t *year,uint8_t *hour,uint8_t *min,uint8_t *sec)
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 7);
  *sec   = bcd2bin(Wire.read() & 0x7F);
  *min   = bcd2bin(Wire.read());
  *hour  = bcd2bin(Wire.read());
           bcd2bin(Wire.read()); //day of week
  *day   = bcd2bin(Wire.read());
  *month = bcd2bin(Wire.read());
  *year  = bcd2bin(Wire.read()) + 2000;
  return;
}
void DS1307::get(int *day, int *month, int *year,int *hour, int *min,int *sec )
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 7);
  *sec   = bcd2bin(Wire.read() & 0x7F);
  *min   = bcd2bin(Wire.read());
  *hour  = bcd2bin(Wire.read());
           bcd2bin(Wire.read()); //day of week
  *day   = bcd2bin(Wire.read());
  *month = bcd2bin(Wire.read());
  *year  = bcd2bin(Wire.read()) + 2000;
  return;
}


void DS1307::set(uint8_t day, uint8_t month, uint16_t year,uint8_t hour,uint8_t min,uint8_t sec)
{
  uint8_t ch, d;
  if(year >= 2000)
  {
    year -= 2000;
  }
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 1);
  ch = Wire.read();
  d  = Wire.read(); 
  if(ch & 0x80) //CH set?
  {
    ch = 0x80;
  }
  else
  {
    ch = 0x00;
  }
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(byte(0x00));
  Wire.write(bin2bcd(sec) | ch);
  Wire.write(bin2bcd(min));
  Wire.write(bin2bcd(hour));
  Wire.write(bin2bcd(0));
  Wire.write(bin2bcd(day));
  Wire.write(bin2bcd(month));
  Wire.write(bin2bcd(year));
  Wire.endTransmission();
  return;
}

void DS1307::set(int day, int month, int year,int hour, int min,int sec )
{
  return set((uint8_t)day, (uint8_t)month, (uint16_t)year, (uint8_t)hour,(uint8_t)min,(uint8_t)sec);
}
uint8_t DS1307::bin2bcd(uint8_t val)
{
  return val + 6 * (val / 10);
}
uint8_t DS1307::bcd2bin(uint8_t val)
{
  return val - 6 * (val >> 4);
}
