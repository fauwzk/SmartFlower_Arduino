#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include "NTPClient.h"
#include "WiFiUdp.h"
#include <ArduinoJson.h>
#include <FS.h>
#include <U8g2lib.h>
#include <Arduino.h>
#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT11

String humterre;
float humidity, temp_f;
String webString = "";
unsigned long previousMillis = 0;
const long interval = 2000;
ESP8266WebServer server(80);
const int pump_out = 13;
const int buttonPin = 12;
int buttonState = 0;
int sensorValue = 0;
int outputValue = 0;
int Reading;
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};
String variable_jour;
String variable_heure;
String variable_minute;
const int analogInPin = A0;
U8G2_SH1107_SEEED_128X128_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
void handle_root() {
  server.send(200, "text/html", "Voici la page d'accueil");
  delay(100);
}

DHT dht(DHTPIN, DHTTYPE);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  WiFiManager wm;
  wm.setClass("invert");
  bool res;
  res = wm.autoConnect("SmartFlower_Conf");
  if (!res) {
    Serial.println("Impossible de se connecter");
  }
  else {
    Serial.println("Connecte");
  }
  server.on("/", handle_root);
  server.on("/temp", []() {
    gettemperature();
    webString = String((int)temp_f);
    server.send(200, "text/html", webString);
  });
  server.on("/hum", []() {
    gettemperature();
    webString = String((int)humidity);
    server.send(200, "text/html", webString);
  });

  server.on("/main", mainmenu);
  server.on("/jour", jour);
  server.on("/heure", heure);
  server.on("/minute", minute);
  server.on("/getheure", getheure);
  server.on("/getminute", getminute);
  pinMode(PIN_GROVE_POWER, OUTPUT);
  digitalWrite(PIN_GROVE_POWER, 1);
  pinMode(buttonPin, INPUT);
  dht.begin();
  timeClient.begin();
  SPIFFS.begin();
  u8g2.begin();
  Wire.begin();
  server.begin();
}

void loop() {
  buttonState = digitalRead(buttonPin);
  //Serial.println(buttonState);
  timeClient.update();
  server.handleClient();
  gettemperature();
  if (buttonState == HIGH) {
    String temp = String((int)temp_f);
    String hum = String((int)humidity);
    sensorValue = analogRead(analogInPin);
    outputValue = map(sensorValue, 0, 1023, 0, 100);
    String humterre = String((int)outputValue);
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_ncenB10_tr);
      u8g2.setCursor(0, 24);
      //u8g2.drawStr(0,24,"Hello World!");
      u8g2.print(temp);
      u8g2.setCursor(0, 48);
      u8g2.print(hum);
      u8g2.setCursor(0, 72);
      u8g2.print(humterre);
    } while ( u8g2.nextPage() );
  }
  else {
    u8g2.clear();
  }

}

void mainmenu() {
  gettemperature();
  sensorValue = analogRead(analogInPin);
  outputValue = map(sensorValue, 0, 1023, 0, 100);
  String humterre = String((int)outputValue);
  String hum = String((int)humidity);
  String temp = String((int)temp_f);
  StaticJsonDocument<200> doc;
  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["humterre"] = humterre;
  String jsonoutput;
  serializeJson(doc, jsonoutput);
  server.send(200, "text/html", jsonoutput);
}

void jour() {
  variable_jour = daysOfTheWeek[timeClient.getDay()];
  server.send(200, "text/html", variable_jour);
}

void heure() {
  variable_heure = timeClient.getHours();
  server.send(200, "text/html", variable_heure);
}

void minute() {
  variable_minute = timeClient.getMinutes();
  server.send(200, "text/html", variable_minute);
}

void getheure() {
  String value;
  File heure = SPIFFS.open("/heure.txt", "r");
  String a = heure.readString();
  server.send(200, "text/plain", a);
  heure.close();
}

void getminute() {
  String value;
  File minute = SPIFFS.open("/minute.txt", "r");
  String a = minute.readString();
  server.send(200, "text/plain", a);
  minute.close();
}

void gettemperature() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    humidity = dht.readHumidity();
    temp_f = dht.readTemperature(false);
    if (isnan(humidity) || isnan(temp_f)) {
      Serial.println("Impossible de commniquer avec le module DHT11");
      return;
    }
  }
}
