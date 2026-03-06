#include <SPI.h>
#include <LoRa.h>

// --- Definisi Pin Mappi32 ---
// Sesuaikan pin SPI LoRa bawaan Mappi32 jika berbeda
#define ss 15
#define rst 25
#define dio0 27

// Pin Sensor dan Aktuator
#define TRIG_PIN 12
#define ECHO_PIN 13
#define RELAY_PIN 14
#define BUTTON_PIN 33
#define LED_ACK_PIN 2 // Gunakan LED internal untuk indikator ACK

// --- Variabel Logika ---
long duration;
int distance;
int thresholdAirRendah = 20; // Batas jarak air rendah dalam cm (contoh: > 20cm = air surut)
bool pumpStatus = false;
bool manualOverride = false;

unsigned long lastSendTime = 0;
int interval = 2000; // Kirim data tiap 2 detik

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Gunakan internal pull-up untuk button
  pinMode(LED_ACK_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW); // Pompa mati di awal

  // Inisialisasi LoRa
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(915E6)) { // Sesuaikan frekuensi: 433E6, 868E6, atau 915E6
    Serial.println("Gagal inisialisasi LoRa Node!");
    while (1);
  }
  Serial.println("LoRa Node Siap.");
}

void loop() {
  // 1. Baca Sensor Ultrasonik
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  // 2. Baca Input Tombol Manual
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(200); // Debounce sederhana
    manualOverride = !manualOverride; // Toggle status manual
    pumpStatus = manualOverride;
  }

  // 3. Logika Otomatis Pompa (jika tidak dioverride manual)
  if (!manualOverride) {
    if (distance > thresholdAirRendah) {
      pumpStatus = true;  // Air rendah, nyalakan pompa
    } else {
      pumpStatus = false; // Air cukup, matikan pompa
    }
  }

  // Eksekusi Relay
  digitalWrite(RELAY_PIN, pumpStatus ? HIGH : LOW);

  // 4. Kirim Data via LoRa setiap interval
  if (millis() - lastSendTime > interval) {
    String payload = "Dist:" + String(distance) + ",Pump:" + String(pumpStatus);
    
    LoRa.beginPacket();
    LoRa.print(payload);
    LoRa.endPacket();
    
    Serial.println("Mengirim: " + payload);
    lastSendTime = millis();
  }

  // 5. Cek Feedback (ACK) dari Gateway
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    if (incoming == "ACK") {
      Serial.println("Terima ACK dari Gateway");
      digitalWrite(LED_ACK_PIN, HIGH);
      delay(500); 
      digitalWrite(LED_ACK_PIN, LOW);
    }
  }
}