#include "./src/Network/Network.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  if (ConnectWifi())
  {
    char date[11];
    getCurrentDate(date);
    char time[10];
    getCurrentTime(time);

    Serial.println(date);
    Serial.println(time);
  }
  else
  {
    Serial.println("Failed to connect to wifi");
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
}
