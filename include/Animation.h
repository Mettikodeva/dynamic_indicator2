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


uint8_t lastPosition = 0; 
uint8_t lastPosition2 = 0; 
uint8_t lastPosition3 = 0; 
const uint8_t dividerFront = 16;
const uint8_t dividerMid = 14;
const uint8_t dividerBack = 7;

 

uint8_t commet = NUM_LEDS_FRONT_STRIP / (dividerFront*2);
uint8_t commet2 = NUM_LEDS_MID_STRIP / (dividerMid*2);
uint8_t commet3 = NUM_LEDS_BACK_STRIP / (dividerBack*2);

// CRGBArray<mid> frontStripRight = frontStrip(NUM_LEDS_FRONT_STRIP / 2, NUM_LEDS_FRONT_STRIP);
bool isAnimating = false;

bool WelcomeAnimation()`
{   
    // Welcome animation
    static int counterAnim = 0;
    static int counterFront = 0;
    if(counterFront >= 1){
        frontStrip.fill_solid(CRGB::White);
        counterFront = 0;
        curMode = IDLE;
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
    static int counterAnim = 0;
    static int counterFront = 0;
    if(counterFront >= 1){
        frontStrip.fill_solid(CRGB::Black);
        curMode = IDLE;
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
    ESP_LOGD("BYE", "frontcounter:%d raww:%d\n", counterFront,raww);
    // Serial.println("----");
    CRGB color = raww > 127 ? CRGB::Black: CRGB(127,127,127);
    frontStrip[mid - indexLed - 1] = color;
    frontStrip[mid + indexLed] = color;

    if (abs(lastPosition - indexLed) >= 2){
        if (lastPosition < indexLed){
            for (int i = lastPosition; i < indexLed; i++){
                frontStrip[mid - i - 1] = color;
                frontStrip[mid + i] = color;
            }
        }else{
            for (int i = indexLed; i < lastPosition; i++){
                frontStrip[mid - i - 1] = color;
                frontStrip[mid + i] = color;
            }
        }

    }
    
    // fadeToBlackBy(frontStrip, NUM_LEDS_FRONT_STRIP, 2);
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

bool TurnLeftAnimation()
{
    EVERY_N_MILLISECONDS(N_MILLIS_SIGNAL){
        static int counterAnim = 0;
        ESP_LOGD("TurnLeft", "counterAnim: %d", counterAnim);
        if (counterAnim == dividerFront+1){
            fadeToBlackBy(frontStrip, mid, 255);
            fadeToBlackBy(midStrip, mid2, 255);
            fadeToBlackBy(backStrip, mid3, 255);
            counterAnim = 0;
            return true;
        }
        // // Left turn animation
        // if (counterAnim>mid){
        //     counterAnim = 0;
        //     counterAnimMid = 0;
        //     counterAnimBack = 0;
        //     fill_solid(frontStrip, mid, CRGB::Black);
        //     fill_solid(midStrip, mid2, CRGB::Black);
        //     fill_solid(backStrip, mid3, CRGB::Black);
        //     return true;
        // }

        // uint8_t indexLed = counterAnim;
        // uint8_t indexLed2 = counterAnimMid;
        // uint8_t indexLed3 = counterAnimBack;

        // counterAnim += commet;
        // counterAnimMid += commet2;
        // counterAnimBack += commet3;

        uint16_t raw = beat8(30);
        uint8_t indexLed = map8(raw, 0, dividerFront+1);
        counterAnim = indexLed;
        uint8_t indexLed2 = map8(raw, 0, dividerMid+1);
        uint8_t indexLed3 = map8(raw, 0, dividerBack+1);
        indexLed = indexLed * commet;
        indexLed2 = indexLed2*commet2;
        indexLed3 = indexLed3*commet3;


        // fadeToBlackBy(frontStrip, mid, 64);
        // fadeToBlackBy(midStrip, mid2, 64);
        // fadeToBlackBy(backStrip, mid3, 64);
            
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
    static int counterAnimFront = 0;
    static int counterAnimMid = 0;
    static int counterAnimBack = 0;
    EVERY_N_MILLISECONDS(N_MILLIS_SIGNAL){
        if (counterAnimFront>mid){
            counterAnimFront = 0;
            counterAnimMid = 0;
            counterAnimBack = 0;

            fill_solid(&frontStrip[mid], mid, CRGB::Black);
            fill_solid(&midStrip[mid2], mid2, CRGB::Black);
            fill_solid(&backStrip[mid3+1], mid3, CRGB::Black);
            return true;
        }
    

        uint8_t indexLed = counterAnimFront;
        uint8_t indexLed2 = counterAnimMid;
        uint8_t indexLed3 = counterAnimBack;

        counterAnimFront += commet;
        counterAnimMid += commet2;
        counterAnimBack += commet3;

        fadeToBlackBy(&frontStrip[mid], mid, 64);
        fadeToBlackBy(&midStrip[mid2], mid2, 64);
        fadeToBlackBy(&backStrip[mid3+1], mid3, 64);

        if(indexLed+commet <= mid){
            fill_solid(&frontStrip[mid], commet+ indexLed, CRGB::Yellow);

        }else{
            fill_solid(&frontStrip[mid], mid-indexLed, CRGB::Yellow);
        }
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
        TurnLeftAnimation();
        return TurnRightAnimation();      
        // Hazard animation
        // if (counterAnim>=mid){
        //     counterAnim = 0;
        //     counterAnimMid = 0;
        //     counterAnimBack = 0;
        //     frontStrip.fill_solid(CRGB::Black);
        //     midStrip.fill_solid(CRGB::Black);
        //     backStrip.fill_solid(CRGB::Black);
        //     return true;
        // }

        // uint8_t indexLed = counterAnim;
        // uint8_t indexLed2 = counterAnimMid;
        // uint8_t indexLed3 = counterAnimBack;

        // counterAnim += commet;
        // counterAnimMid += commet2;
        // counterAnimBack += commet3;
        
        // fadeToBlackBy(frontStrip, mid, 64);
        // fadeToBlackBy(midStrip, mid2, 64);
        // fadeToBlackBy(backStrip, mid3, 64);
        // fill_solid(&frontStrip[mid], commet+ indexLed, CRGB::Yellow);
        // fill_solid(&midStrip[mid2], commet2+ indexLed2, CRGB::Yellow);
        // fill_solid(&backStrip[mid3+1 ], commet3+ indexLed3, CRGB::Yellow);

        // fill_solid(&frontStrip[mid -indexLed-commet], indexLed+commet, CRGB::Yellow);
        // fill_solid(&midStrip[mid2 -indexLed2-commet2], indexLed2+commet2, CRGB::Yellow);
        // fill_solid(&backStrip[mid3+1 -indexLed3-commet3], indexLed3+commet3, CRGB::Yellow);

        // for (int i = 0; i < 3; i++){
        //     frontStrip.blur1d(64);
        //     midStrip.blur1d(64);
        //     backStrip.blur1d(64);
        // }
        // return false;
    }
    return false;
}

bool RunningAnimation()
{
    EVERY_N_MILLISECONDS(N_MILLIS_ANIMATION){
        static int counterAnimFront = 0;
        static int counterAnimMid = 0;
        static int counterAnimBack = 0;

        static int counterKnight = 0;
        counterKnight+=2;
        uint8_t indexLed = counterAnimFront*commet;
        uint8_t indexLed2 = counterAnimMid*commet2;
        uint8_t indexLed3 = counterAnimBack*commet3;

        

        if (counterKnight >= 255 ){
            counterKnight = 0;
            counterAnimFront = 0;
            counterAnimMid = 0;
            counterAnimBack = 0;
            return true;
        }

        uint8_t raw = triwave8(counterKnight);
        counterAnimFront = map8(raw, 0, dividerFront*2-1);
        counterAnimMid = map8(raw, 0, dividerMid*2-1);
        counterAnimBack = map8(raw, 0, dividerBack*2-1);


        
        
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
    FastLED.show(255);
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
