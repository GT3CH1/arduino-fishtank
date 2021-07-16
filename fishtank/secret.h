#ifndef secret_h
#define secret_h

// The URL to your firebase host - no http:// or https://
static String FIREBASE_HOST = "new-project-3cddb-default-rtdb.firebaseio.com";
// The authentication key
static String FIREBASE_AUTH = "7ZnZXL7mXVoqv8h4nxalIvMjMMc7zj97is4AxJyL";
// Your wifi network
static char WIFI_SSID[] = "peasenet-iot";
// Your wifi network password.
static char WIFI_PASSWORD[] = "peasenetiot2019";

const short VERSION = 3;
const char* DEVICE_TYPE = "fishtank";
const char* UPDATE_SERVER = "api.peasenet.com";
const unsigned short SERVER_PORT = 80;

const String LIGHT_GUID = "c91124eb-74fb-41b7-b048-4f139d7587cf";
const String PUMP_GUID = "08b3f4fe-f8ad-4687-9be7-94d313eb0391";

#endif
