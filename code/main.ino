#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <HT1621.h>

static const int GPS_RX_PIN = 6, GPS_TX_PIN = 5;
static const int LCD1_CS_PIN = 2, LCD1_WR_PIN = 3, LCD1_DATA_PIN = 4;
static const int LCD2_CS_PIN = 7, LCD2_WR_PIN = 8, LCD2_DATA_PIN = 9;
static const int LCD_BACKLIGHT_PIN = 12;
static const uint32_t SERIAL_BAUD = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);
HT1621 lcd1;
HT1621 lcd2;

void setup()
{
  Serial.begin(SERIAL_BAUD);
  ss.begin(SERIAL_BAUD);
  lcd1.begin(LCD1_CS_PIN, LCD1_WR_PIN, LCD1_DATA_PIN, LCD_BACKLIGHT_PIN);
  lcd2.begin(LCD2_CS_PIN, LCD2_WR_PIN, LCD2_DATA_PIN);

  lcd1.clear();
  lcd2.clear();

  lcd1.noBacklight(); // controls both backlights

  Serial.print("RUNNING");
}

void loop()
{
  lcd1.print("ONE");
  lcd2.print("TWO");

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    lcd1.print("NOGPS1");
    lcd2.print("NOGPS2");
    while (true)
      ;
  }
}
