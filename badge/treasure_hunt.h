#ifndef TREASURE_HUNT_H
#define TREASURE_HUNT_H

#include "oled.h"

extern bool isCodeCorrect;
extern int codeIndex;  // Track which code lock we are on (0, 1, or 2)
extern int numberIndex; // Track the current number position in the code (0, 1, or 2)
extern int enteredCode[3];  // Initialize entered code to {1, 1, 1} for each lock
extern int correctCodes[3][3];  // Predefined codes for each location
extern bool first_entry;
extern bool back_key_pressed;
extern uint8_t KeyStatus0;
extern bool treasure_hunt;
//const char* win_message;

extern void ReadAndProcessKeyStatus(); 


void showInstructions(); 

void switchNumber(int moveHorizontal);

void adjustNumber(int moveVertical);

void checkCode();

bool isCorrectCode();

void displayCode();

void displayWinMessage();

void run_treasure_hunt();


#endif
