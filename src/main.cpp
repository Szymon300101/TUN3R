#include <Arduino.h>


const int COMMON_CATHODE = 1;
const int COMMON_ANODE   = 2;

// SET YOUR DISPLAY TYPE HERE
const int LED_DISPLAY_TYPE = COMMON_CATHODE;

// Uncomment the following #define if you want to check the 7-segment display
// and 2 signalling LEDs
// Leave it commented out if you want to run the tuner.

//#define LED_TEST

// LED OUTPUT PINS
int LED1 = 7;
int LED2 = 8;

// 9 segment display output pins
int LEDE = 6;
int LEDD = 5;
int LEDC = 3;
int LEDDP = 2;
int LEDB = A2;
int LEDA = A5;
int LEDF = A4;
int LEDG = A3;

// Data storage variables
volatile int impulse_cnt;
unsigned long last_read_time;
int frequency;


// Decode led pattern and switch on/off the leds
void setLeds(String segments) {
    digitalWrite(LED1,  segments[0] ==  '1' ? LOW : HIGH);
    digitalWrite(LED2,  segments[2] ==  '1' ? LOW : HIGH);
  if (LED_DISPLAY_TYPE == COMMON_CATHODE) {
    digitalWrite(LEDE,  segments[3] ==  '0' ? LOW : HIGH);
    digitalWrite(LEDD,  segments[4] ==  '0' ? LOW : HIGH);
    digitalWrite(LEDC,  segments[5] ==  '0' ? LOW : HIGH);
    digitalWrite(LEDDP, segments[6] ==  '0' ? LOW : HIGH);
    digitalWrite(LEDB,  segments[7] ==  '0' ? LOW : HIGH);
    digitalWrite(LEDA,  segments[8] ==  '0' ? LOW : HIGH);
    digitalWrite(LEDF,  segments[9] ==  '0' ? LOW : HIGH);
    digitalWrite(LEDG,  segments[10] == '0' ? LOW : HIGH);
  } else {
    digitalWrite(LEDE,  segments[3] ==  '1' ? LOW : HIGH);
    digitalWrite(LEDD,  segments[4] ==  '1' ? LOW : HIGH);
    digitalWrite(LEDC,  segments[5] ==  '1' ? LOW : HIGH);
    digitalWrite(LEDDP, segments[6] ==  '1' ? LOW : HIGH);
    digitalWrite(LEDB,  segments[7] ==  '1' ? LOW : HIGH);
    digitalWrite(LEDA,  segments[8] ==  '1' ? LOW : HIGH);
    digitalWrite(LEDF,  segments[9] ==  '1' ? LOW : HIGH);
    digitalWrite(LEDG,  segments[10] == '1' ? LOW : HIGH);
  }
}

#define NR_COLUMNS 14

