// Version 0.1.1
#define maxButtonCount 4  // maximum callback function count
#define buttonDebugLog 0  // 0 hide, 1 show

typedef void (*OnClickButtonCallback)();

static OnClickButtonCallback  onClickCallback[maxButtonCount];
static bool                   buttonDown[maxButtonCount];
static int                    currentButtonCallbackCount = 0;

void SetUpCallback(int buttonIndex, OnClickButtonCallback cb)
{
  if (currentButtonCallbackCount >= maxButtonCount)
  {
    Serial.println("[Warning] Over the maximum of button callback count.");
    return;
  }
  currentButtonCallbackCount++;
  buttonDown[buttonIndex]      = false;
  onClickCallback[buttonIndex] = cb;
  Serial.println("Set up OnClick Button Callback.");
}

void CheckButtonOnClick(int buttonIndex, int pin)
{
  int getOnClick = digitalRead(pin);
  
  if (getOnClick == 1)
  {
    if(!buttonDown[buttonIndex])
    {
      buttonDown[buttonIndex] = true;
      
      if (buttonDebugLog)
      {
        Serial.print("pin [");
        Serial.print(pin);
        Serial.print("] Button up   (");
        Serial.print(getOnClick);
        Serial.println(").");
      }
      
      if (onClickCallback[buttonIndex] != NULL)
      {
        onClickCallback[buttonIndex]();
      }
      else
      {
        Serial.print("[Warning] Have don't set up any function at [");
        Serial.print(buttonIndex);
        Serial.println("].");
      }
    }
  }
  else
  {
    if (buttonDown[buttonIndex])
    {
      buttonDown[buttonIndex] = false;
      
      if (buttonDebugLog)
      {
        Serial.print("pin [");
        Serial.print(pin);
        Serial.print("] Button down (");
        Serial.print(getOnClick);
        Serial.println(").");
      }
    }
  }
}
