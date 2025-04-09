// Include Libraries
#include <TinyPICO.h>
#include <esp_now.h>
#include <WiFi.h>

TinyPICO tp = TinyPICO();

// Define a data structure
typedef struct struct_message {
  char a[32];
  int sensors[5];
  int acceleration[3];
  int rotation[3];
  int temperature;
} struct_message;

// Create a structured object
struct_message myData;

// Callback function executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Sensors A:");
  Serial.print(myData.sensors[0]);
  Serial.print(" B:");
  Serial.print(myData.sensors[1]);
  Serial.print(" C:");
  Serial.print(myData.sensors[2]);
  Serial.print(" D:");
  Serial.print(myData.sensors[3]);
  Serial.print(" E:");
  Serial.println(myData.sensors[4]);

  /* Print out the values */
  Serial.print("Acceleration X:");
  Serial.print(myData.acceleration[0]);
  Serial.print(", Y:");
  Serial.print(myData.acceleration[1]);
  Serial.print(", Z:");
  Serial.print(myData.acceleration[2]);
  Serial.println(" m/s^2");

  Serial.print("Rotation X:");
  Serial.print(myData.rotation[0]);
  Serial.print(", Y:");
  Serial.print(myData.rotation[1]);
  Serial.print(", Z:");
  Serial.print(myData.rotation[2]);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(myData.temperature);
  Serial.println(" degC");

  Serial.println("");
  delay(500);
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(9600);

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  tp.DotStar_CycleColor(25);
}