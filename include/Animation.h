#pragma once
#include "Buttons.h"
#include "AnimationMode.h"
#include "LEDStrips.h"
#include <esp_log.h>

#define N_MILLIS_SIGNAL 80
#define N_MILLIS_WELCOME 80
#define N_MILLIS_ANIMATION 20
#define SIGNAL_BEAT 50
#define ANIMATION_CHECK_DEBOUNCE 200

uint8_t mid = NUM_LEDS_FRONT_STRIP / 2;
uint8_t mid2 = NUM_LEDS_MID_STRIP / 2;
uint8_t mid3 = NUM_LEDS_BACK_STRIP / 2;

uint8_t lastPosition = 0;
uint8_t lastPosition2 = 0;
uint8_t lastPosition3 = 0;

uint8_t commet_size = 7;
const uint8_t dividerFront = mid / commet_size;
const uint8_t dividerMid = mid2 / commet_size;
const uint8_t dividerBack = mid3 / commet_size;

uint8_t commet = NUM_LEDS_FRONT_STRIP / (dividerFront * 2);
uint8_t commet2 = NUM_LEDS_MID_STRIP / (dividerMid * 2);
uint8_t commet3 = NUM_LEDS_BACK_STRIP / (dividerBack * 2);

// CRGBArray<mid> frontStripRight = frontStrip(NUM_LEDS_FRONT_STRIP / 2, NUM_LEDS_FRONT_STRIP);
bool isAnimating = false;

