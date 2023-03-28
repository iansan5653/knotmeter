#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <LedControl.h>
#include <math.h>

static const int GPS_RX_PIN = 6, GPS_TX_PIN = 5;
static const int DISPLAY1_CS_PIN = 3, DISPLAY1_CLK_PIN = 2, DISPLAY1_DATA_PIN = 4;
static const int DISPLAY2_CS_PIN = 9, DISPLAY2_CLK_PIN = 8, DISPLAY2_DATA_PIN = 10;

static const uint32_t SERIAL_BAUD = 9600;
static const uint32_t MAX_DATA_AGE_MS = 1500;

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);
LedControl display1(DISPLAY1_DATA_PIN, DISPLAY1_CLK_PIN, DISPLAY1_CS_PIN);
LedControl display2(DISPLAY2_DATA_PIN, DISPLAY2_CLK_PIN, DISPLAY2_CS_PIN);

void setup()
{
  ss.begin(SERIAL_BAUD);

  display1.shutdown(0, false);
  display1.clearDisplay(0);
  display1.setIntensity(0, 1);

  display2.shutdown(0, false);
  display2.clearDisplay(0);
  display2.setIntensity(0, 1);
}

void loop()
{
  readGps();

  if (millis() < 3000)
  {
    printStartUp();
  }
  else if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    printGpsMissing();
    crash();
  }
  else if (!gps.speed.isValid() || !gps.course.isValid() || gps.speed.age() > MAX_DATA_AGE_MS || gps.course.age() > MAX_DATA_AGE_MS)
  {
    printFixing();
  }
  else
  {
    printSpeed();
    printCourse();
  }
}

void readGps()
{
  while (ss.available())
    gps.encode(ss.read());
}

void printGpsMissing()
{
  printChars(display1, 'E', ' ', '1');
  printChars(display2, '-', '-', '-');
}

void crash()
{
  while (true)
    ;
}

void printFixing()
{
  const int n = millis() / 500 % 4;

  switch (n)
  {
  case (0):
    printChars(display1, '-', ' ', ' ');
    break;
  case (1):
  case (3):
    printChars(display1, ' ', '-', ' ');
    break;
  case (2):
    printChars(display1, ' ', ' ', '-');
    break;
  }

  printInt(display2, gps.satellites.value(), false);
}

void printStartUp()
{
  printChars(display1, ' ', 'H', '1'); // HI
}

void printSpeed()
{
  if (gps.speed.isUpdated())
    printFloat(display1, gps.speed.knots());
}

void printCourse()
{
  if (gps.course.isUpdated())
    printInt(display2, gps.course.deg(), true);
}

/**
 * Supported
 * '0','1','2','3','4','5','6','7','8','9','0',
 * 'A','b','c','d','E','F','H','L','P',
 * '.','-','_',' '
 */
void printChars(LedControl display, char char1, char char2, char char3)
{
  display.setChar(0, 0, char1, false);
  display.setChar(0, 1, char2, false);
  display.setChar(0, 2, char3, false);
}

void printInt(LedControl display, int number, bool leadingZeros)
{
  const int absolute = abs(number);
  const byte hundreds = absolute / 100;
  const byte tens = (absolute % 100) / 10;
  const byte ones = absolute % 10;

  const char padChar = leadingZeros ? '0' : ' ';

  if (absolute > 999)
  {
    printChars(display, 'E', 'A', 'A'); // ERR
  }
  else if (absolute >= 100)
  {
    display.setDigit(0, 0, hundreds, false);
    display.setDigit(0, 1, tens, false);
    display.setDigit(0, 2, ones, false);
  }
  else if (absolute >= 10)
  {
    display.setChar(0, 0, padChar, false);
    display.setDigit(0, 1, tens, false);
    display.setDigit(0, 2, ones, false);
  }
  else
  {
    display.setChar(0, 0, padChar, false);
    display.setChar(0, 1, padChar, false);
    display.setDigit(0, 2, ones, false);
  }
}

void printFloat(LedControl display, float number)
{
  const float absolute = abs(number);

  const int integer = absolute;
  const byte hundreds = integer / 100;
  const byte tens = (integer % 100) / 10;
  const byte ones = integer % 10;

  const byte decimal = (absolute - integer) * 100;
  const byte tenths = decimal / 10;
  const byte hundredths = decimal % 10;

  if (integer > 999)
  {
    printChars(display, 'E', 'A', 'A'); // ERR
  }
  else if (integer >= 100)
  {
    display.setDigit(0, 0, hundreds, false);
    display.setDigit(0, 1, tens, false);
    display.setDigit(0, 2, ones, false);
  }
  else if (integer >= 10)
  {

    display.setDigit(0, 0, tens, false);
    display.setDigit(0, 1, ones, true);
    display.setDigit(0, 2, tenths, false);
  }
  else
  {

    display.setDigit(0, 0, ones, true);
    display.setDigit(0, 1, tenths, false);
    display.setDigit(0, 2, hundredths, false);
  }
}
