#include "treasure_hunt.h"

bool isCodeCorrect = false;
int codeIndex = 0;  // Track which code lock we are on (0, 1, or 2)
int numberIndex = 0; // Track the current number position in the code (0, 1, or 2)
int enteredCode[3] = {1, 1, 1};  // Initialize entered code to {1, 1, 1} for each lock
int correctCodes[3][3] = {{3, 5, 7}, {1, 8, 4}, {6, 2, 9}};  // Predefined codes for each location
bool first_entry = true;
const char* win_message  = "You found all 3 codes!";

void showInstructions() {
    oled.clear();
    oled.home();
    oled.println("Treasure Hunt!");
    oled.println("");
    oled.println("Use the keys to");
    oled.println(" the find codes!");
    oled.update();
    delay(2000);
}

void switchNumber(int moveHorizontal) {
    numberIndex += moveHorizontal;
    if (numberIndex < 0) numberIndex = 2;
    if (numberIndex > 2) numberIndex = 0;
}

// Increase or decrease the current number in the code
void adjustNumber(int moveVertical) {
    enteredCode[numberIndex] += moveVertical;
    if (enteredCode[numberIndex] < 1) enteredCode[numberIndex] = 9;
    if (enteredCode[numberIndex] > 9) enteredCode[numberIndex] = 1;
}

void checkCode() {
    if (isCorrectCode()) {  // If the entered code is correct
        // Show "correct code" message before moving to the next code
        oled.clear();
        oled.setScale(1);
        oled.setCursorXY(5, 5);
        oled.print("Correct code!");
        oled.setCursorXY(5, 20);
        oled.print("Next one!");
        oled.update();
        delay(1500);  // Pause to show the "Correct code" message

        codeIndex++;  // Move to the next code

        if (codeIndex >= 3) {  // Check if all codes have been entered correctly
            oled.clear();
            oled.setCursorXY(5, 5);
            oled.print("You won!");
            oled.update();
            delay(2000);  // Pause to display the victory message
            //back_key_pressed = true;  // Exit game
        } else {
            // Reset entered code for next lock
            enteredCode[0] = enteredCode[1] = enteredCode[2] = 1;
            displayCode();  // Show next code lock
        }
    } else if (!first_entry) {
        oled.clear();
        oled.setCursorXY(5, 5);
        oled.print("Try Again!");
        oled.update();
        delay(1000);  // Pause to show the "Try Again" message
        displayCode();  // Re-display the current code lock
    }
}

bool isCorrectCode() {
    for (int i = 0; i < 3; i++) {
        if (enteredCode[i] != correctCodes[codeIndex][i]) {
            return false;  // If any number doesn't match, return false
        }
    }
    return true;  // If all numbers match, return true
}


void displayCode() {
    oled.clear();

    // Display the "Code Lock" label and current code index
    oled.setScale(1);  // Use default size for labels
    oled.setCursorXY(5, 5);
    oled.print("Code Lock: ");
    oled.print(codeIndex + 1);

    // Display "Code:" label
    oled.setCursorXY(5, 20);
    oled.print("Code: ");

    // Move to the next line for displaying the numbers
    oled.setCursorXY(30, 40);  // Shift numbers right by starting at X = 20
    oled.setScale(2);  // Increase the font size only for the code numbers

    // Print each number in the code, with highlighting for the selected number
    for (int i = 0; i < 3; i++) {
        if (i == numberIndex) {
            oled.invertText(true);  // Invert only the selected number
            oled.print(enteredCode[i]);
            oled.invertText(false); // Turn off inversion after the number
        } else {
            oled.print(enteredCode[i]);
        }
        oled.print(" ");  // Add a space without inversion
    }

    oled.update();
}



void displayWinMessage() {
    oled.clear();
    displayQRCode(win_message); 
    oled.update();
    while(!back_key_pressed){ 
      ReadAndProcessKeyStatus();
      delay(10);
    }
}

void run_treasure_hunt() {
    showInstructions();
    back_key_pressed = false;
    codeIndex = 0;               // Reset to first code
    numberIndex = 0;             // Start highlighting the first number
    enteredCode[0] = enteredCode[1] = enteredCode[2] = 1;
    first_entry = true;
    while (!back_key_pressed) { 
        int moveHorizontal = 0;  // Track horizontal movement (left/right)
        int moveVertical = 0;    // Track vertical movement (up/down)
        
        displayCode(); 
        
        if (KeyStatus0 & 0x04) {  // Go down
            moveVertical = -1;                                                                                                     
        }
        if (KeyStatus0 & 0x02) {  // Go up
            moveVertical = 1;
        }
        if (KeyStatus0 & 0x08) {  // Go right
            moveHorizontal = 1;
        }
        if (KeyStatus0 & 0x01) {  // Go left
            moveHorizontal = -1;
        }
        if (moveHorizontal != 0) switchNumber(moveHorizontal);  // Switch between number positions
        if (moveVertical != 0) adjustNumber(moveVertical);      // Adjust the selected number

        if (KeyStatus0 & 0x10) {  // OK button pressed
            checkCode();  // Check if the entered code matches the correct one
            first_entry = false;
            delay(500);  
        }

        if (codeIndex == 3) {
            displayWinMessage();  // Display "You won!" if all codes are correct
        }

        ReadAndProcessKeyStatus();
        delay(10);
    }
    treasure_hunt = false;
    back_key_pressed = false;
    delay(100);
}
