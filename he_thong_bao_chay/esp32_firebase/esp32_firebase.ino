#include <Arduino.h>
// FIREBASE LIBRARY
#include "FirebaseESP32.h"
// FIREBASE DECLARE
#define FIREBASE_AUTH "AIzaSyAK0raN4nFl3vqgyeELdID-zOmtfrD7_6A"
#define FIREBASE_HOST "https://iot-project-1c24d-default-rtdb.firebaseio.com"

#define WIFI_SSID "dang_khanh_2k1"
#define WIFI_PASSWORD "25032001"

// create variables
FirebaseData firebaseData;
// biến hứng data
int dataFirebase;
//=============================================================
String temperature_safe = "/temperature-safe";
String humidity_safe = "/humidity-safe";
String ppm_safe = "/ppm-safe";
String temperature = "/temperature";
String humidity = "/humidity";
String ppm = "/ppm";
String warning = "/warning";
// mode 1 : tat he thong do nguoi dung chon
String off = "/mode/off";
// mode 2 : mo he thong do nguoi dung chon
String on = "/mode/on";
// mode 3 : he thong tu dong
String autoo = "/mode/auto";
//=============================================================
String temp_string = "";
String humidity_string = "";
String ppm_string = "";
//=============================================================
float h_safe, t_safe, t, h;
//=============================================================
int p, p_safe, canh_bao = 2, moduleRelay = 4;
//=============================================================
bool bat_canh_bao = false, nguoi_dung_nhan_nut = false, tac_dong_nguoi_dung = false;  // false : tat he thong ; true : mo he thong
bool mode1 = false, mode2 = false, mode3 = true;
//=============================================================
//=============================================================
//=============================================================
void setup() {
  Serial.begin(9600);

  pinMode(canh_bao, OUTPUT);
  pinMode(moduleRelay, OUTPUT);
  digitalWrite(canh_bao, LOW);
  digitalWrite(moduleRelay, LOW);
  ketNoiWifi();
  giaoTiepFireBase();

  if (Firebase.ready()) {
    readDataFromArduino();
    sendDataFromFirebaseToEsp32();
  }
}

void loop() {

  if (mode1 == true) mode1_true();
  if (mode2 == true) mode2_true();
  if (mode3 == true) mode3_true();

  if (Firebase.ready()) {
    readDataFromArduino();
    sendDataFromEsp32ToFirebase();
    sendDataFromFirebaseToEsp32();
  }
  
}

void ketNoiWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void giaoTiepFireBase() {
  // KET NOI FIREBASE
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}



void readDataFromArduino() {
  // Check if any reads failed and exit early (to try again).
  if (Serial.available()) {
    temp_string = Serial.readStringUntil('\n');
    humidity_string = Serial.readStringUntil('\n');
    ppm_string = Serial.readStringUntil('\n');
  }
  t = temp_string.toFloat();
  h = humidity_string.toFloat();
  p = ppm_string.toInt();

  Serial.println("Nhiet do = " + String(t));
  Serial.println("Do am = " + String(h));
  Serial.println("Chat luong khong khi = " + String(p));
  Serial.println("canh bao : " + String(bat_canh_bao));
  if(mode1==true) {
    Serial.println("Mode 1 - OFF");
  }
  if(mode2==true) {
    Serial.println("Mode 2 - ON");
  }
  if(mode3==true) {
    Serial.println("Mode 3 - AUTO");
  }
  Serial.println("-----------------------------------------------------");
}

void sendDataFromEsp32ToFirebase() {
  // send data from esp32 to firebase
  Firebase.setFloat(firebaseData, temperature, t);
  Firebase.setFloat(firebaseData, humidity, h);
  Firebase.setInt(firebaseData, ppm, p);
}

void sendDataFromFirebaseToEsp32() {
  // lay data tu firebase về esp32
  Firebase.getFloat(firebaseData, temperature_safe);
  t_safe = firebaseData.floatData();
  Firebase.getFloat(firebaseData, humidity_safe);
  h_safe = firebaseData.floatData();
  Firebase.getInt(firebaseData, ppm_safe);
  p_safe = firebaseData.intData();

  Firebase.getBool(firebaseData, off);
  mode1 = firebaseData.boolData();
  Firebase.getBool(firebaseData, on);
  mode2 = firebaseData.boolData();
  Firebase.getBool(firebaseData, autoo);
  mode3 = firebaseData.boolData();

  Firebase.getBool(firebaseData, warning);
  bat_canh_bao = firebaseData.boolData();
  
}

void tatCanhBao() {
  // nút tắt cảnh báo do người dùng nhấn
  digitalWrite(canh_bao, LOW);
  digitalWrite(moduleRelay, LOW);
  // gui du lieu len firebase
  bat_canh_bao = false;
}

void moCanhBao() {
  digitalWrite(canh_bao, HIGH);
  digitalWrite(moduleRelay, HIGH);
}

// tat canh bao bang tay
void mode1_true() {
  if (mode1 == true) {
    tatCanhBao();
  }
}

// mo canh bao bang tay
void mode2_true() {
  if (mode2 == true) {
    moCanhBao();
  }
}

// he thong tu dong
void mode3_true() {
  // cảnh báo tự động
  // mở cảnh báo tự động báo động
  if ((p > p_safe || t > t_safe) && bat_canh_bao == false) {
    bat_canh_bao = true;
    Firebase.setBool(firebaseData, warning, bat_canh_bao);
    moCanhBao();
  }
  // tắt cảnh báo tự động nếu nhiệt độ về mức an toàn
  if ((p <= p_safe && t <= t_safe) && bat_canh_bao == true) {
    bat_canh_bao = false;
    Firebase.setBool(firebaseData, warning, bat_canh_bao);
    tatCanhBao();
  }
}
