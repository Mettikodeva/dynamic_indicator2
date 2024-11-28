#include <FastLED.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "OneButton.h"
#include "Buttons.h"
#include "LEDStrips.h"
#include "AnimationFront.h"

#define NUM_STRIPS 3


// Pin definitions
#define STRIP1_PIN 2
#define STRIP2_PIN 16
#define STRIP3_PIN 5

// Semaphore for thread-safe operations
SemaphoreHandle_t xStripMutex;

// Queue for button events
QueueHandle_t buttonEventQueue;

// LED strip definitions


void setup() {
    Serial.begin(921600);
    // Create mutex for thread-safe strip access
    xStripMutex = xSemaphoreCreateMutex();
    setupLED();
    // Create button event queue
    buttonEventQueue = xQueueCreate(5, sizeof(uint32_t));
    attachButtons();
}

// RTOS will handle tasks, so loop() can be minimal
void loop() {
    long start = micros();
    updateButtons();
    // EVERY_N_MILLISECONDS(1000) {

    //     Serial.println("Loop");
    // }
    WelcomeAnimation();
    long start2 = micros();
    FastLED.show();
    long end = micros();
    long loopTime = end - start;
    long showTime = end - start2;
    Serial.print((float)loopTime/1000.f); Serial.println("ms");
    // Serial.print("SHOW: ");
    // Serial.print((float)showTime / 1000.f);
    // Serial.println("ms");
    // vTaskDelay(2 / portTICK_PERIOD_MS);
}