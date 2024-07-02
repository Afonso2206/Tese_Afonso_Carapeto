//RTC added - badge_v3
//Snake Game added - badge_v4

#include <Arduino.h>
#include <ESP32Time.h>

#include <Wire.h>
#include <GyverOLED.h>

#include "pico_rtc_utils.h"
#include <hardware/rtc.h>

#include <sstream>
#include "qrcodegen.hpp"

GyverOLED<SSH1106_128x64> oled;

#define SLAVE_ADDRESS 0x50

//------------------------------------------------------------------------------------------//
//------------------------------------SNAKE VARIABLES---------------------------------------//
//------------------------------------------------------------------------------------------//

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

bool Snake_Game = false;

unsigned long int frameTime;

int tsz = 4;              //tile size, 4 recommended (for full use of screen, use powers of 2)
int xsz = 128/tsz;        //canvas size
int ysz = 64/tsz;
int ax = 0;               //apple position
int ay = 0;
int px = 16;              //player position
int py = 8;
int xdir = 1;             //player direction
int ydir = 0;
int plen = 2;             //initial player length
int posx[64];             //array containing snake coordinates
int posy[64];
int score = 0;            //player score
int sx = 12 / tsz;        //bounds of score display in top left corner
int sy = 8 / tsz;
String scoreStr = "0";    //player score as string
double tps = 10;          //snake speed in tiles per second

//------------------------------------------------------------------------------------------//
//----------------------------------------------------------QR CODE-------------------------//
//------------------------------------------------------------------------------------------//

using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

static void doBasicDemo();
static std::string toSvgString(const QrCode &qr, int border);

// Variables which must retain their values after deep sleep
// Save to/restored from Watchdog SCRATCH registers
bool                    runtimeExpired;           //!< flag indicating if runtime has expired at least once
bool                    longSleep;                //!< last sleep interval; 0 - normal / 1 - long
time_t                  rtcLastClockSync;         //!< timestamp of last RTC synchonization to network time
bool                    wokeUp;                   //!< flag to indicate if woke up from sleep

ESP32Time rtc;

static byte percent = 0;  //Limpar isto

/*---------------------------------------BLACKBOARD---------------------*/

struct Blackboard {
  unsigned long lastActivityTime = 0;
  const unsigned long inactivityTimeout = 10000;

  bool back_key_pressed = false;
  
  bool KeyIsPressed = true;
  uint8_t KeyStatus0;
  uint8_t i;
  uint8_t Array_SettingRegisters[22];
  const char* Student_Info = "BEGIN:VCARD\n"
                       "VERSION:3.0\n"
                       "FN;CHARSET=UTF-8:Afonso Santos Carapeto\n"
                       "N;CHARSET=UTF-8:Carapeto;Afonso;Santos;;\n"
                       "EMAIL;CHARSET=UTF-8;type=HOME,INTERNET:afonso.carapeto@tecnico.ulisboa.pt\n"
                       "TEL;TYPE=CELL:+351 929264325\n"
                       "NOTE;CHARSET=UTF-8:MEEC nº 93763\n"
                       "REV:2024-01-16T14:41:13.993Z\n"
                       "END:VCARD\n";
  const char* menu_items[6] = {
    "User Data",
    "Games",
    "Voltmeter",
    "I2C Explorer",
    "Focus Attention",
    "IST Info"
  };

  const char* submenu_items[6][2] = {
    {"See user data", "Show QR Code"},      // Submenu options for User Data
    {"Snake", "Game 2"},            // Submenu options for Games
    {"Digital", "Analog"},    // Submenu options for Voltmeter
    {"I2C Explorer 1", "I2C Explorer 2"},  // Submenu options for I2C Explorer
    {"Focus Attention"},  // Submenu options for Focus Attention
    {"IST Alameda", "IST Taguspark"}       // Submenu options for IST Info
};
  
  const int menu_length = sizeof(menu_items) / sizeof(menu_items[0]);
  int current_index = 0;
  int current_submenu_index = 0;
  bool in_submenu = false;
  
  const uint8_t IST_Logo[128*8] PROGMEM  = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x7f, 0x3f, 0x3f, 0xbf, 0xbf, 0xbf, 0xbf, 0x3f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xe0, 0xe0, 0xe0, 0xe0, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xf1, 0xf1, 0x01, 0x01, 0x01, 0x01, 0x01, 0xf1, 0xf1, 0xf1, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x1f, 0x7f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf3, 0xc1, 0x81, 0x81, 0x01, 0x1f, 0x7f, 0x7f, 0x7f, 0x3f, 0x00, 0x80, 
  0x80, 0xc0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x3f, 0x1f, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 
  0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x3f, 0x0f, 
  0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0f, 0x0f, 0x1f, 0x3f, 0x3f, 0x7f, 0x7f, 
  0xff, 0x7f, 0x7f, 0x3f, 0x3f, 0x1f, 0x0f, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
};

