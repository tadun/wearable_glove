// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/

#include "WiFi.h"
 
void setup(){
  Serial.begin(9600);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}

// right: 98:CD:AC:D3:5B:B8
// left: 98:CD:AC:D3:63:38