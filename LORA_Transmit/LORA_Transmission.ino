#include <SPI.h>
#include <LoRa.h>

#define ss     7
#define rst    12
#define dio0   3
#define BTN    1  // Nút nhấn ở GPIO1

unsigned long lastSendTime = 0;
unsigned long buttonPressedTime = 0;
bool autoMode = false;
int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender - Gửi dữ liệu thủ công và tự động");

  pinMode(BTN, INPUT_PULLUP); // Nút nối GND, nên dùng PULLUP

  SPI.begin(10, 8, 5, ss);
  LoRa.setPins(ss, rst, dio0);

  while (!LoRa.begin(433E6)) {
    Serial.println("Đang kết nối với LoRa...");
    delay(500);
  }

  // Cấu hình tối ưu khoảng cách
  LoRa.setTxPower(17);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.setPreambleLength(24);
  LoRa.enableCrc();
  LoRa.setSyncWord(0x22);

  Serial.println("LoRa đã sẵn sàng.");
  Serial.println("Chế độ hiện tại: GỬI THỦ CÔNG từ Serial");
}

void loop() {
  // Kiểm tra nút nhấn chuyển chế độ
  checkButtonModeSwitch();

  if (autoMode) {
    if (millis() - lastSendTime > 3000) {  // mỗi 3s
      lastSendTime = millis();
      String data = "Counter: " + String(counter++);
      Serial.print("Gửi tự động: ");
      Serial.println(data);

      LoRa.beginPacket();
      LoRa.print(data);
      LoRa.endPacket();
    }
  } else {
    // Chế độ gửi thủ công qua Serial
    if (Serial.available()) {
      String inputText = Serial.readStringUntil('\n');
      Serial.print("Gửi: ");
      Serial.println(inputText);

      LoRa.beginPacket();
      LoRa.print(inputText);
      LoRa.endPacket();
    }
  }
}

void checkButtonModeSwitch() {
  static bool buttonHeld = false;

  if (digitalRead(BTN) == HIGH) {  // Nút được nhấn (HIGH do pulldown)
    if (!buttonHeld) {
      buttonPressedTime = millis();
      buttonHeld = true;
    } else if (millis() - buttonPressedTime >= 3000) {  // Giữ ≥ 3s
      autoMode = !autoMode;  // Chuyển chế độ
      counter = 0;
      Serial.print("Đã CHUYỂN chế độ sang: ");
      Serial.println(autoMode ? "GỬI TỰ ĐỘNG" : "GỬI THỦ CÔNG từ Serial");

      // Chờ thả nút để tránh chuyển liên tục
      while (digitalRead(BTN) == HIGH);
      delay(200);
      buttonHeld = false;
    }
  } else {
    buttonHeld = false;
  }
}
