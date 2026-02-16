#include <SPI.h>
#include <MFRC522.h>
 
#define SDA_Pin 10
#define RST_Pin 9
MFRC522 mfrc522(SDA_Pin, RST_Pin);   // Create MFRC522 instance.
 
void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Put the card on the reader...");
  Serial.println();
}

void loop() {
  // look for a new card
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // if reading fails exit to prevent hangs
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //Show UID on serial monitor
  String rfid= "";
  Serial.println("UID tag: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.println(mfrc522.uid.uidByte[i], HEX);
    //Serial.println(mfrc522.uid.uidByte[i]);
    rfid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  rfid.toUpperCase();
  Serial.println(rfid);
  delay(50);
} 

