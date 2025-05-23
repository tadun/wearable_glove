#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Orientation Sensor Test"); 
  Serial.println("");
  
  /* Initialise the sensor */
  bno.begin();
  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  
  /* Display the floating point data */
  //Serial.print("X: ");
  Serial.print(event.orientation.x, 4);
  // Serial.print(",Y: ");
  Serial.print(",");
  Serial.print(event.orientation.y, 4);
  // Serial.print(",Z: ");
  Serial.print(",");
  Serial.print(event.orientation.z, 4);
  Serial.println("");
  
  delay(100);
}