const int frequencyTable[] = {
  // Note, this table contains frequency values multiplied by 10 
  // so that one decimal value is included.
  // A4 which in an equal tempered scale is at 440 Hz, is represented by 4400 in the table.
  //format:
  //low start C0, low end C0, low start C1, low end C1, ...
  //perfect start C0, perfect end C0, perfect start C1, perfect end C1, ...
  //high start C0, high end C0, high start C1, high end C1, ...
  
  158, 161, 318, 323,  636,  650, 1272, 1302, 2550, 2590, 5155,  5170, 10180, 10320, //C
  161, 165, 323, 330,  650,  660, 1302, 1315, 2590, 2640, 5170,  5300, 10320, 10730,
  165, 168, 330, 336,  660,  674, 1315, 1350, 2640, 2690, 5300,  5400, 10730, 11000,
  168, 171, 336, 345,  674,  689, 1350, 1378, 2690, 2750, 5420,  5500, 0,     0, // C#
  171, 175, 345, 351,  689,  698, 1378, 1392, 2750, 2800, 5490,  5591, 0,     0,
  175, 178, 351, 356,  698,  714, 1392, 1429, 2800, 2855, 5581,  5700, 0,     0,
  178, 181, 356, 364,  714,  731, 1429, 1461, 2855, 2923, 5700,  5830, 0,     0, // D
  181, 185, 364, 370,  731,  741, 1461, 1475, 2923, 2955, 5820,  5930, 0,     0,
  185, 188, 370, 377,  741,  756, 1475, 1512, 2955, 3020, 5920,  6080, 0,     0,
  181, 192, 377, 386,  756,  776, 1512, 1548, 3020, 3090, 6080,  6190, 0,     0, // D#
  192, 196, 386, 392,  776,  785, 1548, 1563, 3090, 3125, 6180,  6300, 0,     0,
  196, 201, 392, 400,  785,  799, 1563, 1602, 3125, 3200, 6290,  6440, 0,     0,
  201, 203, 400, 408,  799,  816, 1602, 1640, 3200, 3270, 6440,  6550, 0,     0, // E
  203, 208, 408, 415,  816,  829, 1640, 1656, 3270, 3330, 6540,  6660, 0,     0,
  208, 212, 415, 423,  829,  844, 1656, 1700, 3330, 3400, 6650,  6850, 0,     0,
  212, 215, 423, 432,  844,  866, 1700, 1738, 3400, 3475, 6850,  6930, 0,     0, // F0
  215, 220, 432, 442,  866,  880, 1738, 1753, 3475, 3490, 6920,  7040, 0,     0,
  220, 224, 442, 448,  880,  900, 1753, 1800, 3490, 3600, 7030,  7180, 0,     0,
  224, 229, 448, 460,  900,  918, 1800, 1842, 3600, 3680, 7180,  7340, 0,     0, // F#0
  229, 234, 460, 468,  918,  932, 1742, 1858, 3680, 3730, 7330,  7460, 0,     0,
  234, 239, 468, 476,  932,  955, 1858, 1915, 3730, 3820, 7450,  7630, 0,     0,
  239, 242, 476, 487,  955,  973, 1915, 1953, 3820, 3900, 7763,  7800, 0,     0, // G0
  242, 247, 487, 494,  973,  988, 1953, 1968, 3900, 3950, 7790,  7910, 0,     0,
  247, 252, 494, 505,  988, 1005, 1968, 2020, 3950, 4030, 7900,  8100, 0,     0,
  252, 257, 505, 516, 1005, 1031, 2020, 2068, 4030, 4110, 8100,  8270, 0,     0, // G#0
  257, 261, 516, 525, 1031, 1045, 2068, 2085, 4110, 4180, 8260,  8390, 0,     0,
  261, 266, 525, 538, 1045, 1075, 2085, 2155, 4180, 4265, 8380,  8570, 0,     0,
  266, 273, 538, 546, 1075, 1093, 2155, 2190, 4265, 4385, 8570,  8730, 0,     0, // A0
  273, 277, 546, 555, 1093, 1117, 2190, 2210, 4385, 4480, 8720,  8880, 0,     0,
  277, 283, 555, 567, 1117, 1137, 2210, 2285, 4480, 4560, 8870,  9050, 0,     0,
  283, 288, 567, 578, 1137, 1158, 2285, 2320, 4560, 4630, 9050,  9250, 0,     0, // A#0
  288, 293, 578, 587, 1158, 1172, 2320, 2342, 4630, 4705, 9240,  9410, 0,     0,
  293, 300, 585, 600, 1172, 1190, 2342, 2410, 4705, 4800, 9400,  9580, 0,     0,
  300, 306, 600, 612, 1190, 1222, 2410, 2460, 4800, 4895, 9580,  9800, 0,     0, // B0
  306, 312, 612, 623, 1222, 1237, 2460, 2482, 4895, 4950, 9790,  9990, 0,     0,
  312, 318, 623, 636, 1237, 1272, 2482, 2550, 4950, 5055, 9980, 10180, 0,     0
};

void testNote(int tableIndex, String pattern) {
  boolean condition = false;
  // Find an interval of frequencies which includes the measured frequency.
  for (int i = tableIndex * NR_COLUMNS; i < tableIndex * NR_COLUMNS + NR_COLUMNS; i += 2) {
    if ((frequencyTable[i] > 0) && (frequencyTable[i] < frequency) && (frequency < frequencyTable[i + 1])) {
      condition = true;
      break; // Exit the loop as soon as there is a match.
    }
  }
  // If any of the conditions turns out to be true,
  // switch on the corresponding LEDs.
  if (condition == true) setLeds(pattern);
}

