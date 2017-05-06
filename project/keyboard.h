#define CHAR_0 0x3f
#define CHAR_1 0x6
#define CHAR_2 0x5b
#define CHAR_3 0x4f
#define CHAR_4 0x66
#define CHAR_5 0x6d
#define CHAR_6 0x7d
#define CHAR_7 0x7
#define CHAR_8 0x7f
#define CHAR_9 0x6f 
#define CHAR_A 0x77
#define CHAR_b 0x7c
#define CHAR_C 0x39
#define CHAR_d 0x5e
#define CHAR_E 0x79
#define CHAR_F 0x71

#define DISP_1 0xc0
#define DISP_2 0xc2
#define DISP_3 0xc4
#define DISP_4 0xc6
#define DISP_5 0xc8
#define DISP_6 0xca
#define DISP_7 0xcc
#define DISP_8 0xce

uint8_t DISPLAYS[] = {DISP_1, DISP_2, DISP_3, DISP_4, DISP_5, DISP_6, DISP_7, DISP_8};

#define LED_1 0xc1
#define LED_2 0xc3
#define LED_3 0xc5
#define LED_4 0xc7
#define LED_5 0xc9
#define LED_6 0xcb
#define LED_7 0xcd
#define LED_8 0xcf

#define BUTTON_1 1
#define BUTTON_2 2

uint8_t map_input_to_arduino(char input) {
  switch(input) {
    case '0':
      return CHAR_0;
    case '1':
      return CHAR_1;
    case '2':
      return CHAR_2;
    case '3':
      return CHAR_3;
    case '4':
      return CHAR_4;
    case '5':
      return CHAR_5;
    case '6':
      return CHAR_6;
    case '7':
      return CHAR_7;
    case '8':
      return CHAR_8;
    case '9':
      return CHAR_9;
    case 'A':
      return CHAR_A;
    case 'b':
    case 'B':
      return CHAR_b;
    case 'C':
      return CHAR_C;
    case 'd':
    case 'D':
      return CHAR_d;
    case 'E':
      return CHAR_E;
    case 'F':
      return CHAR_F;
  }

  return 0;
}