#include <GoogleArduino.h>
#include "secret.h"

const int lightPin = 2;
const int pumpPin = 3;
const int resetPin = 4;
int invertedPins[] = {3};
GoogleArduino automation(resetPin);

void setup(){
  automation.setup(FIREBASE_HOST,FIREBASE_AUTH,WIFI_SSID,WIFI_PASSWORD,invertedPins);
  automation.firstRun("fishtank-light",lightPin);
  automation.firstRun("fishtank-pump",pumpPin);
}

void loop(){
  automation.checkStatus("fishtank-light",lightPin);
  automation.checkStatus("fishtank-pump",pumpPin);
  delay(5000);
}
