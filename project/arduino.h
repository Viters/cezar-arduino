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
  for (byte i = 0; i < 16; i++) {
    display_on_screen(0xc0 + i, 0x00);
  }
}

byte check_buttons_status()
{
  byte button_code = 0;
  digitalWrite(STROBE, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, 0x42);

  pinMode(DATA, INPUT);

  for (byte i = 0; i < 4; i++) {
    byte v = shiftIn(DATA, CLOCK, LSBFIRST) << i;
    button_code |= v;
  }

  pinMode(DATA, OUTPUT);
  digitalWrite(STROBE, HIGH);

  return button_code;
}