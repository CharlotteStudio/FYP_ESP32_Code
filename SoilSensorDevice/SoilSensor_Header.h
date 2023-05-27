#define Soil_Sensor_pin 36

static unsigned long waitingTime_soilSensor = 10000;
static unsigned long nextTime_soilSensor = 0;

int currentSoilSensorValue = 0;

bool TryGetSoilSensorValue()
{
  if(millis() > nextTime_soilSensor)
  {
    nextTime_soilSensor = millis() + waitingTime_soilSensor;
    currentSoilSensorValue = analogRead(Soil_Sensor_pin);
    printf("Get the soil value : [%d]\n", currentSoilSensorValue);
    return true;
  }
  return false;
}
