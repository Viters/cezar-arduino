#include "keyboard.h"
#include "globals.h"

void send_command(byte value)
{
  digitalWrite(STROBE, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, value);
  digitalWrite(STROBE, HIGH);
}

void display_on_screen(byte position, byte value) 
{
  send_command(0x44);  
  digitalWrite(STROBE, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, position);
  shiftOut(DATA, CLOCK, LSBFIRST, value);
  digitalWrite(STROBE, HIGH);
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
  pinMode(STROBE, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  send_command(0x8f);
  reset_display();
}


void reset_hacking_progress() {
  SEGMENT_ITER = 0;
  REPEAT_ITER = 0;
  CHAR_ITER = 48;
}

void recv_with_end_maker() {
  byte current_iter = 0;
  byte rc;
  byte should_map = 0;
  byte current_command;
  byte nums_from_input[4];

  while (Serial.available() > 0) {
    delay(20);

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
      CURRENT_CODE[current_iter - 1] = map_input_to_arduino(rc);
    } else {
      nums_from_input[current_iter - 1] = rc - '0';
    }
    
    ++current_iter;
  }

  if (current_command == 'D') {
    ONE_CHAR_DISP = nums_from_input[0] * 1000 + 
                  nums_from_input[1] * 100 +
                  nums_from_input[2] * 10 +
                  nums_from_input[3];
  }

  if (current_command == 'N') {
    ONE_CHAR_TIMES = nums_from_input[0] * 10 + 
                    nums_from_input[1];
  }
}

void read_buttons()
{
  byte button_code = 0;
  digitalWrite(STROBE, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, 0x42);

  pinMode(DATA, INPUT);

  for (byte i = 0; i < 4; i++)
  {
    byte v = shiftIn(DATA, CLOCK, LSBFIRST) << i;
    button_code |= v;
  }

  pinMode(DATA, OUTPUT);
  digitalWrite(STROBE, HIGH);

  if (button_code == 1) {
    HACKING_IN_PROGRESS = !HACKING_IN_PROGRESS;
    delay(300);
  }

  if (button_code == 2 && HACKING_IN_PROGRESS) {
    reset_hacking_progress();
    delay(300);
  }
}

void loop()
{
  on_loop();

  unsigned long current_milis = millis();
  if (current_milis - PREVIOUS_MILIS > ONE_CHAR_DISP) {
    PREVIOUS_MILIS = current_milis;
    on_update();
  }
}

void on_loop() {
  if (SEGMENT_ITER < 8 && HACKING_IN_PROGRESS) {
    display_on_screen(0xc0 + SEGMENT_ITER * 2, map_input_to_arduino(CHAR_ITER));
  }

  recv_with_end_maker();
  read_buttons();
}

void on_update() {
  if (SEGMENT_ITER == 0 && REPEAT_ITER == 0) {
      reset_display();
    }

  if (SEGMENT_ITER < 8 && HACKING_IN_PROGRESS) {
    ++REPEAT_ITER;

    if (REPEAT_ITER >= ONE_CHAR_TIMES * 16 - 16) {
      if (map_input_to_arduino(CHAR_ITER) == CURRENT_CODE[SEGMENT_ITER]) {
        display_on_screen(0xc0 + SEGMENT_ITER * 2 + 1, 1);

        ++SEGMENT_ITER;
        REPEAT_ITER = 0;
        CHAR_ITER = 48;
      }
    }

    ++CHAR_ITER;

    if (CHAR_ITER == 58) {
      CHAR_ITER = 65;
    }

    if (CHAR_ITER == 71) {
      CHAR_ITER = 48;
    }
  }
}