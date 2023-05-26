#define pin_button 5
#define pin_led_r 23
#define pin_led_y 18
#define pin_led_g 19

void BLEConnectCallback()
{
  digitalWrite(pin_led_r, LOW);
  digitalWrite(pin_led_y, HIGH);
}

void BLEDisconnectCallback()
{
  digitalWrite(pin_led_r, HIGH);
  digitalWrite(pin_led_y, LOW);
}

void WiFiMeshConnectCallback()
{
  digitalWrite(pin_led_r, LOW);
  digitalWrite(pin_led_g, HIGH);
}

void WiFiMeshBLEDisconnectCallback()
{
  digitalWrite(pin_led_r, HIGH);
  digitalWrite(pin_led_g, LOW);
}

void SetUpLED()
{
  pinMode(pin_button, INPUT_PULLUP);
  pinMode(pin_led_r, OUTPUT);
  pinMode(pin_led_y, OUTPUT);
  pinMode(pin_led_g, OUTPUT);
  digitalWrite(pin_led_r, HIGH);
  digitalWrite(pin_led_y, LOW);
  digitalWrite(pin_led_g, LOW);
}