Blackboard bb;

void ReadAndProcessKeyStatus();
void Key_Instrucion(uint8_t KeyPressed);
uint8_t Read_SettingRegisters(uint8_t Register);
void Write_SettingRegisters();

void DisplayStartupSequence(); 
void navigate_up();
void navigate_down();
void exit_submenu();
void select_submenu_option();
void update_display();
void select_submenu_option();
void update_display();
void drawVoltmeter();
void drawNeedle();
void fillFocusBar();

void drawSnake();
void drawApple();
void displayScore();
void gameOver();

void displayQRCode();
int translateX();
int translateY();

void prepareSleep(void);

//------------------------------------------------------------------------//

int main() 
{
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

  
        
  Wire.begin();
  DisplayStartupSequence(); 
  oled.clear();
  update_display();
  
  for (bb.i = 0xB0; bb.i < 0xC6; bb.i++) {
    bb.Array_SettingRegisters[bb.i - 0xB0] = Read_SettingRegisters(bb.i);
  }
  
  Write_SettingRegisters();
  
  while (true) {
    unsigned long currentTime = millis();
    if (bb.KeyStatus0 != 0){
      bb.lastActivityTime = currentTime;
    }
    if (currentTime - bb.lastActivityTime >= bb.inactivityTimeout){
      prepareSleep();
    }
    
    delay(100);  
    ReadAndProcessKeyStatus();
  }

  return 0; 
}

void ReadAndProcessKeyStatus() {
  Wire.beginTransmission(SLAVE_ADDRESS); 
  Wire.write(0x08);  
  Wire.endTransmission(); 
  Wire.requestFrom(SLAVE_ADDRESS, 1); 
  while (Wire.available()) bb.KeyStatus0 = Wire.read();

  if (bb.KeyStatus0 != 0) {
    //digitalWrite(LED_BUILTIN, HIGH);
    Key_Instrucion(bb.KeyStatus0);
  } else {
    //digitalWrite(LED_BUILTIN, LOW);
  }
}

void Key_Instrucion(uint8_t KeyPressed) {
  oled.clear();
  oled.home();

  if(!Snake_Game){
    switch (KeyPressed) {
      case 0x01:
        //oled.println("LEFT");  //Presses "LEFT"
        break;
      case 0x02:         //Presses "UP"
        navigate_up();
        break;
      case 0x04:       //Presses "DOWN"
        navigate_down();
        break;
      case 0x08:
        //oled.println("RIGHT");   //Presses "RIGHT"
        break;
      case 0x10:        //Presses "OK"
        if (!bb.in_submenu) {
          bb.in_submenu = true;
          update_display();
        } else {
          select_submenu_option();    ///////-------------------SUBSTITUIR MAIS À FRENTE COM O QUE CADA OPCAO FAZ, EXEMPLO: SE CLICA NO USER DATA MOSTRA OS DADOS----//
        }
        break;
      case 0x20:        //Presses "BACK"
        if (bb.in_submenu) {
          exit_submenu();
          bb.back_key_pressed = true;
          //oled.update();
        }
        break;
    }
  }
  else 
  {
    if (bb.KeyStatus0 & 0x20){
      exit_submenu();
      bb.back_key_pressed = true;
      //oled.update();
    }
  }
  
  //oled.update();
}

uint8_t Read_SettingRegisters(uint8_t Register) {
  Wire.beginTransmission(SLAVE_ADDRESS); 
  Wire.write(Register);   
  Wire.endTransmission();
  Wire.requestFrom(SLAVE_ADDRESS, 1); 
  while (Wire.available()) return Wire.read();
  return 0;
}

void Write_SettingRegisters() {
  Wire.beginTransmission(SLAVE_ADDRESS); 
  Wire.write(0xB0);
  for (bb.i = 0; bb.i < 22; bb.i++) {
    Wire.write(bb.Array_SettingRegisters[bb.i]);
  }    
  Wire.endTransmission();
}

//------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------//

