#include "time_functions.h"

#include "oled_functions.h"

// Variables which must retain their values after deep sleep
// Save to/restored from Watchdog SCRATCH registers
bool                    runtimeExpired;           //!< flag indicating if runtime has expired at least once
bool                    longSleep;                //!< last sleep interval; 0 - normal / 1 - long
time_t                  rtcLastClockSync;         //!< timestamp of last RTC synchonization to network time
bool                    wokeUp;                   //!< flag to indicate if woke up from sleep

ESP32Time rtc;

void clock_and_rtc_initialization(){

    set_sys_clock_khz(24000, true);
    rtc_init();

    // Restore variables and RTC after reset 
    time_t time_saved = watchdog_hw->scratch[0];
    datetime_t dt;
    epoch_to_datetime(&time_saved, &dt);
    
    // Set HW clock (only used in sleep mode)
    rtc_set_datetime(&dt);
    
    // Set SW clock
    rtc.setTime(time_saved);

    runtimeExpired   = ((watchdog_hw->scratch[1] & 1) == 1);
    longSleep        = ((watchdog_hw->scratch[1] & 2) == 2);
    rtcLastClockSync = watchdog_hw->scratch[2];
    wokeUp           = ((watchdog_hw->scratch[1] & 4) == 4);

}

void prepareSleep(void) {
    unsigned long sleep_interval = 10;  // Fixed sleep interval of 10 seconds
    
    oled.clear();
    oled.setCursor(0, 0);
    oled.print("Sleeping for ");
    oled.print(sleep_interval);
    oled.print(" s\n");
    oled.update();
    delay(2000);
    oled.setPower(false);

    wokeUp = true;  // Set the flag to indicate the device will wake up

        // Set HW clock from SW clock
        time_t t_now = rtc.getLocalEpoch();
        datetime_t dt;
        epoch_to_datetime(&t_now, &dt);
        rtc_set_datetime(&dt);
        sleep_us(64);
        pico_sleep(sleep_interval);

        // Save variables to be retained after reset
        watchdog_hw->scratch[2] = rtcLastClockSync;
        
        if (runtimeExpired) {
            watchdog_hw->scratch[1] |= 1;
        } else {
            watchdog_hw->scratch[1] &= ~1;
        }
        if (longSleep) {
            watchdog_hw->scratch[1] |= 2;
        } else {
            watchdog_hw->scratch[1] &= ~2;
        }

        // Set the wakeup flag
        watchdog_hw->scratch[1] |= 4; 

        // Save the current time, because RTC will be reset (SIC!)
        rtc_get_datetime(&dt);
        time_t now = datetime_to_epoch(&dt, NULL);
        watchdog_hw->scratch[0] = now;
        
        //#if defined(REBOOT_AFTER_WAKEUP)
        rp2040.restart();
        //#endif
}