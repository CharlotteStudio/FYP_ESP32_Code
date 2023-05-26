// Version 0.1.1, Release at 20230526
#include <SoftwareSerial.h>

#define rxPin 13
#define txPin 12

static SoftwareSerial mySerial(rxPin, txPin);

void SetUpSerial()
{
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(4800);
}
