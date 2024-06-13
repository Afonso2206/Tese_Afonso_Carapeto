/*
 * start tracker basic camera interface
 * 2024 Rui Duarte
 * 
 * SDA gpio15   SCL gpio16
 * I2C device addr = 0x10, wr addr = 0x20, rd addr = 0x21
 * chip version reg = 0x3000 - default value = 0x2406
 * https://github.com/oresat/oresat-prucam-ar013x/tree/master/testing/camera-test-c
 */


#include <Arduino.h>
#include "I2C_ST.h"

#define I2C_DEV_ADR 0x10

// azul (pino16/21)
#define PIN_SDA 16
// amarelo (pino17/22)
#define PIN_SCL 17
#define PIN_RESET 15
#define PIN_STDBY 14


typedef struct
{
  uint16_t reg;
  uint16_t val;
}camReg;

#include "regs.h"





i2c_st i2c_soft;

bool failure = false;
uint16_t reg_data;
const uint8_t i2c_addr = I2C_DEV_ADR; // do not forget about the shift left on the i2c address


unsigned int reg, val;



#define ROWS 960
#define COLS 1280
#define PIXELS ROWS * COLS







bool camera_reset() {
  i2c_soft.transmission_begin(i2c_addr);
  failure = i2c_soft.transmission_reg_wr(DEVICE_RESET, 0x0001);  // set reset
  i2c_soft.transmission_end();
  if(failure == LOW){
    Serial.print("RESET: OK  ");
  }
  delay(200);

  i2c_soft.transmission_begin(i2c_addr);
  reg_data = i2c_soft.transmission_reg_rd(DEVICE_ID);  // get device ID
  i2c_soft.transmission_end();

  Serial.print("Dev #"); Serial.println(i2c_addr, HEX); 
  Serial.print("Reg #"); Serial.print(DEVICE_ID, HEX); Serial.print(" = 0x"); Serial.println(reg_data, HEX);
  
  if(reg_data == 0x2406) {
    Serial.println("Device ID match!");  
    return LOW;
  }
  else return HIGH;
}

 


bool camera_setup() {
  int i, m = sizeof(startupRegs) / sizeof(camReg);
  Serial.println("*** camera setup ***");
  for(i = 0; i < m; i++) {
    Serial.print("Reg #"); Serial.print(i); Serial.print("/"); Serial.print(m-1);
    Serial.print(": 0x"); Serial.print(startupRegs[i].reg, HEX); Serial.print(" = 0x"); 
    Serial.println(startupRegs[i].val, HEX);
    i2c_soft.transmission_begin(i2c_addr);
    failure = i2c_soft.transmission_reg_wr(startupRegs[i].reg, startupRegs[i].val);  // get device ID
    i2c_soft.transmission_end();

    if(failure){ Serial.println("ERROR: reg wr failed!");
      return HIGH;
    }
  }
  return LOW;
}



void camera_status() {
  for(reg = 0x3000; reg <= 0x301A; reg+=2) {
    i2c_soft.transmission_begin(i2c_addr);
    val = i2c_soft.transmission_reg_rd(reg);  
    i2c_soft.transmission_end();
    Serial.print(": 0x"); Serial.print(reg, HEX); Serial.print(" = 0x"); Serial.println(val, HEX);

  }
}




















void setup()
{  
  Serial.begin(115200);
  while(!Serial);
  Serial.write(27); Serial.print("[2J"); Serial.write(27); Serial.print("[H"); // ANSI stuff
  
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_STDBY, OUTPUT);
  digitalWrite(PIN_RESET, LOW);   delay(100);
  digitalWrite(PIN_RESET, HIGH);  delay(100);
  digitalWrite(PIN_STDBY, HIGH);  delay(100);
  digitalWrite(PIN_STDBY, LOW);   delay(100);
  i2c_soft.begin(PIN_SDA, PIN_SCL);
  Serial.print("\n\n*** Start Tracker Camera I2C debug by RPD ***\t v0.1: "); Serial.print(__DATE__); Serial.print(" - "); Serial.println(__TIME__);
  
  failure = camera_reset();
  Serial.print("camera_reset(): "); if(failure) Serial.println("ERROR"); else Serial.println("OK"); 
  failure = camera_setup();
  Serial.print("camera_setup(): "); if(failure) Serial.println("ERROR"); else Serial.println("OK"); 

  camera_status();
}

 
void loop() 
{ 
  Serial.println("Register (dec): ");
  while (Serial.available() == 0) {}
  reg = Serial.parseInt();   Serial.println(reg);
  if(reg != 0) {
    i2c_soft.transmission_begin(i2c_addr);
    reg_data = i2c_soft.transmission_reg_rd(DEVICE_ID);  
    i2c_soft.transmission_end();
    Serial.print(" = 0x");  Serial.println(reg_data, HEX);
    Serial.println("New Value (dec): ");
    while (Serial.available() == 0) {}
    val = Serial.parseInt();   Serial.println(val);
    if(val != 0) {
      i2c_soft.transmission_begin(i2c_addr);
      failure = i2c_soft.transmission_reg_wr(reg, val);  
      i2c_soft.transmission_end();
    }
    
  }
    
}
