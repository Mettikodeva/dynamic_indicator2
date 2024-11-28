#include <FastLED.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "OneButton.h"


#define NUM_STRIPS 3
#define LEDS_PER_STRIP1 30
#define LEDS_PER_STRIP2 20
#define LEDS_PER_STRIP3 15

// Pin definitions
#define STRIP1_PIN 2
#define STRIP2_PIN 16
#define STRIP3_PIN 5
// #define BUTTON1_PIN 15
// #define BUTTON2_PIN 16
// #define BUTTON3_PIN 17

// Semaphore for thread-safe operations
SemaphoreHandle_t xStripMutex;

// Queue for button events
QueueHandle_t buttonEventQueue;

enum AnimationMode {
    RAINBOW,
    PULSE,
    CHASE,
    GRADIENT,
    SPARKLE,
    RUNNING_LIGHT
};

class StripController {
private:
    CRGB* leds;
    int numLeds;
    AnimationMode currentMode;
    uint16_t animationCounter;
    bool isRunningLightDirectionForward = true;


public:
    StripController(int ledCount) : 
        numLeds(ledCount), 
        currentMode(CHASE),
        animationCounter(0) {
        leds = new CRGB[numLeds];
    }

    void setMode(AnimationMode mode) {
        if (xSemaphoreTake(xStripMutex, portMAX_DELAY) == pdTRUE) {
            currentMode = mode;
            animationCounter = 0;
            isRunningLightDirectionForward = true;
            xSemaphoreGive(xStripMutex);
        }


    }

    void updateAnimation() {
        if (xSemaphoreTake(xStripMutex, portMAX_DELAY) == pdTRUE) {
            switch(currentMode) {
                case RUNNING_LIGHT:
                    runningLightAnimation();
                    break;
                case RAINBOW:
                    rainbowAnimation();
                    break;
                case PULSE:
                    pulseAnimation();
                    break;
                case CHASE:
                    chaseAnimation();
                    break;
                case GRADIENT:
                    gradientAnimation();
                    break;
                case SPARKLE:
                    sparkleAnimation();
                    break;
            }
            animationCounter++;
            xSemaphoreGive(xStripMutex);
        }
    }

    CRGB* getLEDs() { return leds; }

private:
  void runningLightAnimation() {
    // Clear the strip
    FastLED.clear();

    // Window size for dynamic indicator
    const int WINDOW_SIZE = 5;
    
    // Calculate middle point
    int midPoint = numLeds / 2;
    
    // Simultaneously run to both sides from the middle
    int leftPos = midPoint - animationCounter;
    int rightPos = midPoint + animationCounter;

    // Create the running light effect with a window on both sides
    for (int offset = 0; offset < WINDOW_SIZE; offset++) {
        // Left side
        if (leftPos - offset >= 0) {
            uint8_t brightness = map(
                offset, 
                0, WINDOW_SIZE, 
                255, 50  // Full brightness at center, dimmer at edges
            );
            
            leds[leftPos - offset] = CHSV(
                HUE_YELLOW,     // Yellow hue
                255,            // Full saturation
                brightness      // Dynamic brightness
            );
        }

        // Right side
        if (rightPos + offset < numLeds) {
            uint8_t brightness = map(
                offset, 
                0, WINDOW_SIZE, 
                255, 50  // Full brightness at center, dimmer at edges
            );
            
            leds[rightPos + offset] = CHSV(
                HUE_YELLOW,     // Yellow hue
                255,            // Full saturation
                brightness      // Dynamic brightness
            );
        }
    }

    // Reset when reaching strip ends
    if (rightPos >= numLeds - 1) {
        animationCounter = 0;
    }
}

    // Animation methods remain the same as in previous example
    void rainbowAnimation() {
        fill_rainbow(leds, numLeds, animationCounter, 7);
    }

    void pulseAnimation() {
        for (int i = 0; i < numLeds; i++) {
            leds[i] = CHSV(
                120,  // Green hue
                255,  // Full saturation
                sin8(animationCounter + (i * 10))  // Pulsing brightness
            );
        }
    }

    void chaseAnimation() {
        fadeToBlackBy(leds, numLeds, 20);
        int pos = beatsin16(13, 0, numLeds - 1);
        leds[pos] += CRGB::Yellow;
    }

