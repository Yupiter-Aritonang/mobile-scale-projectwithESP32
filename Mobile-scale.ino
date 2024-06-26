#include <Wire.h>
#include "HX711.h"
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;

const char* ssid = "Arpihh";
const char* password = "123456789";
const char* FIREBASE_PROJECT_ID = "weight-a54c7";

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
FirebaseData firebaseData;

HX711 scale;
float calibration_factor = 7050;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(1000);
  Serial.println("");
  Serial.println("Wifi Connected");
  delay(1000);

  config.host = "https://weight-a54c7-default-rtdb.asia-southeast1.firebasedatabase.app/";
  config.api_key = "AIzaSyA4AFiZsZhiTtZKIKZ3u8BxdK8vbX3D6t4";
  auth.user.email = "sofyan@gmail.com";
  auth.user.password = "12345678";
  Firebase.begin(&config, &auth);
  if (Firebase.ready()) {
    Serial.println("Connected to Firebase");
  } else {
    Serial.println("Failed to Connect to firebase");
  }
  delay(1000);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); // Reset the scale to 0

  Serial.println("Scale is ready.");
}

void loop() {
   unsigned long currentTime = millis();

   if (Firebase.ready() && (currentTime - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = currentTime;

      if (scale.is_ready()) {
      float weight = scale.get_units(5); // Dapatkan nilai dalam satuan berat (kilogram)

      // Jika nilai berat kurang dari threshold, anggap sebagai 0
      if (weight < 0.1 && weight > -0.1) {
        weight = 0;
      }

      Serial.print("Weight: ");
      Serial.print(weight);
      Serial.println(" kg");

    } else {
      Serial.println("HX711 not found.");
    }
    float weight = scale.get_units(5);
    if (!isnan(weight)) {
      sendWeightToFirebase(weight);

      Serial.println(weight);

      delay(500);

    }
  }
}

void sendWeightToFirebase(float weight) {
  if (Firebase.ready()) {
    float roundedWeight = round(weight * 100.0) / 100.0;
    if (Firebase.RTDB.setInt(&firebaseData, "/weight", roundedWeight)) {
      Serial.println("Data Berat berhasil dikirim ke Firebase!");
    } else {
      Serial.println("Gagal mengirim data Berat ke Firebase!");
    }
  }
}
