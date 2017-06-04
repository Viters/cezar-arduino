#include "keyboard.h"
#include "globals.h"
#include "arduino.h"
#include "errors.h"

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
  byte current_command;
  byte received_data[9];

  while (Serial.available() > 0) {
    delay(20);
    rc = Serial.read();
    
    if (current_iter == 0) {
      current_command = rc;
    } else if (current_iter < 10) {
      received_data[current_iter - 1] = rc;
    }
    
    if (rc == '\n') {
      break;
    }

    ++current_iter;
  }

  if (current_iter > 0) {
    switch (current_command) {
      case 'C':
        validate_and_process_code(received_data);
        return;
      case 'D':
        validate_and_process_delay(received_data);
        return;
      case 'N':
        validate_and_process_repeat(received_data);
        return;
      default:
        Serial.println(NOT_SUPPORTED_CMD_ERR);
        return;
    }
  }
}

void validate_and_process_code(byte data[])
{
  for (byte i = 0; i < 8; ++i) {
    if (data[i] < '0' || (data[i] > '9' && data[i] < 'A') || (data[i] > 'F' && data[i] < 'a') || data[i] > 'f') {
      Serial.println(WRONG_DATA_IN_CMD_ERR);
      return;
    }
  }

  for (byte i = 0; i < 8; ++i) {
    CURRENT_CODE[i] = map_input_to_arduino(data[i]);
  }

  reset_hacking_progress();
}

void validate_and_process_delay(byte data[])
{
  for (byte i = 0; i < 4; ++i) {
    if (data[i] < '0' || data[i] > '9') {
      Serial.println(WRONG_DATA_IN_CMD_ERR);
      return;
    }
  }
  
  for (byte i = 0; i < 4; ++i) {
    data[i] = data[i] - '0';
  }

  set_one_char_display_length(data);
}

void validate_and_process_repeat(byte data[])
{
  for (byte i = 0; i < 2; ++i) {
    if (data[i] < '0' || data[i] > '9') {
      Serial.println(WRONG_DATA_IN_CMD_ERR);
      return;
    }
  }

  for (byte i = 0; i < 2; ++i) {
    data[i] = data[i] - '0';
  }

  set_one_char_repeat_times(data);
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
  
  mock_other_segments();
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

void mock_other_segments()
{
  if (is_hacking_in_progress()) {
    for (byte i = 2 * SEGMENT_ITER; i < 15; i += 2) {
      display_on_screen(0xc0 + i, map_input_to_arduino(CHAR_ITER));
    }
  }
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