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

boolean is_hacking = true;
uint8_t code[8] = {CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1};
uint8_t received_chars[9];
short delay_timer = 100;
short times_to_disp = 2;
uint8_t repeat = 0;
uint8_t display_iter = 0;
uint8_t char_iter = 48;

void recv_with_end_maker() {
  static uint8_t current_iter = 0;
  char end_maker = '\n';
  char rc;
  uint8_t num_chars;
  uint8_t should_map;  
 
  while (Serial.available() > 0) {
    rc = Serial.read();

    if (rc == 'C' && current_iter == 0) {
      num_chars = 9;
      should_map = 1;
      display_iter = 0;
    }

    if (rc == 'D' && current_iter == 0) {
      num_chars = 5;
      should_map = 0;
    }

    if (rc == 'N' && current_iter == 0) {
      num_chars = 3;
      should_map = 0;
    }

    if (rc != end_maker) {
      if (should_map == 1 && current_iter > 0) {
        code[current_iter - 1] = map_input_to_arduino(rc);
      } else {
        received_chars[current_iter] = rc - '0';
      }
      ++current_iter;
    }

    if (current_iter == num_chars) {
      current_iter = 0;
    }
  }

  if (received_chars[0] == 'D' - '0') {
    delay_timer = received_chars[1] * 1000 + received_chars[2] * 100 + received_chars[3] * 10 + received_chars[4];
    received_chars[0] = 0;
  }

  if(received_chars[0] == 'N' - '0'){
    times_to_disp = received_chars[1] * 10 + received_chars[2];
  }
}

void readButtons()
{
  uint8_t buttons = 0;
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0x42);
 
  pinMode(data, INPUT);
 
  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftIn(data, clock, LSBFIRST) << i;
    buttons |= v;
  }
 
  pinMode(data, OUTPUT);
  digitalWrite(strobe, HIGH);

  if(buttons == 1){
    is_hacking = !is_hacking;
  }

  if(buttons == 2 && is_hacking){
    display_iter = 0; 
  }

  Serial.println(buttons);
}

void loop()
{
  sendCommand(0x44);  // set single address
 
  recv_with_end_maker();
  readButtons();

  if (display_iter == 0) {
    for (int i = 0; i < 16; i++) {
      digitalWrite(strobe, LOW);
      shiftOut(data, clock, LSBFIRST, DISPLAYS[0] + i);
      shiftOut(data, clock, LSBFIRST, 0);
      digitalWrite(strobe, HIGH);
    }
  }

  if (display_iter < 8 && is_hacking /*&& received_chars[0] == 'C' - '0'*/) {  

    if (char_iter == 58) {
      char_iter = 65;
    }

    digitalWrite(strobe, LOW);
    shiftOut(data, clock, LSBFIRST, DISPLAYS[display_iter]);
    shiftOut(data, clock, LSBFIRST, map_input_to_arduino(char_iter));
    digitalWrite(strobe, HIGH);

    ++repeat;

    if (repeat >= times_to_disp * 16 - 16) {
      if (map_input_to_arduino(char_iter) == code[display_iter]) {
        digitalWrite(strobe, LOW);
        shiftOut(data, clock, LSBFIRST, DISPLAYS[display_iter] + 1);
        shiftOut(data, clock, LSBFIRST, 1);
        digitalWrite(strobe, HIGH);
        ++display_iter;   
        repeat = 0;
        char_iter = 48;
      }
    }

    ++char_iter;

    if (char_iter == 71) {
      char_iter = 48;
    }

  }
  
  delay(delay_timer);
}
