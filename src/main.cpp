#include <FastLED.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "OneButton.h"
#include "Buttons.h"
#include "LEDStrips.h"
#include "Animation.h"
#include "AnimationMode.h"
// #include <esp_log.h>

#define NUM_STRIPS 3


// Pin definitions
#define STRIP1_PIN 2
#define STRIP2_PIN 16
#define STRIP3_PIN 5

void buttonLoop(void *pvParameters){
    for (;;){
        updateButtons();
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

void setup() {
    Serial.begin(921600);
    setupLED();
    attachButtons();
    xTaskCreate(buttonLoop, "buttonLoop", 2048, NULL, 1, NULL);
}

void playAnimation(AnimationMode_t mode)
{
    static AnimationMode_t ActiveMode = IDLE;
    const char* TAG = "playAnimation";


    if (ActiveMode == IDLE){
        ActiveMode = mode;
    }
    if (isDRLActive && ActiveMode != WELCOME){
        
        frontStrip.fill_solid(CRGB::White);
    }
    ESP_LOGD(TAG, "Active mode: %d Cur mode: %d", ActiveMode, curMode);

    switch (ActiveMode)
    {
    case WELCOME:
        WelcomeAnimation() ? ActiveMode = IDLE : ActiveMode = WELCOME;
        curMode = ActiveMode == IDLE? IDLE : curMode;
        break;
    case SIGNAL_LEFT:
        TurnLeftAnimation() ? ActiveMode = IDLE : ActiveMode = SIGNAL_LEFT;
        break;
    case SIGNAL_RIGHT:
        TurnRightAnimation() ? ActiveMode = IDLE : ActiveMode = SIGNAL_RIGHT;
        break;
    case SIGNAL_BOTH:
        HazardAnimation() ? ActiveMode = IDLE : ActiveMode = SIGNAL_BOTH;
        break;
    case STROBE:
        StrobeAnimation() ? ActiveMode = IDLE : ActiveMode = STROBE;
        break;
    case BYE:
        ByeAnimation() ? ActiveMode = IDLE : ActiveMode = BYE;
        break;
    case KNIGHT_RIDER:
        RunningAnimation() ? ActiveMode = IDLE : ActiveMode = KNIGHT_RIDER;
        break;
    default:
        break;
    }
}

void loop() {
    EVERY_N_MILLISECONDS(10){
        vTaskDelay(5 / portTICK_PERIOD_MS);
        playAnimation(curMode);
    }
    if(isBrakePressed){
        BrakeLight();
    }else{
        if (curMode == IDLE){
            backStrip.fadeToBlackBy(20);
        }
    }
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.delay(2);
    vTaskDelay(5 / portTICK_PERIOD_MS);
}