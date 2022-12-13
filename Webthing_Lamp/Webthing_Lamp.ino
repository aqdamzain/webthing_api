/**
 * Simple server compliant with Mozilla's proposed WoT API
 * Originally based on the HelloServer example
 * Tested on ESP8266, ESP32, Arduino boards with WINC1500 modules (shields or
 * MKR1000)
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <Arduino.h>
#include "Thing.h"
#include "WebThingAdapter.h"

// TODO: Hardcode your wifi credentials here (and keep it private)
const char *ssid = "noir_2.4G";
//const char *password = "noir24999";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 203);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);

const int ledPin = 2;
const int lampPin = 5; // manually configure Lamp pin

WebThingAdapter *adapter;

const char *lampTypes[] = {"OnOffSwitch", "Lamp", nullptr};
ThingDevice lamp("lamp-1", "DC Lamp", lampTypes);
ThingProperty lampOn("on", "", BOOLEAN, "OnOffProperty");

bool lastOn = false;

void setup(void) {
  pinMode(lampPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(lampPin, HIGH);
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
  
  adapter = new WebThingAdapter("smartlamp", WiFi.localIP());

  lamp.addProperty(&lampOn);
  adapter->addDevice(&lamp);
  adapter->begin();
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(lamp.id);
}

void loop(void) {
  adapter->update();
  bool on = lampOn.getValue().boolean;
  digitalWrite(lampPin, on ? LOW : HIGH); // active low trigger relay
  if (on != lastOn) {
    Serial.print(lamp.id);
    Serial.print(": ");
    Serial.println(on);
  }
  lastOn = on;
}
