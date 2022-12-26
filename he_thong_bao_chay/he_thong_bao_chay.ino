#include <Arduino.h>
#include "DHT.h"
#include <MQ135.h>
// FIREBASE LIBRARY
#include "FirebaseESP32.h"

// FIREBASE DECLARE
#define FIREBASE_AUTH "AIzaSyAK0raN4nFl3vqgyeELdID-zOmtfrD7_6A"
#define FIREBASE_HOST "https://iot-project-1c24d-default-rtdb.firebaseio.com"

#define WIFI_SSID "realme 5i"
#define WIFI_PASSWORD "0988948824"

#define DHTPIN 4       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 22  (AM2302), AM2321
#define PIN_MQ135 34   // MQ135 Analog Input Pin


// create variables
FirebaseData firebaseData;
FirebaseJson json;

DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135_sensor(PIN_MQ135);

// khai bao canh bao
bool bat_canh_bao = false;

// khai bao den led
int led_cap_1 = 12;
int led_cap_2 = 14;
int moduleRelay = 35;

// biến hứng data
int dataFirebase;
//gốc

String temperature = "/temperature";
String humidity = "/humidity";
//mq135
String ppm = "/ppm";
// tín hiệu cảnh báo
String warning = "/warning";
// biến của dht11
float h, t;
// biến của mq135
float p;
// gốc safe
String temperature_safe = "/temperature-safe";
String humidity_safe = "/humidity-safe";

float h_safe, t_safe;
float p_safe;

// mq135-safe
String ppm_safe = "/ppm-safe";

void setup() {

  Serial.begin(9600);

  pinMode(led_cap_1, OUTPUT);
  pinMode(led_cap_2, OUTPUT);
  pinMode(moduleRelay, OUTPUT);

  digitalWrite(led_cap_1, HIGH);
  digitalWrite(led_cap_2, LOW);
  digitalWrite(moduleRelay, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // KET NOI FIREBASE
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  // dht11
  dht.begin();
}

void loop() {
  // do am
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // read ppm from mq135
  p = mq135_sensor.getPPM();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  if (isnan(p)) {
    Serial.println(F("Failed to read from MQ135!"));
    return;
  }

  // send data from esp32 to firebase
  Firebase.setFloat(firebaseData, temperature, t);
  Firebase.setFloat(firebaseData, humidity, h);
  Firebase.setFloat(firebaseData, ppm, p);

  // lay data tu firebase về esp32
  Firebase.getBool(firebaseData, "/warning");
  bat_canh_bao = firebaseData.boolData();

  // temperature-safe from firebase -> esp32
  Firebase.getFloat(firebaseData, temperature_safe);
  t_safe = firebaseData.floatData();

  // humidity-safe from firebase -> esp32
  Firebase.getFloat(firebaseData, humidity_safe);
  h_safe = firebaseData.floatData();

  // ppm-safe from firebase -> esp32
  Firebase.getFloat(firebaseData, ppm_safe);
  p_safe = firebaseData.floatData();

  // cảnh báo tự động
  if (p > p_safe || h < h_safe || t > t_safe) {
    bat_canh_bao = true;
    Firebase.setBool(firebaseData, warning, bat_canh_bao);

    digitalWrite(led_cap_1, LOW);
    digitalWrite(led_cap_2, HIGH);
    digitalWrite(moduleRelay, HIGH);
  }

  // an toan
  if (p <= p_safe && h >= h_safe && t <= t_safe) {
    digitalWrite(led_cap_2, LOW);
    digitalWrite(led_cap_1, HIGH);
    digitalWrite(moduleRelay, LOW);
    Firebase.setBool(firebaseData, "/warning", false);
    return;
  }

  // nút tắt cảnh báo do người dùng nhấn
  if (bat_canh_bao == false) {
    digitalWrite(led_cap_1, LOW);
    digitalWrite(led_cap_2, HIGH);
    digitalWrite(moduleRelay, LOW);
    // gui tat canh bao len firebase
    Firebase.setBool(firebaseData, "/warning", false);
    return;
  }

  // // nút bật cảnh báo do người dùng bật
  // if (bat_canh_bao == true) {
  //   // so lan canh bao do nguoi dung chon
  //   Firebase.getInt(firebaseData, "/so_lan_canh_bao");
  //   stop = firebaseData.intData();

  //   Firebase.getInt(firebaseData, "/count");
  //   count = firebaseData.intData();
  // }
}