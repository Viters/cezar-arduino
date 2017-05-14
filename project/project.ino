#include "keyboard.h"

const int strobe = 7;
const int data = 8;
const int clock = 9;

void send_command(byte value)
{
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);

}

void display_on_screen(byte position, byte value) 
{
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, position);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

void reset_display()
{
  for (byte i = 0; i < 16; i++)
  {
    display_on_screen(0xc0 + i, 0x00);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);

  send_command(0x8f);  // activate and set brightness to max
  reset_display();
}

boolean is_hacking = true;
byte code[8] = {CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1};
short delay_timer = 100;
byte times_to_disp = 2;
byte repeat = 0;
byte display_iter = 0;
byte char_iter = 48;

void recv_with_end_maker() {
  byte current_iter = 0;
  char end_maker = '\n';
  char rc;
  byte should_map = 0;
  byte current_command;
  byte received_data[4];

  while (Serial.available() > 0) {
    rc = Serial.read();
    
    if (rc == end_maker) {
      break;
    }

    if (current_iter == 0) {
      current_command = rc;
    }

    if (current_command == 'C') {
      should_map = 1;
      display_iter = 0;
      repeat = 0;
      char_iter = 48;
    }

    if (should_map == 1 && current_iter > 0) {
      code[current_iter - 1] = map_input_to_arduino(rc);
    } else {
      received_data[current_iter - 1] = rc - '0';
    }
    
    ++current_iter;
  }

  if (current_command == 'D') {
    delay_timer = received_data[0] * 1000 + 
                  received_data[1] * 100 +
                  received_data[2] * 10 +
                  received_data[3];
  }

  if (current_command == 'N') {
    times_to_disp = received_data[0] * 10 + 
                    received_data[1];
  }
}

void read_buttons()
{
  byte buttons = 0;
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0x42);

  pinMode(data, INPUT);

  for (byte i = 0; i < 4; i++)
  {
    byte v = shiftIn(data, clock, LSBFIRST) << i;
    buttons |= v;
  }

  pinMode(data, OUTPUT);
  digitalWrite(strobe, HIGH);

  if (buttons == 1) {
    is_hacking = !is_hacking;
  }

  if (buttons == 2 && is_hacking) {
    display_iter = 0;
    char_iter = 48;
    repeat = 0;
  }

  //Serial.println(buttons); //debug
}

void loop()
{
  send_command(0x44);  // set single address

  recv_with_end_maker();
  read_buttons();

  if (display_iter == 0) {
    reset_display();
  }

  if (display_iter < 8 && is_hacking) {

    if (char_iter == 58) {
      char_iter = 65;
    }

    display_on_screen(DISPLAYS[display_iter], map_input_to_arduino(char_iter));

    ++repeat;

    if (repeat >= times_to_disp * 16 - 16) {
      if (map_input_to_arduino(char_iter) == code[display_iter]) {
        display_on_screen(DISPLAYS[display_iter] + 1, 1);

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
