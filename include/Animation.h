#pragma once
#include "Buttons.h"
#include "AnimationMode.h"
#include "LEDStrips.h"
#include <esp_log.h>

#define N_MILLIS_SIGNAL 80
#define N_MILLIS_WELCOME 80
#define N_MILLIS_ANIMATION 80

uint8_t mid = NUM_LEDS_FRONT_STRIP / 2;
uint8_t mid2 = NUM_LEDS_MID_STRIP / 2;
uint8_t mid3 = NUM_LEDS_BACK_STRIP / 2;

int counterFront = 0;
uint8_t counterAnim = 0;
uint8_t counterAnimMid = 0;
uint8_t counterAnimBack = 0;

uint8_t lastPosition = 0; 
uint8_t lastPosition2 = 0; 
uint8_t lastPosition3 = 0; 
const uint8_t divider = 7;
uint8_t commet = NUM_LEDS_FRONT_STRIP / (divider*2);
uint8_t commet2 = NUM_LEDS_MID_STRIP / (divider*2);
uint8_t commet3 = NUM_LEDS_BACK_STRIP / (divider*2);

// CRGBArray<mid> frontStripRight = frontStrip(NUM_LEDS_FRONT_STRIP / 2, NUM_LEDS_FRONT_STRIP);
bool isAnimating = false;

bool WelcomeAnimation()
{   
    // Welcome animation
    if(counterFront >= 1){
        frontStrip.fill_solid(CRGB::White);
        return true;
    }
    uint8_t rawww = triwave8(counterAnim);
    uint8_t raww = triwave8(rawww);
    uint8_t raw = triwave8(raww);
    if (counterAnim > 255){
        counterAnim = 0;
    }

    int indexLed = map(raw, 0, 255, 0, NUM_LEDS_FRONT_STRIP/2);

    Serial.println(counterAnim++);
    ESP_LOGD("frontcounter", "%d\n", counterFront);
    // Serial.println("----");

    frontStrip[mid - indexLed - 1] = CRGB::White;
    frontStrip[mid + indexLed] = CRGB::White;

    if (abs(lastPosition - indexLed) >= 2){
        if (lastPosition < indexLed){
            for (int i = lastPosition; i < indexLed; i++){
                frontStrip[mid - i - 1] = CRGB::White;
                frontStrip[mid + i] = CRGB::White;
            }
        }else{
            for (int i = indexLed; i < lastPosition; i++){
                frontStrip[mid - i - 1] = CRGB::White;
                frontStrip[mid + i] = CRGB::White;
            }
        }

    }
    
    fadeToBlackBy(frontStrip, NUM_LEDS_FRONT_STRIP, 2);
    if (indexLed <= 1)
    {
        EVERY_N_MILLISECONDS(500)
        {
            ESP_LOGD("frontcounter", "%d\n", counterFront);
            counterFront++;
        }
    }
    lastPosition = indexLed;
    return false;
}

bool ByeAnimation(){
    EVERY_N_MILLISECONDS(100){
        static uint8_t counterBye = 0;
        static uint8_t brightness = 1;
        uint8_t raw = triwave8(counterBye);
        if (counterBye >=254){
            counterBye = 0;
            fill_solid(frontStrip, NUM_LEDS_FRONT_STRIP, CRGB::Black);
            return true;
        }

        if (raw>=254){
            ESP_LOGD("Bye", "brightness: %d", brightness);
            curMode = IDLE;
            brightness = 0;
            delay(500);
        }

        uint8_t indexLed = map8(raw, 0, 15);
        counterBye += 8;
        ESP_LOGD("Bye", "counterBye: %d", counterBye);
        ESP_LOGD("Bye", "raw: %d", raw);
        ESP_LOGD("Bye", "up: %d-", mid + indexLed*commet,mid + indexLed*commet+commet);
        ESP_LOGD("Bye", "brightness: %d", brightness*127);
        // ESP_LOGD("Bye", "down: %d", mid-indexLed*commet);
        fadeToBlackBy(&frontStrip[mid+indexLed*commet], commet , brightness*127);
        fadeToBlackBy(&frontStrip[mid-indexLed*commet-commet],commet, brightness*127);
        return false;
    }
    return false;
}

bool TurnLeftAnimation()
{
    EVERY_N_MILLISECONDS(N_MILLIS_SIGNAL){
        // Left turn animation
        if (counterAnim>mid){
            counterAnim = 0;
            counterAnimMid = 0;
            counterAnimBack = 0;
            fill_solid(frontStrip, mid, CRGB::Black);
            fill_solid(midStrip, mid2, CRGB::Black);
            fill_solid(backStrip, mid3, CRGB::Black);
            return true;
        }

        uint8_t indexLed = counterAnim;
        uint8_t indexLed2 = counterAnimMid;
        uint8_t indexLed3 = counterAnimBack;

        counterAnim += commet;
        counterAnimMid += commet2;
        counterAnimBack += commet3;
        
        fadeToBlackBy(frontStrip, mid, 64);
        fadeToBlackBy(midStrip, mid2, 64);
        fadeToBlackBy(backStrip, mid3, 64);

        fill_solid(&frontStrip[mid - indexLed], indexLed, CRGB::Yellow);
        fill_solid(&midStrip[mid2 - indexLed2], indexLed2, CRGB::Yellow);
        fill_solid(&backStrip[mid3+1 -indexLed3], indexLed3, CRGB::Yellow);

        for (int i = 0; i < 3; i++){
            blur1d(frontStrip, mid, 64);
            blur1d(midStrip, mid2, 64);
            blur1d(backStrip, mid3, 64);
        }
        return false;
    }
    return false;
}

