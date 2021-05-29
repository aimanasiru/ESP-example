/*
  Author      : Aiman Aminudin
  Email       : aimanasiru@gmail.com
  Description : This code is used for sent data to io.adafruit.com using POST REST API.
  TASK        : 1
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <SimpleDHT.h>

//Change this to your network setting
const char* ssid = "<ssid>";
const char* password = "<ssid_password>";

//Your Domain name with URL path or IP address
const char* serverName = "https://io.adafruit.com";

//get this parameter from YOUR ADAFRUIT IO KEY
const char* IO_USERNAME = "<adafruit_username>";
const char* IO_KET = "<adafruit_IO_key";

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

  Serial.println("Timer set to " + String(timerDelay / 1000) + " Second.");
}

void loop() {

  //Send an HTTP POST request every 10 second
  if ((millis() - lastTime) > timerDelay) {
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

    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      Serial.println("HTTP new POST");

      // Your Domain name with URL path or IP address with path
      String httpPath = String(serverName) + "/api/v2/" + String(IO_USERNAME) + "/feeds/temperature/data";
      Serial.println(" httpPath = " + httpPath);

      // Data to send with HTTP POST
      String httpRequestData = "value=" + String(temperature) + "&X-AIO-Key=" + String(IO_KET);
      Serial.println(" httpRequestData = " + httpRequestData);

      http.begin(httpPath);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);

      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content - Type", "application / json");
      //int httpResponseCode = http.POST(" {\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }

    lastTime = millis();
  }

  delay(10);
}