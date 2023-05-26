#define relay_pin 26
#define active_relay_button 22

static bool isRelayOn = false;

void SetUpWaterPump()
{
  isRelayOn = false;
  
  pinMode(active_relay_button, INPUT_PULLUP);
  pinMode(relay_pin, OUTPUT);

  digitalWrite(relay_pin, !isRelayOn);
}
