#pragma once
#include <FastLED.h>

const unsigned int NUM_STRIPS = 3;
const unsigned int NUM_LEDS_FRONT_STRIP = 98*2;
const unsigned int NUM_LEDS_MID_STRIP = 28*2;
const unsigned int NUM_LEDS_BACK_STRIP = 43;

const unsigned int PIN_FRONT_STRIP = 2;
const unsigned int PIN_MID_STRIP = 16;
const unsigned int PIN_BACK_STRIP = 5;

const unsigned int FPS = 400;
const unsigned int BRIGHTNESS = 200  ;

static CRGBArray<NUM_LEDS_FRONT_STRIP> frontStrip;
static CRGBArray<NUM_LEDS_MID_STRIP> midStrip;
static CRGBArray<NUM_LEDS_BACK_STRIP> backStrip;

unsigned long counter = 0;

void setupLED(){
    FastLED.addLeds<WS2812B, PIN_FRONT_STRIP, GRB>(frontStrip, NUM_LEDS_FRONT_STRIP);
    FastLED.addLeds<WS2812B, PIN_MID_STRIP, GRB>(midStrip, NUM_LEDS_MID_STRIP);
    FastLED.addLeds<WS2812B, PIN_BACK_STRIP, GRB>(backStrip, NUM_LEDS_BACK_STRIP);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.setMaxRefreshRate(FPS);
}

