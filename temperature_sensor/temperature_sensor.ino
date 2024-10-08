#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "vietsquad";
const char* password = "klapeczki";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";

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
    int reading = analogRead(0);
    float temp = reading * 0.0048828125 * 100;
    Serial.print(F("Real Time Temp: "));
    Serial.println(temp);
  }

  if (millis() - mqttStamp > 10000) { // 10 seconds
    mqttStamp = millis();
    int reading = analogRead(0);
    float temp = reading * 0.0048828125 * 100;
    char* tempString; // Define tempString as a character array
    snprintf(tempString, sizeof(tempString), "%.2f", temp);
    client.publish("home/esp32/temperature", tempString); // Publish the temperature
  }
}
