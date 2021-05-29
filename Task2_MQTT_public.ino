/*
  Author      : Aiman Aminudin
  Email       : aimanasiru@gmail.com
  Description : This code is used for publish data to hiveMQ using MQTT protocol
  TASK        : 2
*/
#define MQTT_VERSION MQTT_VERSION_3_1

#include <WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "ssid";
const char* password = "ssidpassword";
const char* yourname = "yourname";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "broker.hivemq.com";
const int   mqtt_port = 1883;
const char* mqtt_unique_id = "unique id";
const char* mqtt_shared_topic = "D5qcew/";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

int pinDHT11 = 15;
SimpleDHT11 dht11(pinDHT11);
int temperature;
int humidity;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    mqttClient.disconnect();
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(mqtt_unique_id)) {
      Serial.println("connected");
      // Subscribe
      String topic = String(mqtt_shared_topic) + "#";
      mqttClient.subscribe(topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!mqttClient.connected()) {
    mqtt_reconnect();
  }
  mqttClient.loop();


  //Send an HTTP POST request every 10 second
  if ((millis() - lastTime) > timerDelay) {
    lastTime = millis();
    Serial.println();

    //Read DHT11 Value
    byte t = 0;
    byte h = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&t, &h, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
      Serial.print(","); Serial.println(SimpleDHTErrDuration(err));
      temperature = 0;
      humidity = 0;
    } else {
      temperature = t;
      humidity = h;
      Serial.print("DHT11 Sample OK: ");
      Serial.print(temperature); Serial.print(" *C, ");
      Serial.print(humidity); Serial.println(" H");
    }

    // publish Temperature
    String topic = String(mqtt_shared_topic) + String(yourname) + "/temperature";
    String message = String(temperature);
    Serial.println( " Sending [" + topic + "] " + message);
    mqttClient.publish(topic.c_str(), message.c_str());

  }

  delay(10);
}