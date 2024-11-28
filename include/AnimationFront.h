#pragma once
#include "Buttons.h"
#include "AnimationMode.h"
#include "LEDStrips.h"



unsigned int mid = NUM_LEDS_FRONT_STRIP / 2;
int counterFront = 0;
void WelcomeAnimation()
{
    // Welcome animation
        if(counterFront > 3){
            frontStrip.fill_solid(CRGB::White);
            return;
        }
        int raw = beatsin16(30, 0, mid*4,0,20);
        int sineCount = map(raw,0,mid*4,0,mid);
        Serial.println(raw);
        // Serial.println(sineCount);
        frontStrip[mid-sineCount] = CRGB::White;
        frontStrip[mid + sineCount] = CRGB::White;
        fadeToBlackBy(frontStrip, NUM_LEDS_FRONT_STRIP, 1);
    EVERY_N_MILLISECONDS(100){
        if ( raw <= 5){
            counterFront++;
        }
    }
    // }

}