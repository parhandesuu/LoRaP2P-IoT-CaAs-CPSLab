#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>

// --- Definisi Pin LoRa Ray V1 ---
// Sesuaikan dengan pin mapping Cosmic LoRa Ray V1 Anda
#define ss 18
#define rst 14
#define dio0 26

// --- Konfigurasi WiFi & MQTT ---
const char* ssid = "abc";
const char* password = "yasa1234";
const char* mqtt_server = "test.mosquitto.org"; // Ganti dengan broker pilihan (Blynk/ThingsBoard)
// const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "GatewayLoRa-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected to MQTT");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  // Tambahkan tanda petik dua di awal dan di akhir alamat server
client.setServer("test.mosquitto.org", 1883);

  // Inisialisasi LoRa
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(915E6)) { // Samakan frekuensi dengan Node
    Serial.println("Gagal inisialisasi LoRa Gateway!");
    while (1);
  }
  Serial.println("LoRa Gateway Siap Menerima.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Cek apakah ada paket masuk dari Node
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    
    // Tampilkan di Serial Monitor sesuai instruksi
    Serial.print("Data diterima dari Node: ");
    Serial.println(incoming);
    
    // Parse data sederhana (Contoh incoming: "Dist:25,Pump:1")
    // Teruskan ke MQTT
    client.publish("caas/kelompok2/waterdata", incoming.c_str());
    
    // Kirim Balik ACK ke Node
    delay(10); // Jeda singkat sebelum transmisi balik
    LoRa.beginPacket();
    LoRa.print("ACK");
    LoRa.endPacket();
    Serial.println("Kirim ACK ke Node");
  }
}