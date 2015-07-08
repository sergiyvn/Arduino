#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <util/delay.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

double data;
double LENGTH = 0.221;

volatile unsigned char s1 = 0;
volatile unsigned char s2 = 0;

// Timer/Counter 0 initialization
void Timer0_Init( void )
{
    TCNT0 = 0;
    // Bits: COM0A1 COM0A0 COM0B1 COM0B0 - - WGM01 WGM00
    TCCR0A = 0;
    // Bits: FOC0A FOC0B - - WGM02 CS02 CS01 CS00
    TCCR0B = 0;
    // Bits: - - - - - OCIE0B OCIE0A TOIE0
    TIMSK0 = 0;
    // Bits: - - - - - OCF0B OCF0A TOV0
    TIFR0 = 0;
}

// Timer/Counter 1 initialization
void Timer1_Init( void )
{
    TCNT1 = 0;
   // Bits: COM1A1 COM1A0 COM1B1 COM1B0 - - WGM11 WGM10
    TCCR1A = 0;
    // Bits: ICNC1 ICES1 - WGM13 WGM12 CS12 CS11 CS10
    TCCR1B = 0;
    // Bits: FOC1A FOC1B - - - - - -
    TCCR1C = 0;
    // Bits: - - ICIE1 - - OCIE1B OCIE1A TOIE1
    TIMSK1 = 0;
    // Bits: – – ICF1 – - OCF1B OCF1A TOV1
    TIFR1 = 0;
}

void setup() {
  lcd.init();                     // инициализация LCD 
  lcd.backlight();                // включаем подсветку
  lcd.clear();                    // очистка дисплея
  
   // Global disable interrupts
    cli();
    // Timer/Counter 0 initialization
    Timer0_Init();
    // Timer/Counter 1 initialization
    Timer1_Init();
    // Global enable interrupts
    sei();

    Serial.begin(9600);

    attachInterrupt(0, sensor_1, FALLING);
    attachInterrupt(1, sensor_2, FALLING);
}

void loop()
{
    while ( s1 == 0 && s2 == 0 ) ;
    _delay_ms(800); // wait 800 ms

    if ( s1 != 0 && s2 != 0 )
    {
        data = LENGTH / (TCNT1 * (1.0 / 16000000.0)); // v = s / t
    }
    else
    {
        data = 0;
    }

    Serial.println(data);
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print(data);

    TCCR1B = 0;
    TCNT1 = 0;

    s1 = 0;
    s2 = 0;
}

void sensor_1()
{
    if ( s1 == 0 )
    {
        TCCR1B = (1<<CS10); // Timer/Counter 1 running (no prescaling)
        s1 = 1;
        Serial.println(TCCR1B);
    }
}

void sensor_2()
{
    if ( s2 == 0 )
    {
        TCCR1B = 0; // Timer/Counter 1 stopped (no clock source)
        s2 = 1;
        Serial.println(TCCR1B);
    }
}
