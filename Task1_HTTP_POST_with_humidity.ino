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
const char* ssid = "ssid";
const char* password = "password";

//Your Domain name with URL path or IP address
const char* serverName = "https://io.adafruit.com";

//get this parameter from YOUR ADAFRUIT IO KEY
const char* IO_USERNAME = "username";
const char* IO_KET = "apikey";

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
      
      Serial.println("HTTP new Temperature POST");
      String httpPath = String(serverName) + "/api/v2/" + String(IO_USERNAME) + "/feeds/temperature/data";
      Serial.println(" httpPath = " + httpPath);
      String httpRequestData = "value=" + String(temperature) + "&X-AIO-Key=" + String(IO_KET);
      Serial.println(" httpRequestData = " + httpRequestData);
      http.begin(httpPath);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpResponseCode = http.POST(httpRequestData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      Serial.println("HTTP new Humidity POST");
      httpPath = String(serverName) + "/api/v2/" + String(IO_USERNAME) + "/feeds/humidity/data";
      Serial.println(" httpPath = " + httpPath);
      httpRequestData = "value=" + String(humidity) + "&X-AIO-Key=" + String(IO_KET);
      Serial.println(" httpRequestData = " + httpRequestData);
      http.begin(httpPath);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      httpResponseCode = http.POST(httpRequestData);
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
