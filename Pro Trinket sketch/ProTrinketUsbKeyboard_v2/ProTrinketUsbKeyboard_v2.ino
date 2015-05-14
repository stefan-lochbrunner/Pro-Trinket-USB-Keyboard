/* 
Author: S.L.
11/2014 initial release
05/2015 updated for changed hardware

USB HID device to provide configrable keyboard shortcuts for 
various tasks (multimedia control, Photoshop,...)

Inteded for Adafruit Pro Trinket http://www.adafruit.com/product/2000
  PD2 / digital 2 is D+
  PD7 / digital 7 is D-
  PB5 / digital 13 has LED
Based on VUSB for Arduino https://code.google.com/p/vusb-for-arduino/
  PD5 / digital 5 used for D- pull-up

Notes:
- Originally intended to be used with keypad library
  http://playground.arduino.cc/code/Keypad
  but was abandoned because of conflicts in the usage of TIMSK0.
- Requires modificatonC:\Users\Steve\Downloads\Dropbox\Apps\arduino-1.0.5 of D- pull-up circuit to allow VUSB to disconnect and reconnect
  to be recognized as HID keyboard instead on USBtiny device.
  Manual switch added to select between pull-up to Vdd and connection to PD5 as outlined
  in usbconfig.h.
- See https://learn.adafruit.com/trinket-usb-volume-knob
  for instructions on rotary encoders
  
*/

//setup USB
#include "UsbKeyboard.h"

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
    {{30,0},{31,0},{32,0},{33,0}},
    {{34,0},{35,0},{36,0},{37,0}},
    {{38,0},{39,0},{10,0},{21,0}}
  },
  //multimedia
  {
    {{30,0},{31,0},{32,0},{33,0}},
    {{34,0},{35,0},{36,0},{37,0}},
    {{38,0},{10,0},{39,0},{21,0}}
  },
  //more keys
  {
    {{11,0},{ 8,0},{15,0},{18,0}},
    {{ 4,0},{ 6,0},{14,0},{ 7,0}},
    {{29,0},{10,0},{39,0},{44,0}}
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
    {{81, 0},{82, 0}},
    {{81, 0},{82, 0}},
    {{81, 0},{82, 0}},
    {{81, 0},{82, 0}}
  },
  //misc
  {
    {{81, 0},{82, 0}},
    {{81, 0},{82, 0}},
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
  //mode LEDs
  for(int i=0; i<leds; i++){
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
    delayMs(200);
    digitalWrite(ledPins[i], LOW);
  }
  digitalWrite(ledPins[mode], HIGH);
  
  
  //USB
  TIMSK0&=!(1<<TOIE0);
  // Clear interrupts while performing time-critical operations
  cli();
  // Force re-enumeration so the host will detect us
  usbDeviceDisconnect();
  delayMs(250);
  usbDeviceConnect();
  // Set interrupts again
  sei();
  
}

void loop(){
  //Poll USB
  UsbKeyboard.update();
  
  //break criteria for while loop (to service USB)
  int count = 0;
  //poll inputs
  while(count < 100){
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
          UsbKeyboard.sendKeyStroke(usb_codes[mode][j][i][0]);
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
          UsbKeyboard.sendKeyStroke(usb_codes_enc[mode][i][0][0]);
        }
        else if(B == 0){
          //ccw rotation ...orwhatever?
          
          //execute keystroke
          UsbKeyboard.sendKeyStroke(usb_codes_enc[mode][i][1][0]);
        }
      }
      else if(A == 1 && B == 1){
        //no input on encoder pins
        enc_prev[i] = 1;
      }
      // /poll encoder

      //disable col
      digitalWrite(colPins[i], HIGH);
      
      delayMicroseconds(250);    //software debounce
    }
    // /cycle through cols
  }
}

void delayMs(unsigned int ms){
  for (int i = 0; i < ms; i++) {
    delayMicroseconds(1000);
  }
}
