# LoRaP2P-IoT-CaAs_CPSLab

# Remote Water Control (LoRa P2P)

## 📖 Ringkasan Proyek

Remote Water Control adalah sistem **monitoring dan kontrol pompa air jarak jauh** menggunakan komunikasi **LoRa Point-to-Point (P2P)**.

Sistem ini terdiri dari dua perangkat utama:

* **Node (Mappi32)** → membaca level air menggunakan sensor ultrasonik dan mengontrol pompa air.
* **Gateway (LoRa Ray V1 / ESP32)** → menerima data dari Node melalui LoRa dan meneruskannya ke platform IoT menggunakan **MQTT**.

Sistem ini cocok digunakan pada area **tanpa koneksi internet di sisi sensor**, seperti:

* sawah
* penampungan air
* daerah terpencil

---

# 🛰 Arsitektur Sistem

```
Ultrasonic Sensor
        │
        ▼
   Mappi32 (Node)
   - Baca level air
   - Kontrol pompa
        │
        │ LoRa P2P
        ▼
LoRa Ray V1 (Gateway)
   - Terima data
   - Kirim ke MQTT
        │
        ▼
   IoT Platform
```

---

# 🔌 Skematik Koneksi

## Node (Mappi32)

### Sensor Ultrasonik

| Sensor | Mappi32 |
| ------ | ------- |
| VCC    | 5V      |
| GND    | GND     |
| TRIG   | GPIO 12 |
| ECHO   | GPIO 13 |

### Relay Pompa

| Relay | Mappi32 |
| ----- | ------- |
| IN    | GPIO 14 |
| VCC   | 5V      |
| GND   | GND     |

### Push Button

| Button | Mappi32 |
| ------ | ------- |
| Pin 1  | GPIO 33 |
| Pin 2  | GND     |

### LED ACK

| LED | Mappi32 |
| --- | ------- |
| +   | GPIO 2  |
| -   | GND     |

### LoRa Node Pins

| LoRa | Mappi32 |
| ---- | ------- |
| NSS  | GPIO 15 |
| RST  | GPIO 25 |
| DIO0 | GPIO 27 |

---

## Gateway (LoRa Ray V1)

| LoRa | ESP32   |
| ---- | ------- |
| NSS  | GPIO 18 |
| RST  | GPIO 14 |
| DIO0 | GPIO 26 |

---

# ⚙ Cara Kerja Sistem

1. Sensor ultrasonik membaca jarak permukaan air.
2. Jika jarak air lebih besar dari threshold → pompa menyala.
3. Node mengirim data melalui LoRa setiap beberapa detik.
4. Gateway menerima data dan menampilkannya di Serial Monitor.
5. Data dikirim ke IoT platform melalui MQTT.
6. Gateway mengirim **ACK** kembali ke Node sebagai konfirmasi penerimaan data.

Contoh data yang dikirim:

```
Dist:25,Pump:1
```

---

# 🛠 Panduan Penggunaan

## 1. Persiapan Software

Install library berikut di Arduino IDE:

* LoRa
* WiFi
* PubSubClient
* SPI

---

## 2. Konfigurasi Gateway

Edit bagian berikut pada kode Gateway:

```
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "broker.hivemq.com";
```

---

## 3. Upload Program

Upload program berikut:

Node:

```
node_mappi32.ino
```

Gateway:

```
gateway_loraray_v1.ino
```

Pastikan **frekuensi LoRa sama** pada Node dan Gateway:

```
LoRa.begin(915E6);
```

---

## 4. Menjalankan Sistem

1. Nyalakan Node.
2. Nyalakan Gateway.
3. Buka **Serial Monitor Gateway**.
4. Data dari Node akan muncul di serial dan dikirim ke MQTT.

Contoh output:

```
Data diterima dari Node: Dist:23,Pump:1
Kirim ACK ke Node
```

---

# Kelompok 2 CaAs CPS Laboratory
