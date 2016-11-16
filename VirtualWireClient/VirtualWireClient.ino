#include <VirtualWire.h>

uint8_t buf_out1[16];

int ID = 2;

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

// - RELE PINS -
// Powerh 1
int pin_Zal = 22;
int pin_Kyhnya = 24;
int pin_hall_down = 26;
int pin_WC = 28;
int pin_Office = 30;
// Powerh 2
int pin_hall_up = 22;
int pin_Vanna = 24;
int pin_Spalnya = 26;
int pin_Dutyacha = 28;

void setup()
{
  pinMode(6, OUTPUT);
  pinMode(7, INPUT);
  vw_set_tx_pin(6);
  vw_set_rx_pin(7);
  vw_set_ptt_inverted(true);
  vw_setup(2000);
  vw_rx_start();

  Serial.begin(9600);
  //  Serial3.begin(9600);
}

void loop()
{

  if ( vw_have_message())
    read_message();
}

void send_message(int id, int codeCommand, int value1_1 = 0, int value1_2 = 0, int value2_1 = 0, int value2_2 = 0, int value3_1 = 0, int value3_2 = 0, int value4_1 = 0, int value4_2 = 0, int value5_1 = 0, int value5_2 = 0)
{
  buf_out1[0] = id;
  buf_out1[1] = codeCommand;
  buf_out1[2] = value1_1;
  buf_out1[3] = value1_2;
  buf_out1[4] = value2_1;
  buf_out1[5] = value2_2;
  buf_out1[6] = value3_1;
  buf_out1[7] = value3_2;
  buf_out1[8] = value4_1;
  buf_out1[9] = value4_2;
  buf_out1[10] = value5_1;
  buf_out1[11] = value5_2;
  vw_send(buf_out1, sizeof(buf_out1));
  vw_wait_tx(); // Wait until the whole message is gone
}

void read_message()
{
  //receive functionality
  uint8_t buf_in[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf_in, &buflen)) // Non-blocking
  {
    int i;
    boolean currentId = false;

    Serial.println("");
    Serial.print("buf_in[0] id: ");
    Serial.println(buf_in[0]);
    float randomTemper = random(4000);
    if (buf_in[0] == ID)
    {
      int command = buf_in[1];
      switch (buf_in[1])
      {
        case 1:
          // return Tempers
          Serial.println("");
          Serial.print("randomTemper: ");
          Serial.println(randomTemper);
          if (ID == 1)
          {
            t_Zal = t_Kyhnya = t_hall_down = t_WC = t_Office = randomTemper/100.0;

            t_Zal_c = t_Zal;
            t_Zal_d = (t_Zal - t_Zal_c) * 100;
            t_Kyhnya_c = t_Kyhnya;
            t_Kyhnya_d = (t_Kyhnya - int(t_Kyhnya_c)) * 100;
            t_hall_down_c = t_hall_down;
            t_hall_down_d = (t_hall_down - t_hall_down_c) * 100;
            t_WC_c = t_WC;
            t_WC_d = (t_WC - t_WC_c) * 100;
            t_Office_c = t_Office;
            t_Office_d = (t_Office - t_Office_c) * 100;

            send_message(ID, command, t_Zal_c, t_Zal_d, t_Kyhnya_c, t_Kyhnya_d, t_hall_down_c, t_hall_down_d, t_WC_c, t_WC_d, t_Office_c, t_Office_d);
          } else if (ID == 2)
          {
            t_hall_up = t_Vanna = t_Spalnya = t_Dutyacha = randomTemper / 100.0;

            t_hall_up_c = t_hall_up;
            t_hall_up_d = (t_hall_up - t_hall_up_c) * 100;
            t_Vanna_c = t_Vanna;
            t_Vanna_d = (t_Vanna - t_Vanna_c) * 100;
            t_Spalnya_c = t_Spalnya;
            t_Spalnya_d = (t_Spalnya - t_Spalnya_c) * 100;
            t_Dutyacha_c = t_Dutyacha;
            t_Dutyacha_d = (t_Dutyacha - t_Dutyacha_c) * 100;

            send_message(ID, command, t_hall_up_c, t_hall_up_d, t_Vanna_c, t_Vanna_d, t_Spalnya_c, t_Spalnya_d, t_Dutyacha_c, t_Dutyacha_d);
          }
          break;
        case 2:
          // set rele values
          if (ID == 1)
          {
            if (buf_in[2] == 1)
              digitalWrite(pin_Zal, HIGH);
            else
              digitalWrite(pin_Zal, LOW);

            if (buf_in[3] == 1)
              digitalWrite(pin_Kyhnya, HIGH);
            else
              digitalWrite(pin_Kyhnya, LOW);

            if (buf_in[4] == 1)
              digitalWrite(pin_hall_down, HIGH);
            else
              digitalWrite(pin_hall_down, LOW);

            if (buf_in[5] == 1)
              digitalWrite(pin_WC, HIGH);
            else
              digitalWrite(pin_WC, LOW);

            if (buf_in[6] == 1)
              digitalWrite(pin_Office, HIGH);
            else
              digitalWrite(pin_Office, LOW);
          } else if (ID == 2)
          {
            if (buf_in[2] == 1)
              digitalWrite(pin_hall_up, HIGH);
            else
              digitalWrite(pin_hall_up, LOW);

            if (buf_in[3] == 1)
              digitalWrite(pin_Vanna, HIGH);
            else
              digitalWrite(pin_Vanna, LOW);

            if (buf_in[4] == 1)
              digitalWrite(pin_Spalnya, HIGH);
            else
              digitalWrite(pin_Spalnya, LOW);

            if (buf_in[5] == 1)
              digitalWrite(pin_Dutyacha, HIGH);
            else
              digitalWrite(pin_Dutyacha, LOW);
          }
          break;
        default:
          Serial.println("* Unknown code *");
          break;
      }
    }

    // Message with a good checksum received, dump it.
    Serial.print("Got: ");
    for (i = 0; i < buflen; i++)
    {
      Serial.print(buf_in[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
  } else
    delay(100);
}
