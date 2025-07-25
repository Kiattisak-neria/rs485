#include <SoftwareSerial.h>//ไลบรารี่ที่ใช้ในการอ่านเซนเซอร์ 485
#define RE 2 // pin RE ที่ใช้ในการเปิดปิดการรับส่งข้อมูลของเซนเซอร์ 485
#define DE 4 // pin DE ที่ใช้ในการเปิดปิดการรับส่งข้อมูลของเซนเซอร์ 485

const byte soil[] = {0x01 ,0x03 ,0x00 ,0x00 ,0x00 ,0x02 ,0xC4 ,0x0B};//เป็น ID ต่างๆที่ใช้ในการเชื่อมต่อกับเซนเซอร์ 485 
/*รายละเอียด 
 1. Slave ID หรือ ID ของเซนเซอร์
 2. รูปแบบการสื่อสารซึ่งในโค้ดเป็น 0x03 คือ การอ่านข้อมูลจาก Holding Registers 
 3. และ 4. คือ แอดเดรชของค่าเซนเซอร์ในโค้ดนี้ค่าเซนเซอร์ความชื้นในดินอยู่แอดเดรชที่ 0 เลยใช้เป็น 0x00,0x00 และถ้าหากค่าเซนเซอร์เป็นแอดเดรชที่ 1 จะเป็น 0x00,0x01
 4. และ 5. คือ จำนวน register ที่ทำการเก็บข้อมูลซึ่งจำนวนในการเก็บข้อมูลขึ้นอยู่กับปริมาณข้อมูล(หากข้อมูลเป็น integer สามารถใช้แค่ 1 registerได้ แต่เซ็นเซอร์ที่ใช้เป็น float จึงจำเป็นต้องใช้ 2 register)
 6. และ 7. คือ CRC ที่ใช้ในการตัวสอบความสมบูลของข้อมูลซึ่งจะมีแอดเดรชที่ต่างออกไปในแต่ละเซนเซอร์ ซึ่งสามารถตรวจสอบได้จากโค้ด    ที่อยู่ใน */

byte values[20]; //ตัวแปรในการเก็บข้อมูลที่รับมาจากเซนเซอร์
SoftwareSerial mod(16,17); //ขา Rx และ Tx ที่ใช้ในการสื่อสารกับเซนเซอร์

void setup() {
  Serial.begin(115200); //ใช้การสื่อสารแบบ pot อนุกรมเพื่อใช้ในการสื่อสารระหว่างบอร์ดและคอมพิวเตอร์(สามารถตั้งค่าได้)
  mod.begin(4800); //เป็น Baudrate ของเซ็นเซอร์ที่จะทำให้สามารถสื่อสารกับเซ็นเซอร์ได้(เป็นค่าแบบฟิกซึ่งขึ้นอยู่กับเซ็นเซอร์)
  pinMode(RE, OUTPUT);//ตั้งค่าให้ pin RE เป็น OUTPUT
  pinMode(DE, OUTPUT);//ตั้งค่าให้ pin NE เป็น OUTPUT
}

void loop() {
  float val1 ;//สร้างตัวแปรเพื่อเก็บค่าเซ็นเซอร์
  ReadSoil();//อ่านค่าเซ็นเซอร์
  val1 = ((values[3]*256)+values[4]) * 0.1;//ทำการนำค่า alay ที่ 3 และ 4 ซึ่งเป็นค่าของเซ็นเซอร์โดยที่จะมีสูตรดังนี้ 
  //ทำค่าที่ 3 คูณกับ 256 แล้วบวกกับค่าที่ 4 ก็จะได้ค่าที่ได้มาจากเซนเซอร์ซึ่งค่าที่ได้จากเซ็นเซอร์จะเป็น 0 ถึง 1000 จึงต้องทำการคูณกับ 0.1 เพื่อให้เป็น 100
  Serial.print("ความชื้น: ");//ทำการแสดงค่าความชื้นในดิน
  Serial.print(val1);
  Serial.println(" %   ");
  delay(2000);
}

//ฟังชั่นในการอ่านค่าความชื้นในดิน
void ReadSoil() {
  //เริ่มทำการส่งชุดข้อมูลเพื่อใช้ในการอ่านค่าเซ็นเซอร์โดยการให้ค่า DE และ RE เป็น HIGH(หรือก็คือส่งค่า soil[] = {0x01 ,0x03 ,0x00 ,0x00 ,0x00 ,0x02 ,0xC4 ,0x0B} เพื่อใช้ในการอ่านค่าเซนเซอร์ที่มี ID ตามนี้)
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(soil, sizeof(soil)) == 8) //ทำการส่งและตรวจสอบว่าส่งครบทั้งหมด 8 ตัว
  { //ทำการหยุดส่งชุดข้อมูลโดยการให้ค่า DE และ RE เป็น LOW
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 11; i++) {//ทำการวนลูปเพื่อทำการอ่านค่าเซ็นเซอร์
      values[i] = mod.read();//อ่านค่าชุดข้อมูลของเซนเซอร์โดยจะมีชุดข้อมูลดังนี้
      /*0. Slave ID หรือ ID ของเซนเซอร์
        1. รูปแบบการสื่อสารซึ่งในโค้ดเป็น 0x03 คือ การอ่านข้อมูลจาก Holding Registers
        2. จำนวนไบต์ของข้อมูล
        3. และ 4. เป็น register ชุดที่ 1 ที่ทำการเก็บข้อมูลค่าความชื้นในดิน
        5. และ 6. เป็น register ชุดที่ 2
        7. และ 8. เป็นค่า CRC
        9-11 อาจจะไม่จำเป็นต้องมีก็ได้หรือเป็นการอ่านความผิดพลาดของการรับส่งค่าเซนเซอร์*/
      Serial.print(values[i], HEX);//ทำการแสดงชุดข้อมูลที่ได้รับมาจากเซนเซอณืในรูปแบบฐาน 16
      Serial.print(" ");
    }
    Serial.println();
  }
}