void testLedsIndividually(int delayTime) {
  setLeds(F("10000000000"));
  Serial.println(F("1"));
  delay(delayTime);
  setLeds(F("01000000000"));
  delay(delayTime);
  setLeds(F("00100000000"));
  Serial.println(F("2"));
  delay(delayTime);
  setLeds(F("00010000000"));
  Serial.println(F("3"));
  delay(delayTime);
  setLeds(F("00001000000"));
  Serial.println(F("4"));
  delay(delayTime);
  setLeds(F("00000100000"));
  Serial.println(F("5"));
  delay(delayTime);
  setLeds(F("00000010000"));
  Serial.println(F("6"));
  delay(delayTime);
  setLeds(F("00000001000"));
  Serial.println(F("7"));
  delay(delayTime);
  setLeds(F("00000000100"));
  Serial.println(F("8"));
  delay(delayTime);
  setLeds(F("00000000010"));
  Serial.println(F("9"));
  delay(delayTime);
  setLeds(F("00000000001"));
  Serial.println(F("10"));
  delay(delayTime);
  // Clear all leds of 9 segment display.
  setLeds(F("00000000000"));
  delay(delayTime);
  
}

void testMusicalChars(int delayTime) {
  setLeds(F("00011000110")); // C
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00011010110")); // C#
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00011101001")); // D
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00011111001")); // D#
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00011000111")); // E
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00010000111")); // F0
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00010010111")); // F0#
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00001101111")); // G0
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00001111111")); // G0#
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00010101111")); // A0
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00010111111")); // A0#
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);
  setLeds(F("00011101111")); // B0   
  delay(delayTime);
  setLeds(F("00000000000"));
  delay(delayTime);  
}

void checkNotes()
{
  // C

  testNote( 0, F("10011000110"));
  testNote( 1, F("01011000110"));
  testNote( 2, F("00111000110"));

  // C#

  testNote( 3, F("10011010110"));
  testNote( 4, F("01011010110"));
  testNote( 5, F("00111010110"));

  // D
  
  testNote( 6, F("10011101001"));
  testNote( 7, F("01011101001"));
  testNote( 8, F("00111101001"));

  // D#

  testNote( 9, F("10011111001"));
  testNote(10, F("01011111001"));
  testNote(11, F("00111111001"));

  // E

  testNote(12, F("10011000111"));
  testNote(13, F("01011000111"));
  testNote(14, F("00111000111"));

  // F0

  testNote(15, F("10010000111"));
  testNote(16, F("01010000111"));
  testNote(17, F("00110000111"));

  // F#0

  testNote(18, F("10010010111"));
  testNote(19, F("01010010111"));
  testNote(20, F("00110010111"));

  // G0

  testNote(21, F("10001101111"));
  testNote(22, F("01001101111"));
  testNote(23, F("00101101111"));

  // G#0

  testNote(24, F("10001111111"));
  testNote(25, F("01001111111"));
  testNote(26, F("00101111111"));

  // A0

  testNote(27, F("10010101111"));
  testNote(28, F("01010101111"));
  testNote(29, F("00110101111"));

  // A#0

  testNote(30, F("10011111111"));
  testNote(31, F("01011111111"));
  testNote(32, F("00111111111"));

  // B0

  testNote(33, F("10011101111"));
  testNote(34, F("01011101111"));
  testNote(35, F("00111101111"));

  // TOO HIGH FOR NOW

  if ((frequency > 10180) && (frequency < 100000))
  {
    setLeds(F("00000010000"));
  }
}

//Interrupt triggered by any change od digital state of A0
ISR(PCINT1_vect)
{
  if(digitalRead(A0)) impulse_cnt++; //impulse_cnt inctemented on rising edge
}

void setup() {

  PCICR |= B00000010; //enable PCINT Port C
  PCMSK1 |= B00000001; //unmask pin A0 (PCINT8)

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(LEDE, OUTPUT);
  pinMode(LEDD, OUTPUT);
  pinMode(LEDC, OUTPUT);
  pinMode(LEDDP, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDA, OUTPUT);
  pinMode(LEDF, OUTPUT);
  pinMode(LEDG, OUTPUT);

  Serial.begin(9600);

    impulse_cnt = 0;
    last_read_time = millis();
  
#ifndef LED_TEST
  testLedsIndividually(100);
  //testMusicalChars(100);
#endif
}



#ifdef LED_TEST
void loop() {  
  testLedsIndividually(500);
  testMusicalChars(500);
}
#else
void loop() {

  if(impulse_cnt >= 100)
  {
    double curr_read_time = millis();
    double time = (curr_read_time - last_read_time) / impulse_cnt;
    frequency = 1.0 / (time/1000.0) * 10;

    impulse_cnt = 0;
    last_read_time = curr_read_time;
    
    checkNotes();
  }
}
#endif