void navigate_up() {
  // Move selection up based on whether in main menu or submenu
  if (bb.in_submenu) {
    // Move selection up in the submenu
    if (bb.current_submenu_index > 0) {
      bb.current_submenu_index--;
    } else {
      // Wrap around to the bottom of the submenu if at the top
      bb.current_submenu_index = 1;
    }
  } else {
    // Move selection up in the main menu
    if (bb.current_index > 0) {
      bb.current_index--;
    } else {
      // Wrap around to the bottom of the menu if at the top
      bb.current_index = bb.menu_length - 1;
    }
  }
  update_display();
}

void navigate_down() {
  // Move selection down based on whether in main menu or submenu
  if (bb.in_submenu) {
    // Move selection down in the submenu
    if (bb.current_submenu_index < 1) {
      bb.current_submenu_index++;
    } else {
      // Wrap around to the bottom of the submenu if at the top
      bb.current_submenu_index = 0;;
    }
  } else {
    // Move selection down in the main menu
    if (bb.current_index < bb.menu_length - 1) {
      bb.current_index++;
    } else {
      // Wrap around to the top of the menu if at the bottom
      bb.current_index = 0;
    }
  }
  update_display();
}

void exit_submenu() {
  // Exit the submenu and go back to the main menu
  bb.in_submenu = false;
  update_display();
}

void select_submenu_option() {
  const char* option = bb.submenu_items[bb.current_index][bb.current_submenu_index];
  oled.clear();
  oled.home();
  oled.setScale(1);
  oled.invertText(false);
  if (option == "See user data"){
    oled.println("Afonso Carapeto");
    oled.println("IST 193763");
    oled.println("MEEC");
    oled.println("929264325");
    oled.println("afonso.carapeto");
    oled.println("@tecnico.ulisboa.pt");
  }
  else if (option == "Show QR Code"){
    displayQRCode(bb.Student_Info);
  }
  else if (option == "Snake"){
    Snake_Game = true;
    frameTime = millis();
    initialize();
    newApple();
    bb.back_key_pressed = false;
    while(!bb.back_key_pressed){
      if (ydir == 0) {                             //if snake is going left/right:
      //while((frameTime + 1000/tps) > millis()) { //polling for input (up/down) until next frame
        if (bb.KeyStatus0 & 0x04) {              //go down
          xdir = 0;
          ydir = 1;
        }
        if (bb.KeyStatus0 & 0x02) {              //go up
          xdir = 0;
          ydir = -1;
        }
      //}
      } else {                                     //if snake is going up/down:
        //while((frameTime + 1000/tps) > millis()) { //polling for input (left/right) until next frame
          if (bb.KeyStatus0 & 0x08) {              //go right
            xdir = 1;
            ydir = 0;
          }
          if (bb.KeyStatus0 & 0x01) {              //go left
            xdir = -1;
            ydir = 0;
          }
        //}
      }
      
      frameTime = millis();                       //current time
      
      px += xdir;                                 //new head position
      py += ydir;
      if (px < 0) px = xsz - 1;                   //wrap snake around screen
      if (px > xsz - 1) px = 0;
      if (py < 0) py = ysz - 1;
      if (py > ysz - 1) py = 0;
    
      if(px == ax && py == ay){                   //check if snake is eating an apple
        plen++;                                   //increase length
        randomSeed(frameTime);                    //new random seed before generation of new apple
        newApple();                               //generate new apple
        score++;                                  //increase score
        scoreStr = String(score);                 //update score string
        tps += 0.4;                               //increase speed by 0.4 tiles/s
      }
    
      for (int i = 0; i < plen; i++){
        if (px == posx[i] && py == posy[i]){
          gameOver();
        }
      }
      
      oled.clear();
      drawSnake();
      drawApple();
      displayScore();
      oled.update();
      ReadAndProcessKeyStatus();
    }
    Snake_Game = false;
    bb.back_key_pressed = false;
    delay(100);
  }
  else if (option == "Digital"){
    delay(100);
    bb.back_key_pressed = false;
    while(!bb.back_key_pressed){
      int ADC_Value = analogRead(28);
      float voltage = (ADC_Value / 1023.0) * 3.3;
      oled.clear();
      oled.home();
      oled.print(voltage);
      oled.print("V");
      oled.update();
      ReadAndProcessKeyStatus();
      delay(10);
    }
    bb.back_key_pressed = false;
  }
  else if (option == "Analog"){
    delay(100);
    bb.back_key_pressed = false;
    while(!bb.back_key_pressed){
      int ADC_Value = analogRead(28);
      float voltage = (ADC_Value / 1023.0) * 3.3;
      oled.clear();
      drawVoltmeter();
      drawNeedle(voltage);
      oled.update();
      ReadAndProcessKeyStatus();
      delay(10);
    }
    bb.back_key_pressed = false;
  }
  else if (option == "Focus Attention"){
    oled.clear();
    oled.println("Prepare to study");
    oled.println("for 30 minutes");
    oled.update();
    delay(2000);
    oled.clear();
    oled.println("3,2,1...");
    oled.update();
    delay(2000);
    oled.clear();
    oled.rect(20, 30, 108, 50, OLED_STROKE);  
    oled.setCursorXY(50, 15);                 
    oled.print("Focus");                      
    oled.update();
    bb.back_key_pressed = false;
    while(!bb.back_key_pressed){
      static unsigned long previousMillis = 0;
      unsigned long currentMillis = millis();
      const unsigned long interval = 600;  
      
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;  
    
        if (percent <= 100) {
          fillFocusBar(percent);  // Fill the battery by percent
          percent++;
        } else {
          percent = 0;  // Reset percent after reaching 100
        }
      }
      ReadAndProcessKeyStatus();
      delay(10);
    }
    percent = 0;
    bb.back_key_pressed = false;
  }
  else if (option == "IST Alameda"){
    oled.println("Av. Rovisco Pais");
    oled.println("1049-001 Lisboa");
    oled.println("218417000");
  }
  else if (option == "IST Taguspark"){
    oled.println("Av. Prof. Dr. Cavaco");
    oled.println("Silva");
    oled.println("2744-016 Porto Salvo");
    oled.println("214233200");
  }
  
  oled.update();
}

