/* 
Author: S.L.
11/2014 initial release
05/2015 updated for changed hardware
06/2015 moved from vusb-for-arduino to ProTrinketHidCombo library

USB HID device to provide configrable keyboard shortcuts for 
various tasks (multimedia control, Photoshop,...)

Inteded for Adafruit Pro Trinket http://www.adafruit.com/product/2000
  PD2 / digital 2 is D+
  PD7 / digital 7 is D-

Notes:
- Originally intended to be used with keypad library
  http://playground.arduino.cc/code/Keypad
  but was abandoned because of conflicts in the usage of TIMSK0.
- See https://learn.adafruit.com/trinket-usb-volume-knob
  for instructions on rotary encoders
- Originally used VUSB for Arduino https://code.google.com/p/vusb-for-arduino/
  but changed to Adafruit ProTrinketHidCombo library https://github.com/adafruit/Pro-Trinket-Rotary-Encoder
  due to compatibility issues with Arduino 1.6.4. Also allows usage of multimedia keys.
  See https://learn.adafruit.com/pro-trinket-rotary-encoder for more information.
  Requires change to the library as explained here: https://hackaday.io/project/3502-pro-trinket-usb-keyboard/log/18867-move-to-adafruits-protrinkethidcombo-lib
  
*/

//setup USB
#include <ProTrinketHidCombo.h>

//kaypad & encoder definitions
int colPins[] = { 4, 15, 13, 12};  //select pins, driven LOW to select col => btn active LOW
int rowPins[] = {17, 18, 19};      //signal pins, read LOW means btn pressed
int encPins[] = {14, 16};          //encoder signal pins
int const rows = sizeof(rowPins)/sizeof(int);
int const cols = sizeof(colPins)/sizeof(int);

//array for prev btn states to detect distinct presses and releases
int key_prev[rows][cols] = {
  { 1, 1, 1, 1},
  { 1, 1, 1, 1},
  { 1, 1, 1, 1}
};
//array for prev encoder A pin state to detect rotation direction
int enc_prev[cols] = {
  1, 1, 1, 1
};

//misc UI definitions
int const modes = 3;    //number of modes/different keysets
int mode = 0;           //curent mode
//btn to toggle through modes
int modeBtn = 3;
int modeBtnPrev = 1;
//LEDs to indicate current mode
int ledPins[] = {11, 10, 9};
int leds = sizeof(ledPins)/sizeof(int);

//key sets for different modes
//usb codes for keypad buttons (4x3) in different modes (2) plus optional modifier
//see additional file for codes
int usb_codes[modes][rows][cols][2] = {
  //dialpad
  {
    {{KEYCODE_1,0},{31,0},{32,0},{33,0}},
    {{34,0},{35,0},{36,0},{37,0}},
    {{38,0},{39,0},{10,0},{21,0}}
  },
  //multimedia
  {
    {{30,0},{31,0},{32,0},{33,0}},
    {{34,0},{35,0},{36,0},{37,0}},
    {{38,0},{10,0},{39,0},{21,0}}
  },
  //eagle
  {
    {{KEYCODE_F2,KEYCODE_MOD_LEFT_ALT},{KEYCODE_F2,0},{KEYCODE_F6,0},{18,0}},
    {{KEYCODE_M,KEYCODE_MOD_LEFT_CONTROL},{KEYCODE_G,KEYCODE_MOD_LEFT_CONTROL},{14,0},{KEYCODE_D,KEYCODE_MOD_LEFT_CONTROL}},
    {{KEYCODE_R,KEYCODE_MOD_LEFT_CONTROL},{10,0},{39,0},{43,KEYCODE_MOD_LEFT_ALT}}
  }
};
//encoder actions. 1 enc per col, cw & ccw direction (2) plus optional modifier (2)
int usb_codes_enc[modes][cols][2][2] = {
  //dialpad
  {
    {{81, 0},{82, 0}},
    {{30, 0},{31, 0}},
    {{30, 0},{31, 0}},
    {{30, 0},{31, 0}}
  },
  //multimedia
  {
    {{233, 0},{234, 0}},
    {{81, 0},{82, 0}},
    {{81, 0},{82, 0}},
    {{81, 0},{82, 0}}
  },
  //eagle
  {
    {{KEYCODE_F3, 0},{KEYCODE_F4, 0}},
    {{KEYCODE_Y, KEYCODE_MOD_LEFT_CONTROL},{KEYCODE_Z, KEYCODE_MOD_LEFT_CONTROL}},
    {{81, 0},{82, 0}},
    {{81, 0},{82, 0}}
  }
};

