#include <Arduino.h>
#include "Thing.h"
#include "WebThingAdapter.h"


const char *ssid = "noir_2.4G";
// Set your Static IP address
IPAddress local_IP(192, 168, 1, 202);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);

const int ledPin = 2;
const int fanPin = 5; // manually configure Fan pin

WebThingAdapter *adapter;

const char *fanTypes[] = {"OnOffSwitch", "Fan", nullptr};
ThingDevice fan("fan-1", "DC Fan", fanTypes);
ThingProperty fanOn("on", "", BOOLEAN, "OnOffProperty");

bool lastOn = false;

void setup(void) {
  pinMode(fanPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(fanPin, HIGH);
  digitalWrite(ledPin, HIGH);
  
  Serial.begin(9600);
  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(ssid);
  Serial.println("\"");
#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);
#endif

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  
  //WiFi.begin(ssid, password);
  WiFi.begin(ssid);
  Serial.println("");

  // Wait for connection
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(ledPin, blink ? LOW : HIGH); // active low led
    blink = !blink;
  }
  digitalWrite(ledPin, HIGH); // active low led
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  adapter = new WebThingAdapter("smartfan", WiFi.localIP());

  fan.addProperty(&fanOn);
  adapter->addDevice(&fan);
  adapter->begin();
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(fan.id);
}

void loop(void) {
  adapter->update();
  bool on = fanOn.getValue().boolean;
  digitalWrite(fanPin, on ? LOW : HIGH); // active low relay
  if (on != lastOn) {
    Serial.print(fan.id);
    Serial.print(": ");
    Serial.println(on);
  }
  lastOn = on;
}
