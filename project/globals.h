// ---------- THIS YOU MAY CHANGE ----------

// Stores currently processed code
volatile byte CURRENT_CODE[8] = {CHAR_b, CHAR_A, CHAR_A, CHAR_d, CHAR_A, CHAR_5, CHAR_5, CHAR_5};

// How long should one char persist (in ms)
volatile short ONE_CHAR_DISP = 300;

// How many times should display iterate over correct char before stopping
volatile byte ONE_CHAR_TIMES = 2;

// ---------- THIS YOU MUST NOT CHANGE ----------

// Determines if device should update it's status
volatile boolean HACKING_IN_PROGRESS = true;

// Stores currently processed segment
byte SEGMENT_ITER = 0;

// Stores currently processed char repetition
byte REPEAT_ITER = 0;

// Stores currently processed char
byte CHAR_ITER = 48;

// Helper variable for calculating time interval
unsigned long PREVIOUS_MILIS = 0;

// Pin numbers connected from Arduino to LED&KEY display
const byte STROBE = 7;
const byte DATA = 8;
const byte CLOCK = 9;