bool WelcomeAnimation()
{
    // Welcome animation
    static int counterAnim = 0;
    static int counterFront = 0;
    if (counterFront >= 1)
    {
        frontStrip.fill_solid(CRGB::White);
        counterFront = 0;
        curMode = IDLE;
        return true;
    }
    uint8_t rawww = triwave8(counterAnim);
    uint8_t raww = triwave8(rawww);
    uint8_t raw = triwave8(raww);
    if (counterAnim > 255)
    {
        counterAnim = 0;
    }

    int indexLed = map(raw, 0, 255, 0, NUM_LEDS_FRONT_STRIP / 2);

    Serial.println(counterAnim++);
    ESP_LOGD("frontcounter", "%d\n", counterFront);

    frontStrip[mid - indexLed - 1] = CRGB::White;
    frontStrip[mid + indexLed] = CRGB::White;

    if (abs(lastPosition - indexLed) >= 2)
    {
        if (lastPosition < indexLed)
        {
            for (int i = lastPosition; i < indexLed; i++)
            {
                frontStrip[mid - i - 1] = CRGB::White;
                frontStrip[mid + i] = CRGB::White;
            }
        }
        else
        {
            for (int i = indexLed; i < lastPosition; i++)
            {
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

bool ByeAnimation()
{
    static int counterAnim = 0;
    static int counterFront = 0;
    if (counterFront >= 1)
    {
        frontStrip.fill_solid(CRGB::Black);
        curMode = IDLE;
        return true;
    }
    uint8_t rawww = triwave8(counterAnim);
    uint8_t raww = triwave8(rawww);
    uint8_t raw = triwave8(raww);
    if (counterAnim > 255)
    {
        counterAnim = 0;
    }

    int indexLed = map(raw, 0, 255, 0, NUM_LEDS_FRONT_STRIP / 2);

    Serial.println(counterAnim++);
    ESP_LOGD("BYE", "frontcounter:%d raww:%d\n", counterFront, raww);
    // Serial.println("----");
    CRGB color = raww > 127 ? CRGB::Black : CRGB(127, 127, 127);
    frontStrip[mid - indexLed - 1] = color;
    frontStrip[mid + indexLed] = color;

    if (abs(lastPosition - indexLed) >= 2)
    {
        if (lastPosition < indexLed)
        {
            for (int i = lastPosition; i < indexLed; i++)
            {
                frontStrip[mid - i - 1] = color;
                frontStrip[mid + i] = color;
            }
        }
        else
        {
            for (int i = indexLed; i < lastPosition; i++)
            {
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
    static uint16_t prev_raw = 0;
    static uint16_t timebase = GET_MILLIS();
    if (prev_raw == 0)
    {
        EVERY_N_MILLISECONDS(ANIMATION_CHECK_DEBOUNCE)
        {
            timebase = GET_MILLIS();
        }
    }

    uint16_t raw = beat16(SIGNAL_BEAT, timebase);
    if (prev_raw > raw)
    {
        fadeToBlackBy(frontStrip, mid, 255);
        fadeToBlackBy(midStrip, mid2, 255);
        fadeToBlackBy(backStrip, mid3, 255);
        prev_raw = 0;
        if (curMode != SIGNAL_LEFT)
        {
            return true;
        }
    }
    prev_raw = raw;
    uint8_t indexLed = map(raw, 0, 65535, 0, mid);
    uint8_t indexLed2 = map(raw, 0, 65535, 0, mid2);
    uint8_t indexLed3 = map(raw, 0, 65535, 0, mid3);

    fill_solid(&frontStrip[mid - indexLed], indexLed - 1, CRGB::Yellow);
    fill_solid(&midStrip[mid2 - indexLed2], indexLed2 - 1, CRGB::Yellow);
    fill_solid(&backStrip[mid3 - indexLed3], indexLed3 - 1, CRGB::Yellow);

    for (int i = 0; i < 3; i++)
    {
        blur1d(frontStrip, mid, 64);
        blur1d(midStrip, mid2, 64);
        blur1d(backStrip, mid3, 64);
    }
    return false;
}

bool TurnRightAnimation()
{

    static uint16_t prev_raw = 0;
    static uint16_t timebase = GET_MILLIS();
    if (prev_raw == 0)
    {
        EVERY_N_MILLISECONDS(ANIMATION_CHECK_DEBOUNCE)
        {
            timebase = GET_MILLIS();
        }
    }
    uint16_t raw = beat16(SIGNAL_BEAT, timebase);
    if (prev_raw > raw)
    {

        fadeToBlackBy(&frontStrip[mid], mid, 255);
        fadeToBlackBy(&midStrip[mid2], mid2, 255);
        fadeToBlackBy(&backStrip[mid3], mid3, 255);
        prev_raw = 0;
        if (curMode != SIGNAL_LEFT)
        {
            return true;
        }
    }
    prev_raw = raw;
    uint8_t indexLed = map(raw, 0, 65535, 0, mid);
    uint8_t indexLed2 = map(raw, 0, 65535, 0, mid2);
    uint8_t indexLed3 = map(raw, 0, 65535, 0, mid3);

    fill_solid(&frontStrip[mid], indexLed, CRGB::Yellow);
    fill_solid(&midStrip[mid2], indexLed2, CRGB::Yellow);
    fill_solid(&backStrip[mid3], indexLed3, CRGB::Yellow);

    for (int i = 0; i < 3; i++)
    {
        blur1d(&frontStrip[mid], mid, 64);
        blur1d(&midStrip[mid2], mid2, 64);
        blur1d(&backStrip[mid3], mid3, 64);
    }
    return false;
}

bool HazardAnimation()
{

    TurnLeftAnimation();
    return TurnRightAnimation();
}

bool RunningAnimation()
{
    static int counterAnimFront = 0;
    static int counterAnimMid = 0;
    static int counterAnimBack = 0;

    static int counterKnight = 0;
    counterKnight += 2;
    uint8_t indexLed = counterAnimFront * commet;
    uint8_t indexLed2 = counterAnimMid * commet2;
    uint8_t indexLed3 = counterAnimBack * commet3;

    if (counterKnight >= 255)
    {
        counterKnight = 0;
        counterAnimFront = 0;
        counterAnimMid = 0;
        counterAnimBack = 0;
        return true;
    }

    uint8_t raw = triwave8(counterKnight);
    counterAnimFront = map8(raw, 0, dividerFront * 2 - 1);
    counterAnimMid = map8(raw, 0, dividerMid * 2 - 1);
    counterAnimBack = map8(raw, 0, dividerBack * 2 - 1);

    frontStrip.fadeToBlackBy(64);
    midStrip.fadeToBlackBy(64);
    backStrip.fadeToBlackBy(64);

    fill_solid(&frontStrip[indexLed + 2], commet, CRGB::White);
    fill_solid(&midStrip[indexLed2], commet2, CRGB::White);
    fill_solid(&backStrip[indexLed3], commet3, CRGB::White);

    for (int i = 0; i < 3; i++)
    {
        frontStrip.blur1d(64);
        midStrip.blur1d(64);
        backStrip.blur1d(64);
    }
    return false;
}

bool RunningAnimationV2()
{
    static uint16_t timebase = GET_MILLIS();
    static uint16_t prev_raw = 0;
    static bool first_peak_detected = false;
    static bool period_complete = false;
    const uint8_t BPM = 20;

    if (prev_raw == 0)
    {
        EVERY_N_MILLISECONDS(ANIMATION_CHECK_DEBOUNCE)
        {
            timebase = GET_MILLIS();
        }
    }

    uint16_t raw = beatsin16(BPM);
    uint16_t indexLed = map(raw, 0, 65535, 0, NUM_LEDS_FRONT_STRIP - commet_size * 3);
    uint16_t indexLed2 = map(raw, 0, 65535, 0, NUM_LEDS_MID_STRIP - commet_size);
    uint16_t indexLed3 = map(raw, 0, 65535, 0, NUM_LEDS_BACK_STRIP - commet_size);

    frontStrip.fadeToBlackBy(64);
    midStrip.fadeToBlackBy(64);
    backStrip.fadeToBlackBy(64);

    fill_solid(&frontStrip[indexLed], commet_size * 3, CRGB::White);
    fill_solid(&midStrip[indexLed2], commet_size, CRGB::White);
    fill_solid(&backStrip[indexLed3], commet_size, CRGB::White);

    for (int i = 0; i < 1; i++)
    {
        frontStrip.blur1d(127);
        midStrip.blur1d(64);
        backStrip.blur1d(64);
    }

    // Detect turning points
    if (prev_raw < raw && raw > beatsin16(BPM))
    {
        if (!first_peak_detected)
        {
            ESP_LOGD("Breath", "First peak detected");
            first_peak_detected = true;
        }
        else
        {
            ESP_LOGD("Breath", "Period complete");
            period_complete = true;
        }
        // if (!first_peak_detected)
        // {
        //     first_peak_detected = true;
        // }
        // else
        // {
        //     period_complete = true;
        // }
    }

    prev_raw = raw;

    if (period_complete)
    {
        // fadeToBlackBy(frontStrip, 255);
        // fadeToBlackBy(midStrip, 255);
        // fadeToBlackBy(backStrip, 255);
        period_complete = false;     // Reset for the next period
        first_peak_detected = false; // Reset for the next period
        prev_raw = 0;
        return true;
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
    if (strobe > 127)
    {
        fill_solid(frontStrip, mid, CRGB::White);
        fill_solid(midStrip, mid2, CRGB::White);
        fill_solid(backStrip, mid3, CRGB::White);
        fill_solid(&frontStrip[mid], mid, CRGB::Blue);
        fill_solid(&midStrip[mid2], mid2, CRGB::Blue);
        fill_solid(&backStrip[mid3], mid3 + 1, CRGB::Blue);
    }
    else
    {
        fill_solid(frontStrip, mid, CRGB::Blue);
        fill_solid(midStrip, mid2, CRGB::Blue);
        fill_solid(backStrip, mid3, CRGB::Blue);
        fill_solid(&frontStrip[mid], mid, CRGB::White);
        fill_solid(&midStrip[mid2], mid2, CRGB::White);
        fill_solid(&backStrip[mid3], mid3 + 1, CRGB::White);
        counterStrobe++;
    }
    if (counterStrobe >= 10)
    {
        counterStrobe = 0;
        return true;
    }
    return false;
}

bool BreathAnimation()
{

    static uint16_t timebase = GET_MILLIS();
    // static uint16_t timebase = 0;
    static uint16_t prev_raw = 0;
    static bool first_peak_detected = false;
    static bool period_complete = false;
    static bool swap_colors = false;
    const uint16_t BPM = 30;

    if (prev_raw == 0)
    {
        EVERY_N_MILLISECONDS(ANIMATION_CHECK_DEBOUNCE)
        {
            timebase = GET_MILLIS();
        }
    }

    uint16_t raw = beatsin16(BPM);

    // Generate sine wave values
    uint16_t left_value = beatsin16(BPM, 0, 65535 / 2, timebase);
    uint16_t right_value = beatsin16(BPM, 0, 65535 / 2, timebase, 65535 / 2); // 90-degree phase offset (16384 in 16-bit space)
    ESP_LOGD("Breath", "Left: %d, Right: %d", left_value, right_value);
    // Define colors
    CRGB left_color = swap_colors ? CRGB(0, 0, 255) : CRGB(255, 0, 0);  // Blue or Red
    CRGB right_color = swap_colors ? CRGB(255, 0, 0) : CRGB(0, 0, 255); // Red or Blue

    // Fill the strip
    fill_solid(frontStrip, mid, left_color);
    fill_solid(&frontStrip[mid], mid, right_color);
    fill_solid(midStrip, mid2, left_color);
    fill_solid(&midStrip[mid2], mid2, right_color);
    fill_solid(backStrip, mid3, left_color);
    fill_solid(&backStrip[mid3], mid3 + 1, right_color);

    // Adjust brightness
    fadeLightBy(frontStrip, mid, (left_value >> 8));
    fadeLightBy(&frontStrip[mid], mid, (right_value >> 8));
    fadeLightBy(midStrip, mid2, (left_value >> 8));
    fadeLightBy(&midStrip[mid2], mid2, (right_value >> 8));
    fadeLightBy(backStrip, mid3, (left_value >> 8));
    fadeLightBy(&backStrip[mid3], mid3 + 1, (right_value >> 8));
    // Detect turning points
    if (prev_raw < raw && raw > beatsin16(BPM))
    {
        if (!first_peak_detected)
        {
            ESP_LOGD("Breath", "First peak detected");
            first_peak_detected = true;
        }
        else
        {
            ESP_LOGD("Breath", "Period complete");
            period_complete = true;
        }
    }
    prev_raw = raw;

    if (period_complete)
    {
        period_complete = false;     // Reset for the next period
        first_peak_detected = false; // Reset for the next period
        swap_colors = !swap_colors;  // Swap colors after one full period
        prev_raw = 0;
        return true;
    }

    return false;
}