#include <WiFi.h>

const char* ssid = "SJSU_guest";

void setup()
{
  Serial.begin(9600);
  delay(1000);
  WiFi.begin(ssid);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop()
{

}
