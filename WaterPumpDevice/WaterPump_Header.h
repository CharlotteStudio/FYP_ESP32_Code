#define relay_pin 26
#define active_relay_button 22

static bool isRelayOn = false;
static unsigned long activeTime_waterPump    = 30000;
static unsigned long nextTime_closeWaterPump = 0;

void SetUpWaterPump()
{
  isRelayOn = false;
  
  pinMode(active_relay_button, INPUT_PULLUP);
  pinMode(relay_pin, OUTPUT);

  digitalWrite(relay_pin, !isRelayOn);
}

void ActiveWaterPump()
{
  isRelayOn = true;
  nextTime_closeWaterPump = millis() + activeTime_waterPump;
  digitalWrite(relay_pin, !isRelayOn);
  int activeTime = activeTime_waterPump / 1000;
  printf("Set active water pump at [%d] second\n", activeTime);
}

void CloseWaterPump()
{
  isRelayOn = false;
  digitalWrite(relay_pin, !isRelayOn);
  printf("Close water pump\n");
}

bool IsCloseWaterPump()
{
  if (!isRelayOn) return false;
  if (millis() < nextTime_closeWaterPump) return false;
  CloseWaterPump();
  return true;
}
