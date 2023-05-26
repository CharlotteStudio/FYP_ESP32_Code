#include <SoftwareSerial.h>

#define rxPin 13
#define txPin 12

SoftwareSerial mySerial(rxPin, txPin);

void SetUpSerial()
{
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(4800);
}
