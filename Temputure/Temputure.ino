// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN  2     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE  DHT11   // DHT 22  (AM2302)
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
 
  lcd.init();                     // инициализация LCD 
  lcd.backlight();                // включаем подсветку
  lcd.clear();                    // очистка дисплея
  
  dht.begin();
}

void loop() {
//  lcd.clear();  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    
    
    lcd.setCursor(0, 0); 
//    lcd.print("Humidity: ");
    lcd.print(h);
    lcd.print(" %");
//    lcd.print(" H2: ");
//    lcd.print(h2);
    lcd.setCursor(0, 1);
//    lcd.print("Temperature: ");
    lcd.print(t);
    lcd.print(" *C");
//    lcd.print(" T2: ");
//    lcd.print(t2);
//    lcd.print(" *C");
  }
  
    delay(1000);
  
}

