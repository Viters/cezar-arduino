#include "keyboard.h"
#include "variables.h"

const byte strobe = 7;
const byte data = 8;
const byte clock = 9;

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

  send_command(0x8f);
  reset_display();
}


void reset_hacking_progress() {
  display_iter = 0;
  repeat = 0;
  char_iter = 48;
}

void recv_with_end_maker() {
  byte current_iter = 0;
  char rc;
  byte should_map = 0;
  byte current_command;
  byte received_data[4];

  while (Serial.available() > 0) {
    rc = Serial.read();
    
    if (rc == '\n') {
      break;
    }

    if (current_iter == 0) {
      current_command = rc;
    }

    if (current_command == 'C') {
      should_map = 1;
      reset_hacking_progress();
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
  byte button_code = 0;
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0x42);

  pinMode(data, INPUT);

  for (byte i = 0; i < 4; i++)
  {
    byte v = shiftIn(data, clock, LSBFIRST) << i;
    button_code |= v;
  }

  pinMode(data, OUTPUT);
  digitalWrite(strobe, HIGH);

  if (button_code == 1) {
    hacking_in_progress = !hacking_in_progress;
    delay(300);
  }

  if (button_code == 2 && hacking_in_progress) {
    reset_hacking_progress();
  }
}

void loop()
{
  send_command(0x44);

  


  if (display_iter == 0) {
    reset_display();
  }

  if (display_iter < 8 && hacking_in_progress) {
    if (char_iter == 58) {
      char_iter = 65;
    }

    display_on_screen(0xc0 + display_iter * 2, map_input_to_arduino(char_iter));

    ++repeat;

    if (repeat >= times_to_disp * 16 - 16) {
      if (map_input_to_arduino(char_iter) == code[display_iter]) {
        display_on_screen(0xc0 + display_iter * 2 + 1, 1);

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
  
  for (int i = 0; i < delay_timer / 10; ++i) {
    recv_with_end_maker();
    read_buttons();
    delay(10);    
  }

}
