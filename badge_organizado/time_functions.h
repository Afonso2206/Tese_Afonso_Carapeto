#ifndef TIME_FUNCTIONS_H
#define TIME_FUNCTIONS_H

#include <Arduino.h>
#include <ESP32Time.h>

#include "pico_rtc_utils.h"
#include <hardware/rtc.h>

// Variables which must retain their values after deep sleep
// Save to/restored from Watchdog SCRATCH registers
/*bool                    runtimeExpired;           //!< flag indicating if runtime has expired at least once
bool                    longSleep;                //!< last sleep interval; 0 - normal / 1 - long
time_t                  rtcLastClockSync;         //!< timestamp of last RTC synchonization to network time
bool                    wokeUp;                   //!< flag to indicate if woke up from sleep

ESP32Time rtc;*/

void clock_and_rtc_initialization();

void prepareSleep();

#endif