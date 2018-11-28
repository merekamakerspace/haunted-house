#include "FastLED.h"

#define NUM_LEDS 1
#define DATA_PIN 8

CRGB leds[NUM_LEDS];

#define NUM_OUTPUT_PINS 34
const int startPin = 20;

int OUTPUT_PINS[NUM_OUTPUT_PINS];

#define NUM_SOUNDS 10
#define SOUND_PIN_START 44
int SOUND_PINS[] = {44, 45, 46, 47, 48, 49, 50, 51, 52, 53};

#define NUM_ANALOG_PINS 5
int ANALOG_PINS[] = {3, 4, 5, 6, 7};


#define NUM_INPUT_PINS 16
int INPUT_PINS[] = {9, 10, 11, 12, 13,
                    A0, A1, A2, A3, A4, A5, A6, A7,
                    A8, A9, A10, A11, A12, A13, A14, A15
                   };


int inputVals[NUM_INPUT_PINS];

unsigned long last_send_ops = 0;

bool checkOutputPin(int pin) {
  for (int i = 0; i < NUM_OUTPUT_PINS; i++) {
    if (OUTPUT_PINS[i] == pin) {
      return true;
    }
  }

  return false;
}

bool checkAnalogPin(int pin) {
  for (int i = 0; i < NUM_ANALOG_PINS; i++) {
    if (ANALOG_PINS[i] == pin) {
      return true;
    }
  }
  return false;
}


//Trigger Sound number from 0 - 9
void playSound(int snd) {

  // validate input
  if (snd < 1 || snd > 10) {
    return;
  }
  //Trigger Low, then High again
  digitalWrite(SOUND_PIN_START + snd - 1, LOW);
  delay(100);
  digitalWrite(SOUND_PIN_START + snd - 1, HIGH);
  Serial.print("s,");
  Serial.print(snd);
  Serial.print(",");
  Serial.println(SOUND_PIN_START + snd - 1);
  

}

void setDigitalPin(int pin, int val) {
  if (!checkOutputPin(pin)) {
    Serial.print("d");
    Serial.print(pin);
    Serial.println(",2");
    return;
  }
  if (val > 0) {
    digitalWrite(pin, HIGH);
    Serial.print("d");
    Serial.print(pin);
    Serial.println(",1");
  } else {
    digitalWrite(pin, LOW);
    Serial.print("d");
    Serial.print(pin);
    Serial.println(",0");

  }

}

void setAnalogPin(int pin, int val) {
  if (!checkAnalogPin(pin)) {
    return;
  }

  val = constrain(val, 0, 255);
  analogWrite(pin, val);
  Serial.print("a");
  Serial.print(pin);
  Serial.print(",");
  Serial.println(val);
}

void serialEvent() {
  int val, pin = 0;
  if (Serial.available() > 3) {

    char inp = Serial.read();
    switch (inp) {
      case 'd':
        if (Serial.available() > 2) {
          pin = Serial.parseInt();
          //Serial.print("pin:");
          //Serial.println(pin);
          val = Serial.parseInt();
          //Serial.print("val:");
          //Serial.println(val);
          setDigitalPin(pin, val);
        }
        break;
      case 'a':
        if (Serial.available() > 2) {
          pin = Serial.parseInt();
          //Serial.print("pin:");
          //Serial.println(pin);
          val = Serial.parseInt();
          //Serial.print("val:");
          //Serial.println(val);
          setAnalogPin(pin, val);
        }

        break;
      case 'l':
        if (Serial.available() > 2) {
          pin = Serial.parseInt();
          val = Serial.parseInt();
          Serial.print("l,");
          switch (val) {
            case 0:
              leds[0] = CRGB::Black;
              Serial.println("OFF");
              break;
            case 1:
              leds[0] = CRGB::Red;
              Serial.println("Red");
              break;
            case 2:
              leds[0] = CRGB::Orange;
              Serial.println("Yellow");
              break;
            case 3:
              leds[0] = CRGB::Green;
              Serial.println("Green");
              break;
            default:

              Serial.println(val);

          }

          FastLED.show();
        }
        break;

      case 's':
        val = Serial.parseInt();
        pin = Serial.parseInt();
        
        playSound(val);
        break;

      default:
        Serial.print("Invalid Input: ");
        Serial.println(inp);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, 0, NUM_LEDS);

  Serial.println("Ready,");



  for (int i = 0; i < NUM_OUTPUT_PINS; i++) {
    OUTPUT_PINS[i] = startPin + i;
    pinMode(OUTPUT_PINS[i], OUTPUT);
    digitalWrite(OUTPUT_PINS[i], LOW);

  }

  for (int i = 0; i < NUM_SOUNDS; i++) {
    digitalWrite(SOUND_PINS[i], HIGH);

  }


  for (int i = 0; i < NUM_ANALOG_PINS; i++) {
    pinMode(ANALOG_PINS[i], OUTPUT);
    analogWrite(ANALOG_PINS[i], 0);
  }


  for (int i = 0; i < NUM_INPUT_PINS; i++) {
    pinMode(INPUT_PINS[i], INPUT);
    inputVals[i] = 0;
  }

  leds[0] = CRGB::Black;
  FastLED.show();


}

void checkInputs() {
  for (int i = 0; i < NUM_INPUT_PINS; i++) {
    int val = digitalRead(INPUT_PINS[i]);
    if (val != inputVals[i]) {
      inputVals[i] = val;
      // only send highs for remote
      if (INPUT_PINS[i] == 49) {
        if (val == 0) {
          if (millis() - last_send_ops > 500) {
            Serial.println("i,49,1");
            last_send_ops = millis();
          }

        }

      } else if (val) {
        Serial.print("i,");
        Serial.print(INPUT_PINS[i]);
        Serial.print(",");
        Serial.println(val);

      }

    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  checkInputs();
  delay(30);
}
