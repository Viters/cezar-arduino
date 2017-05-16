volatile boolean hacking_in_progress = true;
volatile byte code[8] = {CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1, CHAR_1};
volatile short delay_timer = 100;
volatile byte times_to_disp = 2;
byte repeat = 0;
byte display_iter = 0;
byte char_iter = 48;
short current_delay = 0;