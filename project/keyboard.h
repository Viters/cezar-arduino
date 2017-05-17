const byte CHAR_0 = 0x3f;
const byte CHAR_1 = 0x6;
const byte CHAR_2 = 0x5b;
const byte CHAR_3 = 0x4f;
const byte CHAR_4 = 0x66;
const byte CHAR_5 = 0x6d;
const byte CHAR_6 = 0x7d;
const byte CHAR_7 = 0x7;
const byte CHAR_8 = 0x7f;
const byte CHAR_9 = 0x6f;
const byte CHAR_A = 0x77;
const byte CHAR_b = 0x7c;
const byte CHAR_C = 0x39;
const byte CHAR_d = 0x5e;
const byte CHAR_E = 0x79;
const byte CHAR_F = 0x71;

const byte map_input_to_arduino(char input) {
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
