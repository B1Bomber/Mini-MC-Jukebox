#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h>
#include <MFRC522.h>

#define SDA_Pin 10
#define RST_Pin 9

MFRC522 mfrc522(SDA_Pin, RST_Pin);

String RFID1 = "B390698";
String RFID2 = "E39948C6";
String RFID3 = "4263A9A137580";

struct Map {
  String key;
  int value;
};

const Map table[] = {
  {RFID1, 1},
  {RFID2, 2},
  {RFID3, 3},
};

// slightly faster than putting this in lookup() since you only calculate this once
int tableLength = sizeof(table) / sizeof(table[0]);

// Table is very small, so O(n) is constant
int lookup(String s) {
  for (int i = 0; i < tableLength; i++) {
    if (s == (table[i].key)) {
      return table[i].value;
    }
  }
  return 0;
}

int volume = 10;
const int busyPin = 4;
// Use pins 2 and 3 to communicate with DFPlayer Mini
static const int TX_Pin = 2;
static const int RX_Pin = 3;
SoftwareSerial softwareSerial(RX_Pin, TX_Pin);
DFRobotDFPlayerMini player;

void setup() {
  SPI.begin();
  mfrc522.PCD_Init();

  softwareSerial.begin(9600);
  player.begin(softwareSerial);
  player.volume(volume);
}

void loop() {
  playMusic("");
}

// play the music after RFID is detected
void playMusic(String currentRFID) {
  while (currentRFID == "") {
    currentRFID = scanRFID();
  }

  int currentMusic = lookup(currentRFID);
  player.playMp3Folder(currentMusic);

  // While there is something playing
  while (digitalRead(busyPin) == LOW) { 
    // If there is a new card, stop playing
    if (mfrc522.PICC_IsNewCardPresent()){
      player.stop();
      //break;
    }

    delay(100);
  }
}

String scanRFID() {
  String rfid = "";

  // Force the reader to wake up to any card in the field
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);
  
  // Reset the PICC state so it responds to a new request
  mfrc522.PCD_Init(); 

  // Check if a card is there (Request A)
  MFRC522::StatusCode status = mfrc522.PICC_RequestA(bufferATQA, &bufferSize);

  if (status == mfrc522.STATUS_OK && mfrc522.PICC_ReadCardSerial()) {
    // If a card responded, try to read its serial number
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfid.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    rfid.toUpperCase();

    // Stop communication so we can start fresh next loop
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
  
  delay(200);
  return rfid;
}
