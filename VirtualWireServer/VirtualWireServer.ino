#include <VirtualWire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

// For the breakout, you can use any (2 or) 3 pins
#define sclk 13//Mega only
#define mosi 11//Mega only
#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);//Uno
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);//Mega

uint8_t buf_out1[16];
int sr;

String txt1 = "";
String txt2 = "";

// - TEMPERS -
// Powerh 1
float t_Zal;
float t_Kyhnya;
float t_hall_down;
float t_WC;
float t_Office;

int t_Zal_c;
int t_Kyhnya_c;
int t_hall_down_c;
int t_WC_c;
int t_Office_c;

int t_Zal_d;
int t_Kyhnya_d;
int t_hall_down_d;
int t_WC_d;
int t_Office_d;

// Powerh 2
float t_hall_up;
float t_Vanna;
float t_Spalnya;
float t_Dutyacha;

int t_hall_up_c;
int t_Vanna_c;
int t_Spalnya_c;
int t_Dutyacha_c;

int t_hall_up_d;
int t_Vanna_d;
int t_Spalnya_d;
int t_Dutyacha_d;

void setup()
{
  // If your TFT's plastic wrap has a Black Tab, use the following:
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab

  tft.fillScreen(ST7735_BLACK);

  pinMode(6, OUTPUT);
  pinMode(7, INPUT);
  vw_set_tx_pin(6);
  vw_set_rx_pin(7);
  vw_set_ptt_inverted(true);
  vw_setup(2000);
  vw_rx_start();

  Serial.begin(9600);
  //  Serial1.begin(9600);
}

void loop()
{
  send_message(01, 01);
  delay(500);
  read_message();
  delay(500);
  send_message(02, 01);
  delay(500);
  read_message();
  delay(500);

  tft.fillScreen(ST7735_BLACK);
  //  tft.setCursor(0, 0);
  drawText(txt1, ST7735_RED, 5, 5);
  //  tft.setCursor(10, 0);
  drawText(txt2, ST7735_GREEN, 5, 25);

  /*tft.setCursor(5, 45);

  tft.println(utf8rus("Зал"));
  tft.println(utf8rus("Кухня"));
  tft.println(utf8rus("Коридор низ"));
  tft.println(utf8rus("Туалет"));
  tft.println(utf8rus("Офiс"));
  tft.println(utf8rus("Коридорх верх"));
  tft.println(utf8rus("Вання"));
  tft.println(utf8rus("Спальня"));
  tft.println(utf8rus("Дитяча"));

  tft.setCursor(90, 45);
  tft.println(t_Zal);

  tft.setCursor(90, 55);
  tft.println(t_Kyhnya);

  tft.setCursor(90, 65);
  tft.println(t_hall_down);

  tft.setCursor(90, 75);
  tft.println(t_WC);

  tft.setCursor(90, 85);
  tft.println(t_Office);

  tft.setCursor(90, 95);
  tft.println(t_hall_up);

  tft.setCursor(90, 105);
  tft.println(t_Vanna);

  tft.setCursor(90, 115);
  tft.println(t_Spalnya);

  tft.setCursor(90, 125);
  tft.println(t_Dutyacha);*/

  // show Tempers
  //  tft.setTextColor(ST7735_WHITE);
  //  tft.setTextSize(1);
  //  tft.setCursor(5, 45);
  //  tft.println("A" utf8rus("АБВГДЕЖЗИЙКЛМНОП"));

  drawText(utf8rus("ЗАЛ"), ST7735_BLUE, 5, 45);
  drawText(String(t_Zal), ST7735_BLUE, 90, 45);

  drawText(utf8rus("КУХНЯ"), ST7735_BLUE, 5, 55);
  drawText(String(t_Kyhnya), ST7735_BLUE, 90, 55);

  drawText(utf8rus("КОРИДОР НИЗ"), ST7735_BLUE, 5, 65);
  drawText(String(t_hall_down), ST7735_BLUE, 90, 65);

  drawText(utf8rus("ТУАЛЕТ"), ST7735_BLUE, 5, 75);
  drawText(String(t_WC), ST7735_BLUE, 90, 75);

  drawText(utf8rus("OFFICE"), ST7735_BLUE, 5, 85);
  drawText(String(t_Office), ST7735_BLUE, 90, 85);

  drawText(utf8rus("КОРИДОР ВЕРХ"), ST7735_BLUE, 5, 95);
  drawText(String(t_hall_up), ST7735_BLUE, 90, 95);

  drawText(utf8rus("ВАННА"), ST7735_BLUE, 5, 105);
  drawText(String(t_Vanna), ST7735_BLUE, 90, 105);

  drawText(utf8rus("СПАЛЬНЯ"), ST7735_BLUE, 5, 115);
  drawText(String(t_Spalnya), ST7735_BLUE, 90, 115);

  drawText(utf8rus("ДИТЯЧА"), ST7735_BLUE, 5, 125);
  drawText(String(t_Dutyacha), ST7735_BLUE, 90, 125);
  delay(5000);
}

