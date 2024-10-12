#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "vietsquad";
const char* password = "klapeczki";
const char* mqtt_server = "192.168.129.25";
#define ONE_WIRE_BUS 2

unsigned long sensorStamp = 0;
unsigned long mqttStamp = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);
}

void reconnectWiFi() {
  Serial.println("WiFi connection lost. Reconnecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Reconnecting to WiFi...");
  }
  Serial.println("Reconnected to WiFi");
}


void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
    if (WiFi.status() != WL_CONNECTED){
      reconnectWiFi();
    }
  }
}

void loop() {
  
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi(); // Check and reconnect WiFi if needed
  }

  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  static unsigned long sensorStamp = 0;
  static unsigned long mqttStamp = 0;
if (millis() - sensorStamp > 100) {
    sensorStamp = millis();
    sensors.requestTemperatures(); // Request temperature readings
    float temp = sensors.getTempCByIndex(0); // Get temperature from the first sensor
    Serial.print(F("Real Time Temp: "));
    Serial.println(temp);
  }

  // Check if it's time to publish to MQTT
  if (millis() - mqttStamp > 1000) { // 10 seconds
    mqttStamp = millis();
    sensors.requestTemperatures(); // Request temperature readings
    float temp = sensors.getTempCByIndex(0); // Get temperature from the first sensor
    char tempString[10]; // Define tempString as a character array with sufficient size
    snprintf(tempString, sizeof(tempString), "%.2f", temp);
    client.publish("home/kitchen/temperature", tempString); // Publish the temperature
  }
}
