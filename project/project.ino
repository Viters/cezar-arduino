#include "keyboard.h"
#include "globals.h"
#include "arduino.h"

void setup()
{
  Serial.begin(9600);
  pinMode(STROBE, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  send_command(0x8f);
  reset_display();
}

void reset_hacking_progress() 
{
  SEGMENT_ITER = 0;
  REPEAT_ITER = 0;
  CHAR_ITER = 48;
  reset_display();
}

void read_from_serial_port() 
{
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
    set_one_char_display_length(nums_from_input);    
  }

  if (current_command == 'N') {
    set_one_char_repeat_times(nums_from_input);
  }
}

void set_one_char_display_length(byte nums[])
{
  ONE_CHAR_DISP = nums[0] * 1000 + 
                  nums[1] * 100 +
                  nums[2] * 10 +
                  nums[3];
}

void set_one_char_repeat_times(byte nums[])
{
  ONE_CHAR_TIMES = nums[0] * 10 + 
                   nums[1];
}

void read_buttons()
{
  byte button_code = check_buttons_status();

  if (button_code == 1) {
    HACKING_IN_PROGRESS = !HACKING_IN_PROGRESS;
    delay(300);
  }

  if (button_code == 2 && SEGMENT_ITER >= 8) {
    reset_hacking_progress();
    delay(300);
  }
}

void loop()
{
  boolean should_update = if_should_update();
  
  display_currently_processed_char();
  read_from_serial_port();
  read_buttons();

  light_led_if_current_char_is_valid();

  if (should_update && is_hacking_in_progress()) {
    ++REPEAT_ITER;
    ++CHAR_ITER;
    shift_char_iter_if_needed(); 
  }
}

boolean if_should_update()
{
  unsigned long current_milis = millis();
  boolean should_update = false;
  if (current_milis - PREVIOUS_MILIS > ONE_CHAR_DISP) {
    PREVIOUS_MILIS = current_milis;
    should_update = true;
  }

  return should_update;
}

void display_currently_processed_char() 
{
  if (is_hacking_in_progress()) {
    display_on_screen(0xc0 + SEGMENT_ITER * 2, map_input_to_arduino(CHAR_ITER));
  }
}

void light_led_if_current_char_is_valid()
{
  if (is_current_iteration_last() && is_current_char_valid()) {
    display_on_screen(0xc0 + SEGMENT_ITER * 2 + 1, 1);

    ++SEGMENT_ITER;
    REPEAT_ITER = 0;
    CHAR_ITER = 48;
  }
}

boolean is_hacking_in_progress() 
{
  return SEGMENT_ITER < 8 && HACKING_IN_PROGRESS;
}

boolean is_current_iteration_last()
{
  return REPEAT_ITER >= ONE_CHAR_TIMES * 16 - 16;
}

boolean is_current_char_valid()
{
  return map_input_to_arduino(CHAR_ITER) == CURRENT_CODE[SEGMENT_ITER];
}

void shift_char_iter_if_needed() 
{
  if (CHAR_ITER == 58) {
    CHAR_ITER = 65;
  }

  if (CHAR_ITER == 71) {
    CHAR_ITER = 48;
  }
}