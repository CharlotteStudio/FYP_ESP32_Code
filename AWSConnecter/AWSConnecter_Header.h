// Version 0.1.0, Release at 20230523
#define led_no_wifi_pin 23
#define led_aws_pin 18

static bool led_wifi_isOn = true;
static bool led_aws_isOn  = false;

void SetUpLED()
{
  led_wifi_isOn = true;
  led_aws_isOn  = false;
  
  pinMode(led_no_wifi_pin, OUTPUT);
  pinMode(led_aws_pin, OUTPUT);
  
  digitalWrite(led_no_wifi_pin, led_wifi_isOn);
  digitalWrite(led_aws_pin, led_aws_isOn);
}

void CheckWiFiLED(bool value)
{
  if (led_wifi_isOn != !value)
  {
    led_wifi_isOn = !value;
    digitalWrite(led_no_wifi_pin, led_wifi_isOn);
  }
}

void CheckAWSLED(bool value)
{
  if (led_aws_isOn != value)
  {
    led_aws_isOn = value;
    digitalWrite(led_aws_pin, led_aws_isOn);
  }
}
