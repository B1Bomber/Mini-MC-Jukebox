#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h>
#include <MFRC522.h>

#define SDA_Pin 10
#define RST_Pin 9

MFRC522 mfrc522(SDA_Pin, RST_Pin);

String RFID1 = "B390698";
String RFID2 = "E39948C6";
String RFID3 = "00000000";

int volume = 10;
const int busyPin = 4;
// Use pins 2 and 3 to communicate with DFPlayer Mini
static const int TX_Pin = 2;
static const int RX_Pin = 3;
SoftwareSerial softwareSerial(RX_Pin, TX_Pin);
DFRobotDFPlayerMini player;

struct Map {
  String key;
  int value;
};

const Map table[] = {
  {RFID1, 1},
  {RFID2,  2},
  {RFID3, 3},
};

int tableLength = sizeof(table) / sizeof(table[0]);

int lookup(String s) {
  for (int i = 0; i < tableLength; i++) {
    if (s == (table[i].key)) {
      return table[i].value;
    }
  }
  return 0;
}

String currentRFID;
int currentMusic;

void setup() {
  // debug
  Serial.begin(9600);

  SPI.begin();
  mfrc522.PCD_Init();

  softwareSerial.begin(9600);
  player.begin(softwareSerial);
  player.volume(volume);
}

void loop() {
  // play the music after RFID is detected
  currentRFID = scanRFID();
  currentMusic = lookup(currentRFID);
  player.playMp3Folder(currentMusic);

  // debug
  Serial.println("Current RFID ")
  Serial.print(currentRFID);
  Serial.println("Current Music ");
  Serial.print(currentMusic);
  Serial.println("Busy Pin ")
  Serial.print(digitalRead(busyPin));

  // While there is something playing
  while (digitalRead(busyPin) == LOW) { 
    // If RFID does not match anymore, exit the playing loop
    // currentRFID != scanRFID()
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()){
      // debug
      Serial.println("Halting Player");

      break;
    }
    // Stops the Arduino from interfering with the mp3 player
    delay(1);
  }
}

String scanRFID() {
  // debug
  Serial.println("Scan Begin");

  // ensure there is a new card
  if (!mfrc522.PICC_IsNewCardPresent()) {
    // debug
    Serial.println("Error: No Card");
    return "";
  }
  // if reading fails exit to prevent hangs
  if (!mfrc522.PICC_ReadCardSerial()) {
    // debug 
    Serial.println("Error: Read Fail");
    return "";
  }
  String rfid= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    rfid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  rfid.toUpperCase();

  // debug
  Serial.println("rfid num");
  Serial.println(rfid);

  return rfid;
}