void update_display() {
  oled.clear();
  oled.home();
  oled.setScale(1);

  if (bb.in_submenu) {
    // Display submenu options
    oled.invertText(false); // Turn off inversion before printing submenu
    oled.println(bb.menu_items[bb.current_index]);
    oled.println("");
    for (int k = 0; k < 2; k++) { // Iterate through submenu options
      if (k == bb.current_submenu_index) {
        oled.invertText(true);
      } else {
        oled.invertText(false);
      }
      oled.println(bb.submenu_items[bb.current_index][k]);
    }
  } else {
    // Display main menu
    oled.invertText(false); // Turn off inversion before printing main menu
    oled.println("Main Menu");
    oled.println("");
    for (int j = 0; j < bb.menu_length; j++) {
      if (j == bb.current_index) {
        oled.invertText(true);
      } else {
        oled.invertText(false);
      }
      oled.println(bb.menu_items[j]);
    }
  }

  oled.update();
}



void DisplayStartupSequence() {
  oled.init();
  oled.clear();
  oled.drawBitmap(0, 0, bb.IST_Logo, 128, 64);
  oled.update();
  delay(1000); 
}

void drawVoltmeter() {
  // Draw the circular face of the voltmeter
  //oled.circle(64, 32, 30);
  //oled.circle(64, 32, 28);

  // Draw tick marks
  for (int i = 0; i <= 10; i++) {
    float angle = map(i, 0, 10, -180, 0);
    float radians = angle * PI / 180.0;
    int x1 = 64 + cos(radians) * 25;
    int y1 = 40 + sin(radians) * 25;
    int x2 = 64 + cos(radians) * 30;
    int y2 = 40 + sin(radians) * 30;
    oled.line(x1, y1, x2, y2);
  }

  //Draw labels
  oled.setCursor(30, 6);
  oled.print("0V");
  oled.setCursor(35, 1);
  oled.print("1V");
  //oled.setCursor(55, 0);
  //oled.print("1.7V");
  oled.setCursor(70, 0);
  oled.print("2V");
  oled.setCursor(95, 3);
  oled.print("3V");
  oled.setCursor(90, 6);
  oled.print("3.3V");
}

void drawNeedle(float voltage) {
  // Map the voltage to an angle between -180 and 0 degrees
  int angle = map(voltage * 100, 0, 3.3 * 100, -180, 0);
  float radians = angle * PI / 180.0;
  int x = 64 + cos(radians) * 25;
  int y = 40 + sin(radians) * 25;

  // Draw the needle
  oled.line(64, 40, x, y);
}

void fillFocusBar(byte percent) {
  // Clear previous fill
  oled.rect(22, 32, 106, 48, OLED_CLEAR);  // Clear inside battery
  // Calculate width to fill based on percentage
  int fillWidth = (106 - 22) * percent / 100;
  // Draw filled part
  oled.rect(22, 32, 22 + fillWidth, 48, OLED_FILL);
  oled.update();
}

//--------------------------------------------SNAKE FUNCTIONS--------------------------------------//

void initialize(){                            //put snake in initial position
  for (int i = 0; i < plen; i++){
    posx[i] = px-i;
    posy[i] = py;
  }
}

