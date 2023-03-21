#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <HT1621.h>

static const int GPS_RX_PIN = 5, GPS_TX_PIN = 6;
static const int LCD1_CS_PIN = 2, LCD1_WR_PIN = 3, LCD1_DATA_PIN = 4;
static const int LCD2_CS_PIN = 7, LCD2_WR_PIN = 8, LCD2_DATA_PIN = 9;
static const int LCD_BACKLIGHT_PIN = 12;

static const uint32_t SERIAL_BAUD = 9600;
static const uint32_t MAX_DATA_AGE_MS = 1500;

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);
HT1621 lcd1;
HT1621 lcd2;

void setup()
{
  ss.begin(SERIAL_BAUD);
  lcd1.begin(LCD1_CS_PIN, LCD1_WR_PIN, LCD1_DATA_PIN, LCD_BACKLIGHT_PIN);
  lcd2.begin(LCD2_CS_PIN, LCD2_WR_PIN, LCD2_DATA_PIN);

  lcd1.clear();
  lcd2.clear();

  lcd1.noBacklight();
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
    displayFixConfidence();
  }
}

void readGps()
{
  while (ss.available())
    gps.encode(ss.read());
}

void printGpsMissing()
{
  lcd1.print("ERROR");
  lcd2.print("NO GPS");
}

void crash()
{
  while (true)
    ;
}

void printFixing()
{
  const int intervalMs = 500;
  const char stages = 4;
  const char currentStage = (millis() % (intervalMs * stages)) / intervalMs;

  lcd2.print(gps.satellites.value(), 0);

  switch (currentStage)
  {
  case 1:
    lcd1.print("GPS");
    break;
  case 2:
    lcd1.print("GPS-");
    break;
  case 3:
    lcd1.print("GPS--");
    break;
  default:
    lcd1.print("GPS---");
    break;
  }
}

void displayFixConfidence()
{
  const int satellites = gps.satellites.value();

  // Battery can only be 0, 1, 2, or 3. For 3 or fewer satellites, we don't show anything, so we
  // indicate from 4 to 9 satellites using both displays.
  lcd2.setBatteryLevel(min(satellites - 3, 3));
  lcd1.setBatteryLevel(min(satellites - 6, 3));
}

void printStartUp()
{
  lcd1.print("HELL0");
  lcd2.clear();
}

void printSpeed()
{
  if (!gps.speed.isUpdated())
    return;

  lcd1.print(gps.speed.knots(), 2);
}

void printCourse()
{
  if (!gps.course.isUpdated())
    return;

  lcd2.print(gps.course.deg(), "   %03li", 0);
}
