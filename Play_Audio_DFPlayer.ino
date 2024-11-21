
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(4, 5);  // RX, TX
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup() {
  // Start the serial communication
#if (defined ESP32)
  FPSerial.begin(9600, SERIAL_8N1, D3, D2); // ESP32 hardware serial setup
#else
  FPSerial.begin(9600); // For Arduino and ESP8266
#endif
  Serial.begin(115200);

  Serial.println(F("DFPlayer Mini Serial Command Interface"));
  Serial.println(F("Initializing DFPlayer... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(FPSerial, true, true)) {  // Initialize DFPlayer
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to be compatible with ESP8266 watchdog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.setTimeOut(500); // Set serial communication timeout to 500ms
  myDFPlayer.volume(20);  // Set volume value (0~30)
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove any surrounding whitespace

    if (command == "play") {
      Serial.println(F("Playing the first mp3..."));
      myDFPlayer.play(1);  // Play the first mp3 file
    }
    else if (command == "next") {
      Serial.println(F("Playing next mp3..."));
      myDFPlayer.next();  // Play the next mp3 file
    }
    else if (command == "previous") {
      Serial.println(F("Playing previous mp3..."));
      myDFPlayer.previous();  // Play the previous mp3 file
    }
    else if (command == "pause") {
      Serial.println(F("Pausing..."));
      myDFPlayer.pause();  // Pause playback
    }
    else if (command == "resume") {
      Serial.println(F("Resuming playback..."));
      myDFPlayer.start();  // Resume playback from the paused state
    }
    else if (command.startsWith("volume")) {
      int volume = command.substring(7).toInt();  // Extract volume value
      if (volume >= 0 && volume <= 30) {
        Serial.print(F("Setting volume to "));
        Serial.println(volume);
        myDFPlayer.volume(volume);  // Set the volume
      } else {
        Serial.println(F("Invalid volume value. Must be between 0 and 30."));
      }
    }
    else if (command.startsWith("loop")) {
      int track = command.substring(5).toInt();  // Extract track number
      Serial.print(F("Looping track "));
      Serial.println(track);
      myDFPlayer.loop(track);  // Loop a specific track
    }
    else if (command == "stop") {
      Serial.println(F("Stopping playback..."));
      myDFPlayer.stop();  // Stop playback
    }
    else if (command == "status") {
      Serial.print(F("Current Volume: "));
      Serial.println(myDFPlayer.readVolume());  // Print current volume
      Serial.print(F("Current File: "));
      Serial.println(myDFPlayer.readCurrentFileNumber());  // Print the current file number
    }
    else {
      Serial.println(F("Unknown command. Available commands:"));
      Serial.println(F("play - Play the first mp3"));
      Serial.println(F("next - Play the next mp3"));
      Serial.println(F("previous - Play the previous mp3"));
      Serial.println(F("pause - Pause playback"));
      Serial.println(F("resume - Resume playback"));
      Serial.println(F("volume [0-30] - Set volume"));
      Serial.println(F("loop [track number] - Loop a specific track"));
      Serial.println(F("stop - Stop playback"));
      Serial.println(F("status - Display current volume and file number"));
    }
  }

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());  // Print error or status messages from DFPlayer
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println(F("USB Inserted!"));
      break;
    case DFPlayerUSBRemoved:
      Serial.println(F("USB Removed!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Play Finished! Number: "));
      Serial.println(value);
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError: "));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
