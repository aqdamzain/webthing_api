#define LARGE_JSON_BUFFERS 1

#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Thing.h>
#include <WebThingAdapter.h>

const char *ssid = "noir_2.4G";
//const char *password = "noir24999";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 201);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);

#define DHTPIN 13
#define DHTTYPE DHT22

const int ledPin = 2;

WebThingAdapter *adapter;

DHT dht(DHTPIN, DHTTYPE);

const char *DHT22Types[] = {"TemperatureSensor", nullptr};
ThingDevice weather("dht22", "DHT22 Weather Sensor", DHT22Types);
ThingProperty weatherTemp("temperature", "", NUMBER, "TemperatureProperty");
ThingProperty weatherHum("humidity", "", NUMBER, "HumidityProperty");

void readDHT22Data() {
  float dhtHUMID, dhtTEMP;
  delay(2000);
  dhtTEMP = dht.readTemperature();
  dhtHUMID = dht.readHumidity();
  ThingPropertyValue value;
  value.number = dhtTEMP;
  weatherTemp.setValue(value);
  value.number = dhtHUMID;
  weatherHum.setValue(value);
  Serial.println("Temperature");
  Serial.println(dhtTEMP);
  Serial.println("Humidity");
  Serial.println(dhtHUMID);
}

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);
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
  
  dht.begin();

  adapter = new WebThingAdapter("weathersensor", WiFi.localIP());

  // Set unit for temperature
  weatherTemp.unit = "degree celsius";
  
  // Set title to "Humidity"
  weatherHum.title = "Humidity";
  // Set unit for humidity to %
  weatherHum.unit = "percent";
  // Set humidity as read only
  weatherHum.readOnly = "true";

  weather.addProperty(&weatherTemp);
  weather.addProperty(&weatherHum);
  adapter->addDevice(&weather);
  adapter->begin();
}

void loop() {
  readDHT22Data();
  adapter->update();
}
