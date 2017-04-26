#include "keyboard.h"

const int strobe = 7;
const int data = 8;
const int clock = 9;
 
void sendCommand(uint8_t value)
{
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}
 
void reset()
{
  sendCommand(0x40); // set auto increment mode
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOut(data, clock, LSBFIRST, 0x00);
  }
  digitalWrite(strobe, HIGH);
}
 
void setup()
{
  Serial.begin(9600);
  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
 
  sendCommand(0x8f);  // activate and set brightness to max
  reset();
}

void loop()
{
  sendCommand(0x44);  // set single address
 
  int char_1 = keyboard[random(0, 15)];
  int char_2 = keyboard[random(0, 15)];
  int char_3 = keyboard[random(0, 15)];
  int char_4 = keyboard[random(0, 15)];

  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc4);
  shiftOut(data, clock, LSBFIRST, char_1);
  digitalWrite(strobe, HIGH);

  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc6);
  shiftOut(data, clock, LSBFIRST, char_2);
  digitalWrite(strobe, HIGH);

  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc8);
  shiftOut(data, clock, LSBFIRST, char_3);
  digitalWrite(strobe, HIGH);
 
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xca); // last digit
  shiftOut(data, clock, LSBFIRST, char_4);
  digitalWrite(strobe, HIGH);

  delay(500);
}
