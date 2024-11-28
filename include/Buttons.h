#pragma once
#include "OneButton.h"
#include "AnimationMode.h"

#define PIN_DRL  27
#define PIN_SIGNAL_LEFT  13
#define PIN_SIGNAL_RIGHT  18
#define PIN_HAZARD  14
#define PIN_BRAKE  15

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

QueueHandle_t AnimationQueue;

AnimationMode mode;

bool isBrakePressed = false;
bool isHazardPressed = false;

void pressStartCallback(void* button){
    OneButton* btn = (OneButton*)button;
    // Serial.println("Button pressed");

    switch (btn->pin())
    {
    case PIN_DRL:
        Serial.println("DRL button pressed");
        mode = WELCOME;
        xQueueSend(AnimationQueue, &mode, 0);
        break;

    case PIN_SIGNAL_LEFT:
        Serial.println("Signal left button pressed");
        break;

    case PIN_SIGNAL_RIGHT:
        Serial.println("Signal right button pressed");
        break;
    case PIN_HAZARD:   
        Serial.println("Hazard button pressed");
        break;
    case PIN_BRAKE:
        Serial.println("Brake button pressed");
        break;
    default:
        break;
    }


  

}


void pressStopCallback(void* button){
    OneButton* btn = (OneButton*)button;


   switch (btn->pin())
   {
    case PIN_DRL:
         Serial.println("DRL button released");
         break;
    case PIN_SIGNAL_LEFT:
        Serial.println("Signal left button released");
        break;
    case PIN_SIGNAL_RIGHT:
        Serial.println("Signal right button released");
        break;
    case PIN_HAZARD:   
        Serial.println("Hazard button released");
        break;
    case PIN_BRAKE:
        Serial.println("Brake button released");
        break;
    default:
        break;
   }
    

}

void attachButtons(){


    AnimationQueue = xQueueCreate(5, sizeof(AnimationMode));
    
    drlButton->attachLongPressStart(pressStartCallback, drlButton);
    signalLeftButton->attachLongPressStart(pressStartCallback, signalLeftButton);
    signalRightButton->attachLongPressStart(pressStartCallback, signalRightButton);
    hazardButton->attachLongPressStart(pressStartCallback, hazardButton);
    brakeButton->attachLongPressStart(pressStartCallback, brakeButton);

    drlButton->attachLongPressStop(pressStopCallback, drlButton);
    signalLeftButton->attachLongPressStop(pressStopCallback, signalLeftButton);
    signalRightButton->attachLongPressStop(pressStopCallback, signalRightButton);
    hazardButton->attachLongPressStop(pressStopCallback, hazardButton);
    brakeButton->attachLongPressStop(pressStopCallback, brakeButton);

    
    // hazardButton->setClickMs(600);
    hazardButton->setPressMs(400);
    signalLeftButton->setPressMs(350);
    signalRightButton->setPressMs(350);
    drlButton->setPressMs(500);
    brakeButton->setPressMs(50);
    Serial.println("Buttons attached");
}

void updateButtons(){
    long start = micros();
    drlButton->tick();
    signalLeftButton->tick();
    signalRightButton->tick();
    hazardButton->tick();
    brakeButton->tick();
    long lapse = micros()-start;
}

void (*Subscribers[3])() = {NULL, NULL, NULL};

void addSubscriber(void (*callback)(),  int index){
    Subscribers[index] = callback;
}

void notifyHazardPressed(){
    isHazardPressed = true;
    for(int i = 0; i < 3; i++){
        if(Subscribers[i] != NULL){
            Subscribers[i]();
        }
    }
}

void notifyBrakePressed(){

    isBrakePressed = true;
}