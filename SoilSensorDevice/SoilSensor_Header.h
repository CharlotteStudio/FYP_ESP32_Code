#define Soil_Sensor_pin 26

#define waitingTime_soilSensor 5000

static unsigned long nextTime_soilSensor = 0;

static int currentSoilSensorValue = 0;
static int lastSoilSensorValue    = 0;

void TryGetSoilSensorValue()
{
  if(millis() > nextTime_soilSensor)
  {
    nextTime_soilSensor = millis() + waitingTime_soilSensor;
    currentSoilSensorValue = analogRead(Soil_Sensor_pin);
    Serial.print("Get the soil value : ");
    Serial.println(currentSoilSensorValue);
  }
}
