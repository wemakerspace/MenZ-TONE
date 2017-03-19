#include <TimerOne.h>
#include <avr/pgmspace.h>
#include "pitches.h"
#include "songs.h"

const int PowerLedPin = 13;      // the number of the LED pin
const int ledPin = 2;      // the number of the LED pin
const int anode_pins[] = {9, 10, 11, 5, 6, 7, 8};    // アノードに接続するArduinoのピン
//const int digits[] = {
volatile int digits[] = {
  0b01000000, // 0
  0b01110011, // 1
  0b00100100, // 2
  0b00100001, // 3
  0b00010011, // 4
  0b00001001, // 5
  0b00001000, // 6
  0b01000011, // 7
  0b00000000, // 8
  0b00000001, // 9
};

const int buttonPin = 3;    // the number of the pushbutton pin
int buttonState = 0;             // the current reading from the input pin

const int upButtonPin = 4;    // the number of the pushbutton pin
int upButtonState;             // the current reading from the input pin
int lastUpButtonState = LOW;             // the current reading from the input pin

long upButtonDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

volatile int currentSong = 0;
int currentPosition = 0;
int readyTone = 1;
int nowNote;

void ledBlink() {
  static boolean output = LOW;  // プログラム起動前に１回だけHIGH(1)で初期化される
  digitalWrite(PowerLedPin, output);      // 13番ピン(LED)に出力する(HIGH>ON LOW>OFF)
  if(output){
    for (int i = 0; i < (sizeof(anode_pins) / sizeof(anode_pins[0])); i++) {
      digitalWrite(anode_pins[i], digits[currentSong] & (1 << i) ? HIGH : LOW);
    }
  } else {
    for (int i = 0; i < (sizeof(anode_pins) / sizeof(anode_pins[0])); i++) {
      digitalWrite(anode_pins[i], HIGH);
    }
  }
  output = !output;              // 現在のoutput内容を反転(HIGH→LOW/LOW→HIGH)させoutputにセットする
}

void setup() {
  pinMode(PowerLedPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  PORTD = B11100000;

//  digitalWrite(PowerLedPin, HIGH);
//  Timer1.initialize(1000000); //マイクロ秒単位で設定
  Timer1.initialize(15000); //マイクロ秒単位で設定
  Timer1.attachInterrupt(ledBlink);

  pinMode(buttonPin, INPUT);
  pinMode(upButtonPin, INPUT);

  Serial.begin(9600);
  Serial.println("DEBUG DEBUG DEBUG");
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  int reading = digitalRead(upButtonPin);
  if (reading != lastUpButtonState) {
    upButtonDebounceTime = millis();
  }

  if ((millis() - upButtonDebounceTime) > debounceDelay) {
    if (reading != upButtonState) {
      upButtonState = reading;

      if (upButtonState == HIGH) {
        //曲の頭出し
        currentPosition = 0;
        currentSong++;
        if(currentSong >= songNum) currentSong = 0;
      }
    }
  }

  lastUpButtonState = reading;

  // ONされて一回だけ実行
  if(buttonState == 1 && readyTone == 1){
    digitalWrite(ledPin, HIGH);
    nowNote = pgm_read_word(&melody[currentSong][currentPosition]);
    // 最後の音まで来たらリセット
    if(nowNote == 0){
      // Serial.print("if melody_size: ");
      // Serial.println(melody_size);
      Serial.print("if currentPosition: ");
      Serial.println(currentPosition);
      currentPosition = 0;
      nowNote = pgm_read_word(&melody[currentSong][currentPosition]);
    }

    tone(14, nowNote);
    Serial.print("tone: ");
    Serial.println(nowNote);
    Serial.print("currentPosition: ");
    Serial.println(currentPosition);

    // ifを何度も実行しないようにフラグを立てる
    readyTone = 0;

  }

  // OFFされて一回だけ実行
  if (buttonState == 0 && readyTone == 0) {
    digitalWrite(ledPin, LOW);
    currentPosition++;
    noTone(14);
    delay(50);
    readyTone = 1;
  }

}
