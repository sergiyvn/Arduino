
//segments
//int a = 1;// LED pin 11
//int b = 2;// LED pin 7
//int c = 3;// LED pin 4
//int d = 4;// LED pin 2
//int e = 5;// LED pin 1
//int f = 6;// LED pin 10
//int g = 7;// LED pin 5
//int p = 8;// LED pin 3
//digits
//int d1 = 9;// LED pin 12
//int d2 = 10;// LED pin 9
//int d3 = 11;// LED pin 8
//int d4 = 12;// LED pin 6

const int digitPins[4] = {
  4, 5, 6, 7
};                 //4 common anode pins of the display

//const int clockPin = 11;    //74HC595 Pin 11
//const int latchPin = 12;    //74HC595 Pin 12
//const int dataPin = 13;     //74HC595 Pin 14

const byte digit[10] =      //seven segment digits in bits
{
  B00111111, //0
  B00000110, //1
  B01011011, //2
  B01001111, //3
  B01100110, //4
  B01101101, //5
  B01111101, //6
  B00000111, //7
  B01111111, //8
  B01101111  //9
};
int digitBuffer[4] = {0};
int digitScan = 0, flag = 0,  soft_scaler = 0;

float tempK;

//void setup(){
//  Serial.begin(9600);
//  for(int i=0;i<4;i++)
//  {
//    pinMode(digitPins[i],OUTPUT);
//  }
//  pinMode(latchPin, OUTPUT);
//  pinMode(clockPin, OUTPUT);
//  pinMode(dataPin, OUTPUT);
//}
//
////writes the temperature on display
//void updateDisp(){
//  for(byte j=0; j<4; j++)
//    digitalWrite(digitPins[j], LOW);
//
//  digitalWrite(latchPin, LOW);
//  shiftOut(dataPin, clockPin, MSBFIRST, B11111111);
//  digitalWrite(latchPin, HIGH);
//
//  delayMicroseconds(100);
//  digitalWrite(digitPins[digitScan], HIGH);
//
//  digitalWrite(latchPin, LOW);
//  if(digitScan==2)
//    shiftOut(dataPin, clockPin, MSBFIRST, ~(digit[digitBuffer[digitScan]] | B10000000)); //print the decimal point on the 3rd digit
//  else
//    shiftOut(dataPin, clockPin, MSBFIRST, ~digit[digitBuffer[digitScan]]);
//
//  digitalWrite(latchPin, HIGH);
//  digitScan++;
//  if(digitScan>3) digitScan=0;
//}
//
//void loop(){
//  tempK = 1234;//random(1,100);
//  Serial.print(tempK);
//  Serial.print("\n");
//  digitBuffer[3] = int(tempK)/1000;
//  digitBuffer[2] = (int(tempK)%1000)/100;
//  digitBuffer[1] = (int(tempK)%100)/10;
//  digitBuffer[0] = (int(tempK)%100)%10;
//  updateDisp();
////  delay(5000);
//  delay(1000);
//
//}


// Define the bit-patterns for the 7-segment displays
const byte SevenSeg[16] =
{
  // Hex digits
  B11111100, B01100000, B11011010, B11110010,   // 0123
  B01100110, B10110110, B10111110, B11100000,   // 4567
  B11111110, B11110110, B11101110, B00111110,   // 89AB
  B00011010, B01111010, B10011110, B10001110,   // CDEF

};

// Pin connected to clock pin (SRCLK,11) of 74HC595
const int clockPin = 11;
// Pin connected to latch pin (RCLK,12) of 74HC595
const int latchPin = 12;
// Pin connected to Data in (SER,14) of 74HC595
const int dataPin  = 13;
// Pin connected to cathodes
const int disp1 = 7;  // Displays are numbered from left to right
const int disp2 = 6;  // as disp1 to disp4.
const int disp3 = 5;  // Scan displays quickly and use POV to display
const int disp4 = 4;  // 4 digits with only 1 shift register

