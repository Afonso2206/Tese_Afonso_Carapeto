#include "oled_functions.h"

#include "snake_functions.h"

#include "time_functions.h"

#include <Wire.h>

#define SLAVE_ADDRESS 0x50 //   Touch Key Controller Address

GyverOLED<SSH1106_128x64> oled;

//----------------------------------------TIMEOUT VARIABLES---------------------------------//

unsigned long lastActivityTime = 0;
const unsigned long inactivityTimeout = 10000;

//--------------------------------TOUCH KEY CONTROLLER VARIABLES-----------------------------//
uint8_t KeyStatus0, i; 
uint8_t Array_SettingRegisters[22];

//---------------------------------------SNAKE GAME VARIABLES----------------------------------------//

bool Snake_Game = false;
int xdir = 1;             //player direction
int ydir = 0;

//---------------------------------------TOUCH KEY VARIABLES----------------------------------------//

bool back_key_pressed = false;

//--------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------//

void ReadAndProcessKeyStatus();
void Key_Instrucion(uint8_t KeyPressed);
uint8_t Read_SettingRegisters(uint8_t Register);
void Write_SettingRegisters();

void navigate_up();
void navigate_down();
void exit_submenu();
void select_submenu_option();
void update_menu();
void select_submenu_option();

int main() {

    clock_and_rtc_initialization();       

    //I2c Communication Intialization
    Wire.begin();

    //OLED Intialization
    DisplayStartupSequence(); 
    oled.clear();
    update_menu();
    
    //Touch Controller Intialization
    for (i = 0xB0; i < 0xC6; i++) {
        Array_SettingRegisters[i - 0xB0] = Read_SettingRegisters(i);
    }

    Write_SettingRegisters();

    //Main Loop
    while (true) {
        unsigned long currentTime = millis();         //Checks to see if there if 10 seconds pass without a key being pressed
        if (KeyStatus0 != 0){
          lastActivityTime = currentTime;
        }
        if (currentTime - lastActivityTime >= inactivityTimeout){
          prepareSleep();             //If 10 seconds pass, the pico goes to sleep
        }
        delay(100); 
        ReadAndProcessKeyStatus();
    }
}

//--------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------//

void ReadAndProcessKeyStatus() {
    Wire.beginTransmission(SLAVE_ADDRESS);
    Wire.write(0x08); // Key1-8 Status Data Register: 08H (This touch controller supports 12 keys)
    Wire.endTransmission();
    Wire.requestFrom(SLAVE_ADDRESS, 1);
    while (Wire.available()) KeyStatus0 = Wire.read();

    if (KeyStatus0 != 0) {
        //digitalWrite(LED_BUILTIN, HIGH);
        Key_Instruction(KeyStatus0);
    } else {
        //digitalWrite(LED_BUILTIN, LOW);
    }
}

