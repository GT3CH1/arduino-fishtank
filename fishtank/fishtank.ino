#include <aa_lib.h>
#include <aREST.h>
#include "secret.h"
int LIGHT_PIN = 2;
int PUMP_PIN = 3;
aREST m_rest = aREST();
WiFiServer m_server(80);
aa_lib automation(WIFI_SSID, WIFI_PASSWORD, DEVICE_TYPE, UPDATE_SERVER, SERVER_PORT, VERSION, API_KEY, API_ID);

void setup() {
  // put your setup code here, to run once:
  automation.setup();
  m_server.begin();
  m_rest.function("on", setOn);
  m_rest.function("off", setOff);
}

void loop() {
  automation.checkAndConnectWifi();
  WiFiClient client = m_server.available();
  if (!client) {
     return;
  }
  while(!client.available()){
    delay(1);
  }
  m_rest.handle(client);
  automation.handleSketchDownload();
}

int setOn(String guid){
    checkGuid(guid,true);
    return 1;
}


int setOff(String guid){
    checkGuid(guid,false);
    return 1;
}

void checkGuid(String guid, bool state){
  if (guid == "c91124eb-74fb-41b7-b048-4f139d7587cf"){
    pinMode(LIGHT_PIN,OUTPUT);
    digitalWrite(LIGHT_PIN, state ? LOW : HIGH);
  }
  if (guid == "08b3f4fe-f8ad-4687-9be7-94d313eb0391"){
    pinMode(PUMP_PIN,OUTPUT);
    digitalWrite(PUMP_PIN, state ? HIGH : LOW);
  }
  automation.sendUpdate(guid,state);
}