//
void setup()
{
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin,  OUTPUT);
  pinMode(disp1, OUTPUT);
  pinMode(disp2, OUTPUT);
  pinMode(disp3, OUTPUT);
  pinMode(disp4, OUTPUT);
}
//
void loop()
{
  for (int i = 0; i < 10000; ++i)
  {
    // Display i as 4 digits on the display.
    // No leading zero blanking.
    //    for (int j=0; j<20; j++) // Draw the display several times to slow it down
    SevenSegDisplay(i);
    delay(1000);
  }
}

void SevenSegDisplay(int number)
{
  
  int d1, d2, d3, d4; // Temporary values for thousands, hundreds, tens and units
  short ones, tens, hundreds, thousands;

  if (number > 9999)
    number = 9999;  // Do some bounds checks to avoid strangeness
  if (number < 0)
    number = 0;
  int input_num = number;
  
  thousands = int(input_num / 1000);
  thousands = (thousands << 4) + 1;

  hundreds = int(input_num / 100);
  hundreds = hundreds - (int(hundreds / 10) * 10);
  hundreds = (hundreds << 4) + 2;

  tens = int(input_num / 10);
  tens = tens - (int(tens / 10) * 10);
  tens = (tens << 4) + 4;

  ones = input_num - (int(input_num / 10) * 10); ;
  ones = (ones << 4) + 8;

    d1 = (int) (number / 1000);    // Get thousands
    number = number - (d1 * 1000);
    d2 = (int) (number / 100);     // Get hundreds
    number = number - (d2 * 100);
    d3 = (int) (number / 10);      // Get tens
    d4 = number - (d3 * 10);       // Get units
//  Serial.print(number);

  Serial.print(thousands);
  Serial.print("__");
  Serial.print(hundreds);
  Serial.print("__");
  Serial.print(tens);
  Serial.print("__");
  Serial.print(ones);
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, input_num);
  digitalWrite(latchPin, HIGH);

//  Serial.print(d1);
//  Serial.print(d2);
//  Serial.print(d3);
//  Serial.print(d4);
  Serial.print("\n");
//  for(int i=0; i<5; i++)
//  {
  /*if (input_num >= 1000){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, thousands);
    digitalWrite(latchPin, HIGH);
    delay(5);
  }
//  else
  if (input_num >= 100){
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, hundreds);
    digitalWrite(latchPin, HIGH);
    delay(5);
  }
//  else
  if (input_num >= 10)
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, tens);
    digitalWrite(latchPin, HIGH);
    delay(5);
  }
//  else
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, ones);
    digitalWrite(latchPin, HIGH);
    delay(5);
  } 
  digitalWrite(latchPin, LOW);
  */
// }

//    DisplayADigit(disp1, byte(SevenSeg[d1]), 1); // Show thousands
//    DisplayADigit(disp2, byte(SevenSeg[d2]), 2); // Show hundreds
//    DisplayADigit(disp3, byte(SevenSeg[d3]), 3); // Show tens
//    DisplayADigit(disp4, byte(SevenSeg[d4]), 4); // Show units

  //  DisplayADigit(disp1, d1); // Show thousands
  //  DisplayADigit(disp2, d2); // Show hundreds
  //  DisplayADigit(disp3, d3); // Show tens
  //  DisplayADigit(disp4, d4); // Show units
  
    
}

void DisplayADigit(int dispno, byte digit2disp, int n)
{

  // Turn off the shift register pins
  // while you're shifting bits:
  //  digitalWrite(latchPin, LOW);

  AllDispOff(LOW);  // Turn off all cathode drivers.
//  digitalWrite(dispno, LOW);
  // shift the bits out:
  //  if(n == 4)
  shiftOut(dataPin, clockPin, LSBFIRST, digit2disp);
  AllDispOff(HIGH);
  //  digitalWrite(latchPin, HIGH);  // Set latch high to set segments.

//  digitalWrite(dispno, HIGH);  // Drive one cathode low to turn on display.

  delay(5);  // Wait a bit for POV
}

void AllDispOff(boolean m)
{
  // Turn all cathode drivers off

  digitalWrite(dataPin, m);
//  digitalWrite(disp1, m);
//  digitalWrite(disp2, m);
//  digitalWrite(disp3, m);
//  digitalWrite(disp4, m);
}