    void gradientAnimation() {
        CRGB color1 = CRGB::Blue;
        CRGB color2 = CRGB::Purple;
        
        for (int i = 0; i < numLeds; i++) {
            leds[i] = blend(color1, color2, map(i, 0, numLeds, 0, 255));
        }
    }

    void sparkleAnimation() {
        fadeToBlackBy(leds, numLeds, 10);
        
        if (random8() < 10) {
            int pos = random16(numLeds);
            leds[pos] += CRGB(
                random8(100, 255),
                random8(100, 255),
                random8(100, 255)
            );
        }
    }
};

// Global strip controllers and task handles
StripController* strips[NUM_STRIPS];
TaskHandle_t animationTaskHandles[NUM_STRIPS];
TaskHandle_t buttonTaskHandle;

// Animation task for each strip
void animationTask(void* parameter) {
    int stripIndex = *((int*)parameter);
    
    for (;;) {
        // Update specific strip animation
        strips[stripIndex]->updateAnimation();
        Serial.print("Updating animation : ");
        Serial.println(stripIndex);

        
        // Delay to control animation speed
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

// Button event handling task
void buttonTask(void* parameter) {
    uint32_t buttonEvent;
    
    for (;;) {
        // Wait for button event
        if (xQueueReceive(buttonEventQueue, &buttonEvent, portMAX_DELAY) == pdTRUE) {
            // Cycle through modes for all strips
            static int currentMode = 0;
            currentMode = (currentMode + 1) % 5;

            for (int i = 0; i < NUM_STRIPS; i++) {
                strips[i]->setMode(static_cast<AnimationMode>(currentMode));
            }
        }
    }
}

// Interrupt handler for button press
void IRAM_ATTR buttonInterruptHandler() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t buttonEvent = 1;
    
    // Send event to queue from ISR
    xQueueSendFromISR(buttonEventQueue, &buttonEvent, &xHigherPriorityTaskWoken);
    
    // Request context switch if a higher priority task was woken
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
unsigned long start;

void setup() {
    Serial.begin(921600);
    // Create mutex for thread-safe strip access
    xStripMutex = xSemaphoreCreateMutex();
    
    // Create button event queue
    buttonEventQueue = xQueueCreate(5, sizeof(uint32_t));

    // Initialize strips
    // for (int i = 0; i < NUM_STRIPS; i++) {
    // }
      strips[0] = new StripController(LEDS_PER_STRIP1);
      strips[1] = new StripController(LEDS_PER_STRIP2);
      strips[2] = new StripController(LEDS_PER_STRIP3);

    // FastLED setup
    FastLED.addLeds<WS2812B, STRIP1_PIN, GRB>(strips[0]->getLEDs(), LEDS_PER_STRIP1);
    FastLED.addLeds<WS2812B, STRIP2_PIN, GRB>(strips[1]->getLEDs(), LEDS_PER_STRIP2);
    FastLED.addLeds<WS2812B, STRIP3_PIN, GRB>(strips[2]->getLEDs(), LEDS_PER_STRIP3);
    FastLED.setBrightness(50);

    // Create animation tasks for each strip
    for (int i = 0; i < NUM_STRIPS; i++) {
        int* stripIndex = new int(i);
        char buffer[50];
        sprintf(buffer,"AnimationTask %d", i);
        xTaskCreate(
            animationTask,
            buffer,
            2048,       // Stack size
            stripIndex, // Task parameter
            2,          // Priority
            &animationTaskHandles[i]
        );
    }

    // Create button handling task
    xTaskCreate(
        buttonTask,
        "ButtonTask",
        2048,
        NULL,
        2,              // Higher priority than animation tasks
        &buttonTaskHandle
    );

    start = millis();
}
// RTOS will handle tasks, so loop() can be minimal
void loop() {

    switch(currentMode) {
        case RUNNING_LIGHT:
            runningLightAnimation();
            break;
        case RAINBOW:
            rainbowAnimation();
            break;
        case PULSE:
            pulseAnimation();
            break;
        case CHASE:
            chaseAnimation();
            break;
        case GRADIENT:
            gradientAnimation();
            break;
        case SPARKLE:
            sparkleAnimation();
            break;
    }
    // if(millis() - start > 5000){
    //     buttonInterruptHandler();
    //     start = millis();
    // }
    // Render all strips
    FastLED.show(255);
    vTaskDelay(pdMS_TO_TICKS(15));
}