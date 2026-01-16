#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

int volume = 10;
const int busyPin = 4;
// Use pins 2 and 3 to communicate with DFPlayer Mini
static const int PIN_MP3_TX = 2;
static const int PIN_MP3_RX = 3;
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);
DFRobotDFPlayerMini player;

#define SDA_Pin 10

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  softwareSerial.begin(9600);
  player.begin(softwareSerial);
}

void loop() {
  player.volume(volume);
  player.playMp3Folder(1);
  while (digitalRead(busyPin) == LOW) { 
    delay(10);
  }

}

