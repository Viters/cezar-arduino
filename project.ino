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
 
  for (int i = 0; i < 8; ++i) {
    int char_to_display = KEYBOARD[random(0, 15)];
    digitalWrite(strobe, LOW);
    shiftOut(data, clock, LSBFIRST, DISPLAYS[i]);
    shiftOut(data, clock, LSBFIRST, char_to_display);
    digitalWrite(strobe, HIGH);
  }

  delay(200);
}
