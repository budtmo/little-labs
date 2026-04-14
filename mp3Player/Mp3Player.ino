#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "IRremote.h"
#include <LiquidCrystal.h>

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
int receiverPin = 13;
IRrecv irrecv(receiverPin);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

bool isPaused = false;
int8_t currentTrackIndex = -1; // -1 means no track, in case user press next for the first time.

String displayedText = "LCD is ready";

int currentVolume = 15;
bool showingVolume = false;
unsigned long volumeDisplayStart = 0;
const unsigned long VOLUME_DISPLAY_DURATION = 3000; // ms

struct TrackEntry {
  uint32_t   code;
  uint8_t    track;
  const char* name;
};

const TrackEntry tracks[] = {
  {0xF30CFF00,  1, "01.light of the world"},
  {0xE718FF00,  2, "02.ja, so ist mein Gott"},
  {0xA15EFF00,  3, "03.mein fels"},
  {0xF708FF00,  4, "04.f\xFCrchte dich nicht"},
  {0xE31CFF00,  5, "05.lalalass dich nicht"},
  {0xA55AFF00,  6, "06.liebt einander"},
  {0xBD42FF00,  7, "07.nichts unm\xF6glich"},
  {0xAD52FF00,  8, "08.top top top wichtig"},
  {0xB54AFF00,  9, "09.Du gibst frieden"},
  {0xE916FF00, 10, "10.freude freunde"},
};
const uint8_t NUM_TRACKS = sizeof(tracks) / sizeof(tracks[0]);

void printLCD(String text) {
  lcd.clear();
  if (text.length() <= 16) {
    lcd.setCursor(0, 0);
    lcd.print(text);
  } else {
    lcd.setCursor(0, 0);
    lcd.print(text.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(text.substring(16));
  }
}

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while(true); //infinite loop to stop program from running for safety
  }

  myDFPlayer.volume(15);  //Set volume value. From 0 to 30
  Serial.println("DFPlayer Ready");

  IrReceiver.begin(receiverPin, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready");

  lcd.begin(16, 2); // Coloum, Row
  lcd.setCursor(0, 0);
  lcd.print(displayedText);
}

void loop()
{
  // Revert volume display back to song name after timeout
  if (showingVolume && (millis() - volumeDisplayStart >= VOLUME_DISPLAY_DURATION)) {
    showingVolume = false;
    printLCD(displayedText);
  }

  if (IrReceiver.decode()) {
    uint32_t code = IrReceiver.decodedIRData.decodedRawData;
    Serial.print("IR Code: 0x");
    Serial.println(code, HEX);

    bool handled = false;

    // Check track buttons via lookup table (easy to extend: just add a row)
    for (uint8_t i = 0; i < NUM_TRACKS; i++) {
      if (code == tracks[i].code) {
        Serial.print("Play track ");
        Serial.println(tracks[i].track);
        currentTrackIndex = i;
        displayedText = tracks[i].name;
        myDFPlayer.play(tracks[i].track);
        showingVolume = false;
        printLCD(displayedText);
        handled = true;
        break;
      }
    }

    if (!handled) {
      switch (code) {
        case 0xBB44FF00: // Button Fast Back
          Serial.println("Previous Play");
          if (currentTrackIndex <= 0) currentTrackIndex = NUM_TRACKS - 1;
          else currentTrackIndex--;
          displayedText = tracks[currentTrackIndex].name;
          myDFPlayer.previous();
          showingVolume = false;
          printLCD(displayedText);
          break;

        case 0xBC43FF00: // Button Fast Forward
          Serial.println("Next Play");
          if (currentTrackIndex == -1) currentTrackIndex = 0; // it will play first song
          else currentTrackIndex = (currentTrackIndex + 1) % NUM_TRACKS;
          displayedText = tracks[currentTrackIndex].name;
          myDFPlayer.next();
          showingVolume = false;
          printLCD(displayedText);
          break;

        case 0xBF40FF00: // Button Pause/Resume
          if (!isPaused) {
            Serial.println("Pause");
            myDFPlayer.pause();
            isPaused = true;
            showingVolume = false;
            printLCD("Paused");
          } else {
            Serial.println("Resume");
            myDFPlayer.start();
            isPaused = false;
            showingVolume = false;
            printLCD(displayedText); // show current song on resume
          }
          break;

        case 0xB847FF00: // Button Func/Stop
          Serial.println("Stop");
          myDFPlayer.stop();
          showingVolume = false;
          printLCD("Stopped");
          break;

        case 0xB946FF00: // VOL+
          Serial.println("Volume Up");
          myDFPlayer.volumeUp();
          if (currentVolume < 30) currentVolume++;
          showingVolume = true;
          volumeDisplayStart = millis();
          printLCD("Volume: " + String(currentVolume));
          break;

        case 0xEA15FF00: // VOL-
          Serial.println("Volume Down");
          myDFPlayer.volumeDown();
          if (currentVolume > 0) currentVolume--;
          showingVolume = true;
          volumeDisplayStart = millis();
          printLCD("Volume: " + String(currentVolume));
          break;

        default:
          Serial.println("Other button");
          break;
      }
    }

    delay(300); // Avoid immediate repeat / double click
    IrReceiver.resume(); // Ready for next code
  }
}
