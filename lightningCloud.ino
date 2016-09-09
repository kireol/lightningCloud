#include <elapsedMillis.h>
#include "FastLED.h"

#define NUM_LEDS 60
#define DATA_PIN 6
#define PIR_PIN 5

elapsedMillis timeElapsed;
elapsedMillis timeElapsedSinceOff;
unsigned int animationDurationMillis = 3000; 
unsigned int randomStrikeMinimumMillis = 30000; 

enum Mode {
    CLOUD, ACID, OFF, ON, RED, GREEN, BLUE, FADE
};

Mode mode = OFF;
Mode lastMode = OFF;
int pirCurrentValue = 0;
boolean animationIsPlaying;
#define SAMPLES   10  // Length of buffer for dynamic level adjustment
int n, total = 30;
float average = 0;
int fade_h;
int fade_direction = 1;
CRGB leds[NUM_LEDS];

void setup() {
    Serial.begin(115200);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    pinMode(PIR_PIN, INPUT);
    animationIsPlaying = false;
}

void loop() {
    determineAction();
    runAnimation();
}

void determineAction(){
    pirCurrentValue = digitalRead(PIR_PIN);

    if (animationIsPlaying == true && timeElapsed > animationDurationMillis) {
        animationIsPlaying = false;
        timeElapsedSinceOff = 0;
        mode = OFF;
    } else {
        int chance = 0;

        if (timeElapsedSinceOff > randomStrikeMinimumMillis) {
            chance = random(1, 1000);
        }

        if (chance > 998 || pirCurrentValue == HIGH) {
            Serial.println("ON");
            mode = ON;
            animationIsPlaying = true;
            timeElapsed = 0;
        }
    }
}

void runAnimation(){
    switch (mode) {
        case CLOUD:
            lightningStrikes();
            turnAllLightsOff();
            break;
        case ACID:
            acidCloud();
            turnAllLightsOff();
            break;
        case OFF:
            turnAllLightsOff();
            break;
        case ON:
            constantLightning();
            turnAllLightsOff();
            break;
        case RED:
            singleColor(0);
            break;
        case BLUE:
            singleColor(160);
            break;
        case GREEN:
            singleColor(96);
            break;
        case FADE:
            colorFade();
            break;
        default:
            lightningStrikes();
            turnAllLightsOff();
            break;
    }

}

void singleColor(int H) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(H, 255, 255);
    }

    if (lastMode != mode) {
        FastLED.show();
        lastMode = mode;
    }
    delay(50);
}

void colorFade() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(fade_h, 255, 255);
    }
    if (fade_h > 254) {
        fade_direction = -1; //reverse once we get to 254
    } else if (fade_h < 0) {
        fade_direction = 1;
    }

    fade_h += fade_direction;
    FastLED.show();
    delay(100);
}

void lightningStrikes() {
    average = (total / SAMPLES) + 2;
    if (n > average) {
        Serial.println("TRIGGERED");
        turnAllLightsOff();


        switch (random(1, 3)) {
            case 1:
                thunderburst();
                delay(random(10, 500));
                Serial.println("Thunderburst");
                break;

            case 2:
                rolling();
                Serial.println("Rolling");
                break;

            case 3:
                crack();
                delay(random(50, 250));
                Serial.println("Crack");
                break;
        }
    }
}

void turnAllLightsOff() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(0, 0, 0);
    }
    FastLED.show();
}

void acidCloud() {
    for (int i = 0; i < NUM_LEDS; i++) {
        if (random(0, 100) > 90) {
            leds[i] = CHSV(random(0, 255), 255, 255);

        } else {
            leds[i] = CHSV(0, 0, 0);
        }
    }
    FastLED.show();
    delay(random(5, 100));
    turnAllLightsOff();
}

void rolling() {
    for (int r = 0; r < random(2, 10); r++) {
        for (int i = 0; i < NUM_LEDS; i++) {
            if (random(0, 100) > 90) {
                leds[i] = CHSV(0, 0, 255);

            } else {
                //dont need reset as we're blacking out other LEDs her
                leds[i] = CHSV(0, 0, 0);
            }
        }
        FastLED.show();
        delay(random(5, 100));
        turnAllLightsOff();
    }
}

void crack() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(0, 0, 255);
    }
    FastLED.show();
    delay(random(10, 100));
    turnAllLightsOff();
}

void thunderburst() {
    int rs1 = random(0, NUM_LEDS / 2);
    int rl1 = random(10, 20);
    int rs2 = random(rs1 + rl1, NUM_LEDS);
    int rl2 = random(10, 20);

    for (int r = 0; r < random(3, 6); r++) {
        for (int i = 0; i < rl1; i++) {
            leds[i + rs1] = CHSV(0, 0, 255);
        }
        if (rs2 + rl2 < NUM_LEDS) {
            for (int i = 0; i < rl2; i++) {
                leds[i + rs2] = CHSV(0, 0, 255);
            }
        }

        FastLED.show();
        delay(random(10, 50));
        turnAllLightsOff();
        delay(random(10, 50));
    }
}

void constantLightning() {
    switch (random(1, 10)) {
        case 1:
            thunderburst();
            delay(random(10, 500));
            Serial.println("Thunderburst");
            break;

        case 2:
            rolling();
            Serial.println("Rolling");
            break;

        case 3:
            crack();
            delay(random(50, 250));
            Serial.println("Crack");
            break;

    }
}
