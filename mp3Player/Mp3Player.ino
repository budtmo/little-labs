#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "IRremote.h"

SoftwareSerial mySoftwareSerial(3, 4); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
int receiverPin = 2;
IRrecv irrecv(receiverPin);

bool isPaused = false;

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while(true); //infinite loop to stop program from running for safety
  }

  myDFPlayer.volume(5);  //Set volume value. From 0 to 30
  Serial.println("DFPlayer Ready");

  IrReceiver.begin(receiverPin, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready");
}

void loop()
{
  if (IrReceiver.decode()) {
    uint32_t code = IrReceiver.decodedIRData.decodedRawData;
    Serial.print("IR Code: 0x");
    Serial.println(code, HEX);
    switch (code) {
      case 0xF30CFF00: // Button 1
        Serial.println("Play track 1");
        myDFPlayer.play(1);
        break;

      case 0xE718FF00: // Button 2
        Serial.println("Play track 2");
        myDFPlayer.play(2);
        break;

      case 0xA15EFF00: // Button 3
        Serial.println("Play track 3");
        myDFPlayer.play(3);
        break;

      case 0xF708FF00: // Button 4
        Serial.println("Play track 4");
        myDFPlayer.play(4);
        break;

      case 0xE31CFF00: // Button 5
        Serial.println("Play track 5");
        myDFPlayer.play(5);
        break;

      case 0xA55AFF00: // Button 6
        Serial.println("Play track 6");
        myDFPlayer.play(6);
        break;

      case 0xBD42FF00: // Button 7
        Serial.println("Play track 7");
        myDFPlayer.play(7);
        break;

      case 0xAD52FF00: // Button 8
        Serial.println("Play track 8");
        myDFPlayer.play(8);
        break;

      case 0xB54AFF00: // Button 9
        Serial.println("Play track 9");
        myDFPlayer.play(9);
        break;

      case 0xE916FF00: // Button 0
        Serial.println("Play track 10");
        myDFPlayer.play(10);
        break;

      case 0xBB44FF00: // Button Fast Back
        Serial.println("Previous Play");
        myDFPlayer.previous();
        break;

      case 0xBC43FF00: // Button Fast Forward
        Serial.println("Next Play");
        myDFPlayer.next();
        break;

      case 0xBF40FF00: // Button Pause/Resume
        if (!isPaused) {
          Serial.println("Pause");
          myDFPlayer.pause();
          isPaused = true;
        } else {
          Serial.println("Resume");
          myDFPlayer.start();
          isPaused = false;
        }
        break;

      case 0xB847FF00: // Button Func/Stop
        Serial.println("Stop");
        myDFPlayer.stop();
        break;

      case 0xB946FF00: // VOL+
        Serial.println("Volume Up");
        myDFPlayer.volumeUp();   // increases current volume by 1
        break;

      case 0xEA15FF00: // VOL-
        Serial.println("Volume Down");
        myDFPlayer.volumeDown(); // decreases current volume by 1
        break;

      default:
        Serial.println("Other button");
        break;
    }
    delay(300); // Do not get immediate repeat and avoid double click
    IrReceiver.resume(); // Ready for next code
  }
}
