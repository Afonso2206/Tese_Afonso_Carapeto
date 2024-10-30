#ifndef OLED_FUNCTIONS_H
#define OLED_FUNCTIONS_H

#include <sstream>
#include "qrcodegen.hpp"

#include <GyverOLED.h>

extern GyverOLED<SSH1106_128x64> oled;

extern const char* menu_items[6];
extern const char* submenu_items[6][3];
extern const int menu_length;
extern int current_index;
extern int current_submenu_index;
extern bool in_submenu;
extern int submenu_length;

extern const char* Student_Info;

extern byte percent;

using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

//---------------------------------------------------------------------------------------------------------------//

static void doBasicDemo();
static std::string toSvgString(const QrCode &qr, int border);

void DisplayStartupSequence();    //Initializes the display and prints IST Logo 

void update_menu();               //Updates the Main Menu and sub menus, based on the option chosen

void print_student_data();        //Prints the student data

void displayQRCode(const char *text);       //Displays the QR Code

int translateX(int x, int y, int border, int qrSize);   //Centers the QR Code in the X axis

int translateY(int x, int y, int border, int qrSize);   //Centers the QR Code in the Y axis

void digital_Voltmeter();       //Digital voltmeter

void analog_Voltmeter();        //Analog Voltmeter

void drawVoltmeter();     //Draws the analog Voltmeter 

void drawNeedle(float voltage);   //Draws the needle of the analog voltmeter

void Focus_Preparation();

void Focus_Attention();

void fillFocusBar(byte percent);

void print_alameda_info();

void print_taguspark_info();

#endif
