#include "oled.h"

const char* menu_items[6] = {
    "Student Data",
    "Games",
    "Voltmeter",
    "Treasure Hunt",
    "Focus Attention",
    "IST Info"
  };

const char* submenu_items[6][3] = {
  {"See student data", "Show QR Code", nullptr},      // Submenu options for Student Data
  {"Snake", "Tic Tac Toe", nullptr},            // Submenu options for Games
  {"Digital", "Analog", nullptr},    // Submenu options for Voltmeter
  {"Treasure Hunt", nullptr},  // Submenu options for I2C Explorer
  {"Focus Attention", nullptr},  // Submenu options for Focus Attention
  {"IST Alameda", "IST Taguspark", nullptr}       // Submenu options for IST Info
};
  
const int menu_length = sizeof(menu_items) / sizeof(menu_items[0]);
int current_index = 0;
int current_submenu_index = 0;
bool in_submenu = false;
int submenu_length = 0;

const char* Student_Info = "BEGIN:VCARD\n"
                       "VERSION:3.0\n"
                       "FN;CHARSET=UTF-8:Afonso Santos Carapeto\n"
                       "N;CHARSET=UTF-8:Carapeto;Afonso;Santos;;\n"
                       "EMAIL;CHARSET=UTF-8;type=HOME,INTERNET:afonso.carapeto@tecnico.ulisboa.pt\n"
                       "TEL;TYPE=CELL:+351 929264325\n"
                       "NOTE;CHARSET=UTF-8:MEEC nº 93763\n"
                       "REV:2024-01-16T14:41:13.993Z\n"
                       "END:VCARD\n"; 

byte percent = 0;
//-------------------------------------------------------------------------------------------------------------------//

void DisplayStartupSequence() {

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

    //Initializes OLED and Displays IST Logo 
    oled.init();        
    oled.clear();
    oled.drawBitmap(0, 0, IST_Logo, 128, 64);
    oled.update();
    delay(2000); 
}

//---------------------------------------------------------------------------------------------------//

void update_menu() {    //Displays the Main Menu and submenus, inverting the text while the user toggles between the different options
  oled.clear();
  oled.home();
  oled.setScale(1);

  if (in_submenu) {
    // Display submenu options
    oled.invertText(false); // Turn off inversion before printing submenu
    oled.println(menu_items[current_index]);
    oled.println("");
    submenu_length = 0;
    //submenu_length = sizeof(submenu_items[current_index]) / sizeof(const char*);
    while (submenu_items[current_index][submenu_length] != nullptr) {
        submenu_length++;
    }
    for (int k = 0; k < submenu_length; k++) { // Iterate through submenu options
      if (k == current_submenu_index) {
        oled.invertText(true);
      } else {
        oled.invertText(false);
      }
      oled.println(submenu_items[current_index][k]);
    }
  } else {
    // Display main menu
    oled.invertText(false); // Turn off inversion before printing main menu
    oled.println("Main Menu");
    oled.println("");
    for (int j = 0; j < menu_length; j++) {
      if (j == current_index) {
        oled.invertText(true);
      } else {
        oled.invertText(false);
      }
      oled.println(menu_items[j]);
    }
  }

  oled.update();
}

//----------------------------------------------------------------------------------------//

void print_student_data(){

    oled.println("Afonso Carapeto");
    oled.println("IST 193763");
    oled.println("MEEC");
    oled.println("929264325");
    oled.println("afonso.carapeto");
    oled.println("@tecnico.ulisboa.pt");
    
}

//----------------------------------------------------------------------------------------//
//------------------------------------------QR CODE---------------------------------------//
//----------------------------------------------------------------------------------------//

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

void digital_Voltmeter(){
  
  int ADC_Value = analogRead(28);
  float voltage = (ADC_Value / 1023.0) * 3.3;
  oled.clear();
  oled.home();
  oled.setScale(2);
  oled.print(voltage);
  oled.print("V");
  oled.update();
  oled.setScale(1);
  
}

void analog_Voltmeter(){
  int ADC_Value = analogRead(28);
  float voltage = (ADC_Value / 1023.0) * 3.3;
  oled.clear();
  drawVoltmeter();
  drawNeedle(voltage);
  oled.update();
}

void drawVoltmeter() {

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

void Focus_Preparation(){   //Some prints before starting the Focus Attention feature
  
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
  oled.setCursorXY(20, 15);                 
  oled.print("Time elapsed");                      
  oled.update();
  
}

void Focus_Attention(){
  
  static unsigned long previousMillis = 0;
      unsigned long currentMillis = millis();
      const unsigned long interval = 600;  
      
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;  
    
        if (percent <= 100) {
          oled.setCursorXY(100, 15); 
          oled.print(percent);
          oled.print("%");
          fillFocusBar(percent);  // Fill the battery by percent
          percent++;
        } else {
          oled.clear();
          oled.println("Another round?");
          oled.update();
          delay(2000);
          percent = 0;  // Reset percent after reaching 100
          Focus_Preparation();
        }
      }
      
}

void fillFocusBar(byte percent) {
  //oled.rect(22, 32, 106, 48, OLED_CLEAR);  // Clear inside battery
  // Calculate width to fill based on percentage
  int fillWidth = (106 - 22) * percent / 100;
  // Fill Focus Bar
  oled.rect(22, 32, 22 + fillWidth, 48, OLED_FILL);
  oled.update();
}

void print_alameda_info(){
  oled.println("Av. Rovisco Pais");
  oled.println("1049-001 Lisboa");
  oled.println("218417000");
}

void print_taguspark_info(){
  oled.println("Av. Prof. Dr. Cavaco");
  oled.println("Silva");
  oled.println("2744-016 Porto Salvo");
  oled.println("214233200");
}
