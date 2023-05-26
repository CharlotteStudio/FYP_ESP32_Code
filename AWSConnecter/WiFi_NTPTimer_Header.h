/* Version 0.1.0
 *
 * Compatible ESP32 & ESP8266
 * Get the current time from internet (NTP)
 * Required Connected Internet
 * 
 * Resource : https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
 *
 * Functions for ESP8266 & ESP32 :
 * void InitNTPTimer()
 * 
 * ESP32 also have :
 * String GetYear()
 * String GetMonth()
 * String GetDay()
 * String GetWeek()
 * String GetHour()
 * String GetHour_12()
 * String GetMinute()
 * String GetSecond()
 * void PrintLocalTime()
 *
 */
#include<time.h>

#define NTP_Server "pool.ntp.org"
#define NIST_Server "time.nist.gov"

const int8_t TIME_ZONE = 8;             // +8 HK
const int    daylightOffset_sec = 0;

struct tm timeinfo;

time_t now;
time_t nowish = 1510592825;

void InitNTPTimer()
{
  Serial.print("Setting time using SNTP ");
  configTime(TIME_ZONE * 3600, daylightOffset_sec, NTP_Server, NIST_Server);
  
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");

  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

#ifdef ESP32

String GetTime(int i);
String GetYear();
String GetMonth();
String GetDay();
String GetWeek();
String GetHour();
String GetHour_12();
String GetMinute();
String GetSecond();

void PrintLocalTime()
{
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: +-");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Test Function :");

  Serial.println(GetYear());
  Serial.println(GetMonth());
  Serial.println(GetDay());
  Serial.println(GetWeek());
  Serial.println(GetHour());
  Serial.println(GetHour_12());
  Serial.println(GetMinute());
  Serial.println(GetSecond());
}

String GetTime(int i)
{
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return "";
  }

  char c[10];
  switch (i)
  {
      case 1:  // Year=1
          strftime(c, 10, "%Y", &timeinfo);
          break;
      case 2:  // Month=2
          strftime(c, 10, "%B", &timeinfo);
          break;
      case 3:  // Day=3
          strftime(c, 10, "%d", &timeinfo);
          break;
      case 4:  // Week=4
          strftime(c, 10, "%A", &timeinfo);
          break;
      case 5:  // Hour=5
          strftime(c, 10, "%H", &timeinfo);
          break;
      case 6:  // Hour_12=6
          strftime(c, 10, "%I", &timeinfo);
          break;
      case 7:  // Minute=7
          strftime(c, 10, "%M", &timeinfo);
          break;
      case 8:  // Second=8,
          strftime(c, 10, "%S", &timeinfo);
          break;
      default:
          break;
   }
  return String(c);
}

String GetYear()
{
  return GetTime(1);
}

String GetMonth()
{
  return GetTime(2);
}

String GetDay()
{
  return GetTime(3);
}

String GetWeek()
{
  return GetTime(4);
}

String GetHour()
{
  return GetTime(5);
}

String GetHour_12()
{
  return GetTime(6);
}

String GetMinute()
{
  return GetTime(7);
}

String GetSecond()
{
  return GetTime(8);
}
#endif