void Key_Instruction(uint8_t KeyPressed) {
  oled.clear();
  oled.home();

  if(!Snake_Game){
    switch (KeyPressed) {
      case 0x01:
        //oled.println("LEFT");    //Presses "LEFT"
        break;
      case 0x02:                 //Presses "UP"
        navigate_up();
        break;
      case 0x04:                 //Presses "DOWN"
        navigate_down();
        break;
      case 0x08:
        //oled.println("RIGHT");   //Presses "RIGHT"
        break;
      case 0x10:                 //Presses "OK"
        if (!in_submenu) {
          in_submenu = true;
          update_menu();
        } else {
          select_submenu_option();    
        }
        break;
      case 0x20:                 //Presses "BACK"
        if (in_submenu) {
          exit_submenu();
          current_submenu_index = 0;
          back_key_pressed = true;
        }
        break;
    }
  }
  else 
  {
    if (KeyStatus0 & 0x20){   //If In Snake Game
      exit_submenu();
      back_key_pressed = true;
      oled.update();
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
    for (i = 0; i < 22; i++) {
        Wire.write(Array_SettingRegisters[i]);
    }
    Wire.endTransmission();
}

void navigate_up() {
  // Move selection up based on whether in main menu or submenu
  if (in_submenu) {
    // Move selection up in the submenu
    if (current_submenu_index > 0) {
      current_submenu_index--;
    } else {
      // Wrap around to the bottom of the submenu if at the top
      current_submenu_index = submenu_length - 1;
    }
  } else {
    // Move selection up in the main menu
    if (current_index > 0) {
      current_index--;
    } else {
      // Wrap around to the bottom of the menu if at the top
      current_index = menu_length - 1;
    }
  }
  update_menu();
}

void navigate_down() {
  // Move selection down based on whether in main menu or submenu
  if (in_submenu) {
    // Move selection down in the submenu
    if (current_submenu_index < submenu_length - 1) {
      current_submenu_index++;
    } else {
      // Wrap around to the bottom of the submenu if at the top
      current_submenu_index = 0;
    }
  } else {
    // Move selection down in the main menu
    if (current_index < menu_length - 1) {
      current_index++;
    } else {
      // Wrap around to the top of the menu if at the bottom
      current_index = 0;
    }
  }
  update_menu();
}

void exit_submenu() {
  // Exit the submenu and go back to the main menu
  in_submenu = false;
  update_menu();
}

void select_submenu_option() {
  const char* option = submenu_items[current_index][current_submenu_index];
  oled.clear();
  oled.home();
  oled.setScale(1);
  oled.invertText(false);
  if (option == "See student data"){
    print_student_data();                 //Show Student Data
  }
  else if (option == "Show QR Code"){
    displayQRCode(Student_Info);          //Display QR Code with Student Data
  }
  else if (option == "Snake"){
    Snake_Game = true;
    initialize();
    newApple();
    back_key_pressed = false;
    while(!back_key_pressed){
      if (ydir == 0) {                             //if snake is going left/right:
        if (KeyStatus0 & 0x04) {              //go down
          xdir = 0;
          ydir = 1;
        }
        if (KeyStatus0 & 0x02) {              //go up
          xdir = 0;
          ydir = -1;
        }
        } else {                                     //if snake is going up/down:
            if (KeyStatus0 & 0x08) {              //go right
              xdir = 1;
              ydir = 0;
            }
            if (KeyStatus0 & 0x01) {              //go left
              xdir = -1;
              ydir = 0;
            }
        }
        snake_game(xdir, ydir);
        ReadAndProcessKeyStatus();
    }
    resetGame();
    Snake_Game = false;
    back_key_pressed = false;
    delay(100);
  }
  else if (option == "Digital"){
    delay(100);
    back_key_pressed = false;
    while(!back_key_pressed){
     digital_Voltmeter();       //Digital Voltmeter
     ReadAndProcessKeyStatus();
     delay(10);
    }
    back_key_pressed = false;
  }
  else if (option == "Analog"){
    delay(100);
    back_key_pressed = false;
    while(!back_key_pressed){     
      analog_Voltmeter();       //Analog Voltmeter
      ReadAndProcessKeyStatus();
      delay(10);
    }
    back_key_pressed = false;
  }
  else if (option == "Focus Attention"){
    Focus_Preparation();                  //Prints information before starting the Focus Attention Functionality
    back_key_pressed = false;
    while(!back_key_pressed){
      Focus_Attention();                  //Fills the Focus Attention bar
      ReadAndProcessKeyStatus();
      delay(10);
    }
    percent = 0;                          //Puts the percentage of the Focus Attention bar back at zero
    back_key_pressed = false;
  }
  else if (option == "IST Alameda"){
    print_alameda_info();                 //Prints Information about the Alameda Campus
  }
  else if (option == "IST Taguspark"){
    print_taguspark_info();               //Prints Information about the Taguspark Campus
  }
  
  oled.update();
}