void setup(){
  //set data direction for keypad
  //cols: select, drive low to select
  for(int i=0; i<cols; i++){
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }
  //rows: sense/read, active low
  for(int i=0; i<rows; i++){
    pinMode(rowPins[i], INPUT_PULLUP);
  }
  //set up encoder inputs
  pinMode(encPins[0], INPUT_PULLUP);
  pinMode(encPins[1], INPUT_PULLUP);
  //mode btn
  pinMode(modeBtn, INPUT_PULLUP);
  //mode LEDs initialization & startup animation
  for(int i=0; i<leds; i++){
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
    delay(200);
    digitalWrite(ledPins[i], LOW);
  }
  delay(500);
  
  //USB
  TrinketHidCombo.begin();
  
  while(!TrinketHidCombo.isConnected()){
    TrinketHidCombo.poll();
  }
  
  digitalWrite(ledPins[mode], HIGH);
}

void loop(){
  //poll USB
  TrinketHidCombo.poll();
  
  //break criteria for while loop (to service USB)
  int count = 0;
  //poll inputs 250us=>7; 100us=>18; no delay=>504
  while(count < 504){
    count ++;
    
    //poll mode button
    if(digitalRead(modeBtn) == 0 && modeBtnPrev == 1){
      modeBtnPrev = 0;
      mode += 1;
      if(mode >= modes){
        mode = 0;
      }
      //change LEDs
      //turn off all LEDs
      for(int i=0; i<leds; i++){
        digitalWrite(ledPins[i], LOW);
      }
      //turn on current LED
      digitalWrite(ledPins[mode], HIGH);
      
      //TrinketHidCombo.pressMultimediaKey(MMKEY_MUTE);
    }
    else if (digitalRead(modeBtn) == 1 && modeBtnPrev == 0){
      modeBtnPrev = 1;
    }
    
    //cycle through cols
    for(int i=0; i<cols; i++){
      //enable col (active low)
      digitalWrite(colPins[i], LOW);

      //poll rows
      for(int j=0; j<rows; j++){
        //read state of current btn
        int but = digitalRead(rowPins[j]);    //Button Under Test
        
        //btn was just pressed (0), which means prev state was 1 (unpressed)
        if(but == 0 && key_prev[j][i] == 1){
          //set new prev state
          key_prev[j][i] = 0;
          
          //execute keystroke
          TrinketHidCombo.pressKey(usb_codes[mode][j][i][1], usb_codes[mode][j][i][0]);
          TrinketHidCombo.pressKey(0, 0);
        }
        //key release
        else if(but == 1 && key_prev[j][i] == 0){
          //set new prev state
          key_prev[j][i] = 1;
        }
      }
      // /poll rows

      //poll encoder
      int A = digitalRead(encPins[0]);
      int B = digitalRead(encPins[1]);
      //detect rotation
      if(A == 0 && enc_prev[i] == 1){
        //change prev state
        enc_prev[i] = 0;
        
        //determine rotation direction (cw/ccw)
        if(B == 1){
          //cw rotation ...orwhatever? 
          
          //execute keystroke
          TrinketHidCombo.pressKey(usb_codes_enc[mode][i][0][1], usb_codes_enc[mode][i][0][0]);
          TrinketHidCombo.pressKey(0, 0);
        }
        else if(B == 0){
          //ccw rotation ...orwhatever?
          
          //execute keystroke
          TrinketHidCombo.pressKey(usb_codes_enc[mode][i][1][1], usb_codes_enc[mode][i][1][0]);
          TrinketHidCombo.pressKey(0, 0);
        }
      }
      else if(A == 1 && B == 1){
        //no input on encoder pins
        enc_prev[i] = 1;
      }
      // /poll encoder

      //disable col
      digitalWrite(colPins[i], HIGH);
      
      //delayMicroseconds(250);    //software debounce
      //delayMicroseconds(100);
    }
    // /cycle through cols
  }
}

void executeKeystroke(){
  
}

void delayMs(unsigned int ms){
  for (int i = 0; i < ms; i++) {
    delayMicroseconds(1000);
  }
}

