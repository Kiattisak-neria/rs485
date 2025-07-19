#include <Arduino.h>

// ฟังก์ชันคำนวณ CRC-16 (Modbus RTU)
uint16_t calculateCRC(byte *data, int length) {
  uint16_t crc = 0xFFFF;

  for (int pos = 0; pos < length; pos++) {
    crc ^= (uint16_t)data[pos];
    for (int i = 0; i < 8; i++) {
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }

  return crc;
}

void setup() {
  Serial.begin(9600);

  // ตัวอย่างคำสั่ง Modbus ที่ยังไม่มี CRC
  byte command[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02};

  // คำนวณ CRC
  uint16_t crc = calculateCRC(command, 6); // นับแค่คำสั่ง 6 ไบต์แรก

  // แยกเป็น Low byte และ High byte
  command[6] = crc & 0xFF;        // CRC low byte
  command[7] = (crc >> 8) & 0xFF; // CRC high byte

  // แสดงผลลัพธ์
  Serial.println("Modbus RTU Request (with CRC):");
  for (int i = 0; i < 8; i++) {
    if (command[i] < 0x10) Serial.print("0");
    Serial.print(command[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  
}

