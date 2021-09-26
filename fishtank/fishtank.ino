#include <ArduinoOTA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include "secret.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include <aREST.h>

int status = WL_IDLE_STATUS;
aREST rest = aREST();
WiFiServer server(80);
int LIGHT_PIN = 2;
int PUMP_PIN = 3;
bool fishtank_light;
bool fishtank_pump;
StaticJsonDocument<512> doc;

void handleSketchDownload() {
    const char* PATH = "/ota/arduino/update-%s-%d.bin";
    const unsigned long CHECK_INTERVAL = 10000;

    static unsigned long previousMillis;

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis < CHECK_INTERVAL)
        return;
    previousMillis = currentMillis;

    WiFiClient transport;
    HttpClient client(transport, UPDATE_SERVER, SERVER_PORT);
    Serial.println(String(UPDATE_SERVER) + String(PATH));
    char buff[50];
    snprintf(buff, sizeof(buff), PATH, DEVICE_TYPE, VERSION + 1);

    Serial.print("Check for update file ");
    Serial.println(buff);

    client.get(buff);

    int statusCode = client.responseStatusCode();
    Serial.print("Update status code: ");
    Serial.println(statusCode);
    if (statusCode != 200) {
        client.stop();
        return;
    }

    long length = client.contentLength();
    if (length == HttpClient::kNoContentLengthHeader) {
        client.stop();
        Serial.println("Server didn't provide Content-length header. Can't continue with update.");
        return;
    }
    Serial.print("Server returned update file of size ");
    Serial.print(length);
    Serial.println(" bytes");

    if (!InternalStorage.open(length)) {
        client.stop();
        Serial.println("There is not enough space to store the update. Can't continue with update.");
        return;
    }
    byte b;
    while (length > 0) {
        if (!client.readBytes(&b, 1)) // reading a byte with timeout
            break;
        InternalStorage.write(b);
        length--;
    }
    InternalStorage.close();
    client.stop();
    if (length > 0) {
        Serial.print("Timeout downloading update file at ");
        Serial.print(length);
        Serial.println(" bytes. Can't continue with update.");
        return;
    }

    Serial.println("Sketch update apply and reset.");
    Serial.flush();
    InternalStorage.apply(); // this doesn't return
}

void setup() {
    server.begin();
    rest.variable("connected",&status);
    rest.function("on", setOn);
    rest.function("off", setOff);
}

void checkAndConnectWifi(){
    status = WiFi.status();
    if(status != WL_CONNECTED){
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.print("WiFi disconnected, connecting to wifi.");
        while(status != WL_CONNECTED){
            status = WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
            Serial.print(".");
            delay(100);
        }
        Serial.println("");
        Serial.println("Connected");
        digitalWrite(LED_BUILTIN, LOW);
        printIP();
        bool last_state = getLastState(LIGHT_GUID);
        checkGuid(LIGHT_GUID, last_state);
        last_state = getLastState(PUMP_GUID);
        checkGuid(PUMP_GUID, last_state);
    }
}

void printIP(){
    Serial.println("IP Address: ");
    Serial.println(getIP());
}

void loop() {
    checkAndConnectWifi();
    WiFiClient client = server.available();
    rest.handle(client);
    handleSketchDownload();
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
    if (guid == LIGHT_GUID){
        pinMode(LIGHT_PIN,OUTPUT);
        digitalWrite(LIGHT_PIN, state ? LOW : HIGH);
        fishtank_light = state;
    }
    if (guid == PUMP_GUID){
        pinMode(PUMP_PIN,OUTPUT);
        digitalWrite(PUMP_PIN, state ? HIGH : LOW);
        fishtank_pump = !state;
        state = state;
    }
    sendUpdate(guid,state);

}

void sendUpdate(String guid, bool state){
    WiFiClient wifi;
    HttpClient httpClient = HttpClient(wifi, UPDATE_SERVER, SERVER_PORT);
    String contentType = "application/x-www-form-urlencoded";
    String data = "guid=" + guid + "&ip=" + getIP() + "&state=" + (state ? "true" : "false") + "&sw_version=" + String(VERSION);
    httpClient.sendHeader("x-api-key", API_KEY);
    httpClient.sendHeader("x-auth-id", API_ID);
    httpClient.sendHeader(HTTP_HEADER_CONTENT_LENGTH, data.length());
    Serial.println(data);
    httpClient.put("/smarthome/update",contentType,data);
    int statusCode = httpClient.responseStatusCode(); 
    httpClient.stop();
}

bool getLastState(String guid){
    WiFiClient wifi;
    Serial.println("Getting last state for " + guid);
    HttpClient httpClient = HttpClient(wifi, UPDATE_SERVER, SERVER_PORT);
    httpClient.beginRequest();
    httpClient.get("/smarthome/device/"+guid);
    httpClient.sendHeader("x-api-key", API_KEY);
    httpClient.sendHeader("x-auth-id", API_ID);
    httpClient.endRequest();
    String responeBody = httpClient.responseBody();
    DeserializationError error = deserializeJson(doc, responeBody.c_str(), 512);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return false;
    }
    Serial.println(responeBody);
    return doc["last_state"];
}


String getIP()
{
  const IPAddress& ipAddress = WiFi.localIP();
    return String(ipAddress[0]) + String(".") +\
        String(ipAddress[1]) + String(".") +\
        String(ipAddress[2]) + String(".") +\
        String(ipAddress[3])  ;
}
