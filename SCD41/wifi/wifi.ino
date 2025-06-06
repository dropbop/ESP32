#include <WiFi.h>

const char* SSID     = "testingtesting";
const char* PASSWORD = "waterwater";

void connectWiFi() {
  WiFi.mode(WIFI_STA);          // station mode
  WiFi.begin(SSID, PASSWORD);   // start Wi-Fi

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }

  Serial.print("\nConnected. IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
}

void loop() {
  // put your main code here
}
