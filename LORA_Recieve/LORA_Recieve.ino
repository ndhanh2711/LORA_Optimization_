#include <SPI.h>
#include <LoRa.h>

// Định nghĩa chân kết nối với module LoRa
#define ss 7
#define rst 12
#define dio0 3
#define LED 1
void setup() {
  Serial.begin(115200);
  pinMode(1, OUTPUT);
  digitalWrite(LED, LOW);
  while (!Serial);
  Serial.println("LoRa Receiver - Tối ưu tầm xa");

  SPI.begin(10, 8, 5, ss);
  LoRa.setPins(ss, rst, dio0);

  while (!LoRa.begin(433E6)) {
    Serial.println("Đang kết nối với LoRa...");
    delay(500);
  }

  // Cấu hình giống hệt bên phát:
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.setPreambleLength(24);
  LoRa.enableCrc();
  LoRa.setSyncWord(0x22);
  
  Serial.println("LoRa đã sẵn sàng để nhận dữ liệu ở cấu hình tối đa.");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Nháy LED 2 lần
    for (int i = 0; i < 2; i++) {
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(100);
    }

    // In nội dung và thông số RSSI/SNR
    Serial.print("Received: ");
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }
    Serial.print(" | RSSI: ");
    Serial.print(LoRa.packetRssi());
    Serial.print(" dBm | SNR: ");
    Serial.println(LoRa.packetSnr());
  }
}
