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
  // debug
  Serial.begin(9600);
  Serial.println("initialize");

  SPI.begin();
  mfrc522.PCD_Init();

  softwareSerial.begin(9600);
  player.begin(softwareSerial);
  player.volume(volume);
}

void loop() {
  playMusic("", "");
}

// play the music after RFID is detected
void playMusic(String previousRFID, String currentRFID) {
  while (currentRFID == "") {
    currentRFID = scanRFID();
  }

  int currentMusic = lookup(currentRFID);
  player.playMp3Folder(currentMusic);

  // debug
  Serial.print("Current RFID ");
  Serial.println(currentRFID);
  Serial.print("Current Music ");
  Serial.println(currentMusic);
  Serial.print("Busy Pin ");
  Serial.println(digitalRead(busyPin));

  // While there is something playing
  while (digitalRead(busyPin) == LOW) { 
    previousRFID = currentRFID;
    currentRFID = scanRFID();
    // If RFID does not match anymore, exit the playing loop
    // currentRFID != previousRFID
    if (isCardPresent()){
      // debug
      Serial.println("Halting Player");

      player.stop();

      break;
    }
  }
}

String scanRFID() {
  // debug
  Serial.println("Scan Begin");

  String rfid = "";
  // ensure there is a new card and if reading fails exit to prevent hangs
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    // debug
    Serial.println("Error: Read Fail");
    return rfid;
  }

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    rfid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  rfid.toUpperCase();

  // debug
  Serial.print("rfid nummber: ");
  Serial.println(rfid);

  return rfid;
}

bool isCardPresent() {
  // This trick re-selects the card. If it fails, the card is gone.
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);
  MFRC522::StatusCode status = mfrc522.PICC_WakeupA(bufferATQA, &bufferSize);
  
  if (status == MFRC522::STATUS_OK) {
    return mfrc522.PICC_ReadCardSerial(); 
  }
  return false;
}