#include "time_functions.h"

#include "oled.h"

// Variables which must retain their values after deep sleep
// Save to/restored from Watchdog SCRATCH registers
bool                    runtimeExpired;           //!< flag indicating if runtime has expired at least once
bool                    longSleep;                //!< last sleep interval; 0 - normal / 1 - long
time_t                  rtcLastClockSync;         //!< timestamp of last RTC synchonization to network time
bool                    wokeUp;                   //!< flag to indicate if woke up from sleep

int IRQ_PIN = 13;

ESP32Time rtc;

void clock_and_rtc_initialization(){

    set_sys_clock_khz(24000, true);
    rtc_init();
    pinMode(IRQ_PIN,INPUT);

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
    
    oled.clear();
    oled.setCursor(0, 0);
    oled.println("Going to sleep");
    oled.println(" ");
    oled.println("Press any key twice");
    oled.println("to wake me up!");
    oled.update();
    delay(5000);
    oled.setPower(false);

    wokeUp = true;  // Set the flag to indicate the device will wake up

        // Set HW clock from SW clock
        time_t t_now = rtc.getLocalEpoch();
        datetime_t dt;
        epoch_to_datetime(&t_now, &dt);
        rtc_set_datetime(&dt);
        sleep_us(64);
        sleep_run_from_rosc();
        sleep_goto_dormant_until_edge_high(IRQ_PIN);
        //pico_sleep(sleep_interval);

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