bool TurnRightAnimation()
{   
    EVERY_N_MILLISECONDS(N_MILLIS_SIGNAL){
        if (counterAnim>=mid){
            counterAnim = 0;
            counterAnimMid = 0;
            counterAnimBack = 0;

            fill_solid(&frontStrip[mid], mid, CRGB::Black);
            fill_solid(&midStrip[mid2], mid2, CRGB::Black);
            fill_solid(&backStrip[mid3+1], mid3, CRGB::Black);
            return true;
        }
    

        uint8_t indexLed = counterAnim;
        uint8_t indexLed2 = counterAnimMid;
        uint8_t indexLed3 = counterAnimBack;

        counterAnim += commet;
        counterAnimMid += commet2;
        counterAnimBack += commet3;

        fadeToBlackBy(&frontStrip[mid], mid, 64);
        fadeToBlackBy(&midStrip[mid2], mid2, 64);
        fadeToBlackBy(&backStrip[mid3+1], mid3, 64);

        fill_solid(&frontStrip[mid], commet+ indexLed, CRGB::Yellow);
        fill_solid(&midStrip[mid2], commet2+ indexLed2, CRGB::Yellow);
        fill_solid(&backStrip[mid3+1 ], commet3+ indexLed3, CRGB::Yellow);
        

        for (int i = 0; i < 3; i++){
            blur1d(&frontStrip[mid], mid, 64);
            blur1d(&midStrip[mid2], mid2, 64);
            blur1d(&backStrip[mid3], mid3, 64);
        }
        
        return false;
    }
    return false;
}

bool HazardAnimation()
{
    
    EVERY_N_MILLISECONDS(N_MILLIS_SIGNAL){
            
        // Hazard animation
        if (counterAnim>=mid){
            counterAnim = 0;
            counterAnimMid = 0;
            counterAnimBack = 0;
            frontStrip.fill_solid(CRGB::Black);
            midStrip.fill_solid(CRGB::Black);
            backStrip.fill_solid(CRGB::Black);
            return true;
        }

        uint8_t indexLed = counterAnim;
        uint8_t indexLed2 = counterAnimMid;
        uint8_t indexLed3 = counterAnimBack;

        counterAnim += commet;
        counterAnimMid += commet2;
        counterAnimBack += commet3;
        
        fadeToBlackBy(frontStrip, mid, 64);
        fadeToBlackBy(midStrip, mid2, 64);
        fadeToBlackBy(backStrip, mid3, 64);
        fill_solid(&frontStrip[mid], commet+ indexLed, CRGB::Yellow);
        fill_solid(&midStrip[mid2], commet2+ indexLed2, CRGB::Yellow);
        fill_solid(&backStrip[mid3+1 ], commet3+ indexLed3, CRGB::Yellow);

        fill_solid(&frontStrip[mid -indexLed-commet], indexLed+commet, CRGB::Yellow);
        fill_solid(&midStrip[mid2 -indexLed2-commet2], indexLed2+commet2, CRGB::Yellow);
        fill_solid(&backStrip[mid3+1 -indexLed3-commet3], indexLed3+commet3, CRGB::Yellow);

        for (int i = 0; i < 3; i++){
            frontStrip.blur1d(64);
            midStrip.blur1d(64);
            backStrip.blur1d(64);
        }
        return false;
    }
    return false;
}

bool RunningAnimation()
{
    EVERY_N_MILLISECONDS(N_MILLIS_ANIMATION){
        static int counterKnight = 0;
        counterKnight+=2;
        uint8_t indexLed = counterAnim*commet;
        uint8_t indexLed2 = counterAnim*commet2;
        uint8_t indexLed3 = counterAnim*commet3;

        ESP_LOGD("Running", "ck: %d, counterAnim: %d  %c divider: %d",counterKnight, counterAnim, (counterAnim>=divider*2-1)?'>':'<', divider);

        if (counterKnight >= 255 ){
            counterKnight = 0;
            return true;
        }

        uint8_t raw = triwave8(counterKnight);
        counterAnim = map8(raw, 0, divider*2-1);

        
        
        frontStrip.fadeToBlackBy(64);
        midStrip.fadeToBlackBy(64);
        backStrip.fadeToBlackBy(64);


        fill_solid(&frontStrip[indexLed], commet, CRGB::White);
        fill_solid(&midStrip[indexLed2], commet2, CRGB::White);
        fill_solid(&backStrip[indexLed3], commet3, CRGB::White);

        for (int i = 0; i < 3; i++){
            frontStrip.blur1d(64);
            midStrip.blur1d(64);
            backStrip.blur1d(64);
        }
        return false;
    }
    return false;
}

void BrakeLight()
{
    backStrip.fill_solid(CRGB::Red);
}

bool StrobeAnimation()
{
    // Strobe animation
    static int counterStrobe = 0;
    int strobe = beat8(120);
    if(strobe > 127){
        fill_solid(frontStrip, mid, CRGB::White);
        fill_solid(midStrip, mid2, CRGB::White);
        fill_solid(backStrip, mid3, CRGB::White);
        fill_solid(&frontStrip[mid], mid, CRGB::Blue);
        fill_solid(&midStrip[mid2], mid2, CRGB::Blue);
        fill_solid(&backStrip[mid3], mid3, CRGB::Blue);
    }else{
        fill_solid(frontStrip, mid, CRGB::Blue);
        fill_solid(midStrip, mid2, CRGB::Blue);
        fill_solid(backStrip, mid3, CRGB::Blue);
        fill_solid(&frontStrip[mid], mid, CRGB::White);
        fill_solid(&midStrip[mid2], mid2, CRGB::White);
        fill_solid(&backStrip[mid3], mid3, CRGB::White);
        counterStrobe++;
    }
    if (counterStrobe >= 10){
        counterStrobe = 0;
        return true;
    }
    return false;
}
