#pragma once
#include "OneButton.h"
#include "AnimationMode.h"
#include <esp_log.h>

#define PIN_DRL 27
#define PIN_SIGNAL_LEFT 13
#define PIN_SIGNAL_RIGHT 18
#define PIN_HAZARD 14
#define PIN_BRAKE 15

enum ButtonType
{
    DRL_BUTTON,
    SIGNAL_LEFT_BUTTON,
    SIGNAL_RIGHT_BUTTON,
    HAZARD_BUTTON,
    HAZARD_BUTTON_TWICE,
    BRAKE_BUTTON
};

OneButton *drlButton = new OneButton(PIN_DRL, true, true);
OneButton *signalLeftButton = new OneButton(PIN_SIGNAL_LEFT, true, true);
OneButton *signalRightButton = new OneButton(PIN_SIGNAL_RIGHT, true, true);
OneButton *hazardButton = new OneButton(PIN_HAZARD, true, true);
OneButton *brakeButton = new OneButton(PIN_BRAKE, true, true);

AnimationMode_t curMode = IDLE;
AnimationMode_t prevMode = IDLE;
bool isBrakePressed = false;
bool isHazardPressed = false;
bool isChanged = false;
bool isDRLActive = false;

void pressStartCallback(void *button)
{
    OneButton *btn = (OneButton *)button;
    // Serial.println("Button pressed");
    prevMode = curMode;
    const char *TAG = "pressStartCallback";
    int pin = btn->pin();
    if (pin == PIN_BRAKE)
    {
        isBrakePressed = true;
        // ESP_LOGD(TAG,"Brake button pressed");
    }
    if (pin == PIN_DRL)
    {
        isDRLActive = true;
        curMode = WELCOME;
        ESP_LOGD(TAG, "DRL button pressed");
    }
    if (pin == PIN_HAZARD)
    {
        curMode = SIGNAL_BOTH;
        // ESP_LOGD(TAG,"Hazard button pressed");
    }
    else if (pin == PIN_SIGNAL_LEFT || pin == PIN_SIGNAL_RIGHT)
    {
        if (pin == PIN_SIGNAL_LEFT)
        {
            curMode = SIGNAL_LEFT;
            // ESP_LOGD(TAG,"Signal left button pressed");
        }
        else
        {
            curMode = SIGNAL_RIGHT;
            // ESP_LOGD(TAG,"Signal right button pressed");
        }
    }
}

void pressStopCallback(void *button)
{
    OneButton *btn = (OneButton *)button;

    int pin = btn->pin();
    const char *TAG = "pressStopCallback";
    if (pin == PIN_BRAKE)
    {
        isBrakePressed = false;
        // ESP_LOGD(TAG,"Brake button released");
    }
    if (pin == PIN_DRL)
    {
        isDRLActive = false;
        curMode = BYE;
        // ESP_LOGD(TAG,"DRL button released");
    }

    if (pin == PIN_HAZARD)
    {
        curMode = IDLE;
        // ESP_LOGD(TAG,"Hazard button released");
    }
    else if (pin == PIN_SIGNAL_LEFT || pin == PIN_SIGNAL_RIGHT)
    {
        if (curMode == SIGNAL_LEFT || curMode == SIGNAL_RIGHT)
        {
            curMode = IDLE;
        }
        // ESP_LOGD(TAG,"Signal button released");
    }
}

void doubleClickCallback(void *button)
{
    OneButton *btn = (OneButton *)button;
    const char *TAG = "doubleClick";
    // ESP_LOGD(TAG, "Double click");
    uint8_t pin = btn->pin();
    if (pin == PIN_HAZARD)
    {
        curMode = STROBE;
    }
    if (pin == PIN_DRL)
    {
        curMode = KNIGHT_RIDER;
    }
}

void attachButtons()
{

    drlButton->attachLongPressStart(pressStartCallback, drlButton);
    drlButton->attachDoubleClick(doubleClickCallback, drlButton);
    signalLeftButton->attachLongPressStart(pressStartCallback, signalLeftButton);
    signalRightButton->attachLongPressStart(pressStartCallback, signalRightButton);
    hazardButton->attachLongPressStart(pressStartCallback, hazardButton);
    hazardButton->attachDoubleClick(doubleClickCallback, hazardButton);
    brakeButton->attachLongPressStart(pressStartCallback, brakeButton);

    drlButton->attachLongPressStop(pressStopCallback, drlButton);
    signalLeftButton->attachLongPressStop(pressStopCallback, signalLeftButton);
    signalRightButton->attachLongPressStop(pressStopCallback, signalRightButton);
    hazardButton->attachLongPressStop(pressStopCallback, hazardButton);
    brakeButton->attachLongPressStop(pressStopCallback, brakeButton);

    hazardButton->setClickMs(450);
    hazardButton->setPressMs(400);
    signalLeftButton->setPressMs(350);
    signalRightButton->setPressMs(350);
    drlButton->setPressMs(400);
    drlButton->setClickMs(350);
    brakeButton->setPressMs(20);
    Serial.println("Buttons attached");
}

void updateButtons()
{
    long start = micros();
    drlButton->tick();
    signalLeftButton->tick();
    signalRightButton->tick();
    hazardButton->tick();
    brakeButton->tick();
    long lapse = micros() - start;
}

// void (*Subscribers[3])() = {NULL, NULL, NULL};

// void addSubscriber(void (*callback)(),  int index){
//     Subscribers[index] = callback;
// }

// void notifyHazardPressed(){
//     isHazardPressed = true;
//     for(int i = 0; i < 3; i++){
//         if(Subscribers[i] != NULL){
//             Subscribers[i]();
//         }
//     }
// }