void newApple(){                              //generate random pos for new apple
  ax = random(sx, xsz);                       //new pos excluding top left corner due to score display
  ay = random(sy, ysz);
  for (int i = 0; i < plen; i++){             //check if apple pos occupied by snake
    while((ax == posx[i] && ay == posy[i]) || (ax == px && ay == py)){
      newApple();                             //generate new apple recursively until not on snake
      break;
    }
  }
}

void drawSnake(){  
  for (int i = plen; i > 0; i--){             //update snake coordinates in pos array
    posx[i] = posx[i-1];
    posy[i] = posy[i-1];
  }
  posx[0] = px;                               //new head inserted in pos array
  posy[0] = py;
  
  for (int i = 0; i < plen; i++){             //draw snake from pos array
    oled.rect(posx[i]*tsz, posy[i]*tsz, posx[i]*tsz + tsz, posy[i]*tsz + tsz, OLED_FILL);
  }
}

void drawApple(){
  oled.rect(ax*tsz, ay*tsz, ax*tsz+tsz, ay*tsz+tsz, OLED_FILL);
}

void displayScore(){
  oled.setScale(1);
  oled.setCursor(0, 0);
  oled.println(scoreStr);
}

void gameOver(){                              
  for (int i = 0; i < 4; i++){  //blink snake 4 times
    oled.clear();
    for (int i = 0; i < plen; i++) oled.rect(posx[i]*tsz, posy[i]*tsz, posx[i]*tsz+tsz, posy[i]*tsz+tsz, OLED_FILL);
    drawApple();
    displayScore();
    oled.update();
    delay(300);
    oled.clear();
    drawApple();
    displayScore();
    oled.update();
    delay(300);
  }

  px = 16;                                    //reset game
  py = 8;
  xdir = 1;
  ydir = 0;
  plen = 2;
  score = 0;
  scoreStr = String(score);
  tps = 10;
  displayScore();
  initialize();
  newApple();
}


//-----------------------------------------------------------------------------------------//
//----------------------------QR CODE FUNCTIONS--------------------------------------------//
//-----------------------------------------------------------------------------------------//

void displayQRCode(const char *text) {
  int scale = 2;
  const QrCode::Ecc errCorLvl = QrCode::Ecc::LOW;
  const QrCode qr = QrCode::encodeText(text, errCorLvl);

  int border = 1;
  int rectSize = 2; // Adjust this size as needed

  for (int y = -border; y < qr.getSize() + border; y++) {
    for (int x = -border; x < qr.getSize() + border; x++) {
      int xPos = translateX(x, y, border, qr.getSize());
      int yPos = translateY(x, y, border, qr.getSize());
      if (!qr.getModule(x, y)) {
        oled.dot(xPos, yPos);
      }
    }
  }
}

// Returns a string of SVG code for an image depicting the given QR Code, with the given number
// of border modules. The string always uses Unix newlines (\n), regardless of the platform.
static std::string toSvgString(const QrCode &qr, int border) {
  if (border < 0)
    throw std::domain_error("Border must be non-negative");
  if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
    throw std::overflow_error("Border too large");
  
  std::ostringstream sb;
  sb << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  sb << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
  sb << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
  sb << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2) << "\" stroke=\"none\">\n";
  sb << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
  sb << "\t<path d=\"";
  for (int y = 0; y < qr.getSize(); y++) {
    for (int x = 0; x < qr.getSize(); x++) {
      if (qr.getModule(x, y)) {
        if (x != 0 || y != 0)
          sb << " ";
        sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
      }
    }
  }
  sb << "\" fill=\"#000000\"/>\n";
  sb << "</svg>\n";
  return sb.str();
}


int translateX(int x, int y, int border, int qrSize) {
  // Center the QR Code on a 128x64 display
  int displayWidth = 128;
  return (displayWidth - qrSize) / 2 + x + border;
}

int translateY(int x, int y, int border, int qrSize) {
  // Center the QR Code on a 128x64 display
  int displayHeight = 64;
  return (displayHeight - qrSize) / 2 + y + border;
}

//------------------------------------------RTC Functions----------------------//
//-----------------------------------------------------------------------------//

/*void printDateTime(void) {
    struct tm timeinfo;
    char tbuf[25];
    
    time_t tnow = rtc.getLocalEpoch();
    localtime_r(&tnow, &timeinfo);
    strftime(tbuf, 25, "%Y-%m-%d %H:%M:%S", &timeinfo);
    oled.home();
    oled.println(tbuf);
    oled.update();
}*/

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
