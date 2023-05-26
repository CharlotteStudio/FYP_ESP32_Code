#define maxButtonCount 4

typedef void (*OnClickButtonCallback)();

static OnClickButtonCallback onClickCallback[maxButtonCount];
static bool                  buttonDown[maxButtonCount];

void SetUpCallback(int buttonIndex, OnClickButtonCallback cb)
{
  buttonDown[buttonIndex]      = false;
  onClickCallback[buttonIndex] = cb;
  Serial.println("set up OnClick Button Callback.");
}

void CheckButtonOnClick(int buttonIndex, int pin)
{
  int getOnClick = digitalRead(pin);
  
  if(getOnClick == 0)
  {
    if(!buttonDown[buttonIndex])
    {
      buttonDown[buttonIndex] = true;
      Serial.print("pin [");
      Serial.print(pin);
      Serial.print("] Button up   (");
      Serial.print(getOnClick);
      Serial.println(").");

      onClickCallback[buttonIndex]();
    }
  }
  else
  {
    if(buttonDown[buttonIndex])
    {
      buttonDown[buttonIndex] = false;
      Serial.print("pin [");
      Serial.print(pin);
      Serial.print("] Button down (");
      Serial.print(getOnClick);
      Serial.println(").");
    }
  }
}