void send_message(int id, int msg)
{
  buf_out1[0] = id;
  buf_out1[1] = msg;
  vw_send(buf_out1, sizeof(buf_out1));
  vw_wait_tx(); // Wait until the whole message is gone
}

void read_message()
{
  //receive functionality
  uint8_t buf_in[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if ( vw_have_message())
  {
    Serial.println("");
    Serial.println("Message aviable");
  }

  if (vw_get_message(buf_in, &buflen)) // Non-blocking
  {

    int i;
    // Message with a good checksum received, dump it.
    Serial.print("Got: ");
    String txt = "";
    for (i = 0; i < buflen; i++)
    {
      txt = txt + buf_in[i];
      txt = txt + " ";
      Serial.print(buf_in[i], HEX);
      Serial.print(" ");
    }
    int id = buf_in[0];
    if (id == 1)
      txt1 = txt;
    else if (id == 2)
      txt2 = txt;

    switch (buf_in[1])
    {
      case 1:
        setTempers(buf_in, id);
        break;
    }


    Serial.println("");
    Serial.print("id: ");
    Serial.println(id);
  }
  //  else
  //    delay(100);
}

void setTempers(uint8_t buf_in[VW_MAX_MESSAGE_LEN], int ID)
{
  if (ID == 1)
  {
    t_Zal_c = buf_in[2];
    t_Zal_d = buf_in[3];
    t_Zal = t_Zal_c + t_Zal_d / 100.0;
    t_Kyhnya_c = buf_in[4];
    t_Kyhnya_d = buf_in[5];
    t_Kyhnya = t_Kyhnya_c + t_Kyhnya_d / 100.0;
    t_hall_down_c = buf_in[6];
    t_hall_down_d = buf_in[7];
    t_hall_down = t_hall_down_c + t_hall_down_d / 100.0;
    t_WC_c = buf_in[8];
    t_WC_d = buf_in[9];
    t_WC = t_WC_c + t_WC_d / 100.0;
    t_Office_c = buf_in[10];
    t_Office_d = buf_in[11];
    t_Office = t_Office_c + t_Office_d / 100.0;
  } else if (ID == 2)
  {
    t_hall_up_c = buf_in[2];
    t_hall_up_d = buf_in[3];
    t_hall_up = t_hall_up_c + t_hall_up_d / 100.0;
    t_Vanna_c = buf_in[4];
    t_Vanna_d = buf_in[5];
    t_Vanna = t_Vanna_c + t_Vanna_d / 100.0;
    t_Spalnya_c = buf_in[6];
    t_Spalnya_d = buf_in[7];
    t_Spalnya = t_Spalnya_c + t_Spalnya_d / 100.0;
    t_Dutyacha_c = buf_in[8];
    t_Dutyacha_d = buf_in[9];
    t_Dutyacha = t_Dutyacha_c + t_Dutyacha_d / 100.0;
  }
}

void drawText(String _text, uint16_t color, int x, int y)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(1);
  tft.print(_text);
}
