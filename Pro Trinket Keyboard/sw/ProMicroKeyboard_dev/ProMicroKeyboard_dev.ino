/*
Author: S.L.
Amendments by C Hissett
11/2014 initial release
05/2015 updated for changed hardware
06/2015 moved from vusb-for-arduino to ProTrinketHidCombo library
09/2015 updated to utilize multimedia keys, see notes for limitations
11/2015 code forked and amended for ATMega32u4 Arduino boards (exchanging ProTrinketHidCombo.h for Keyboard.h
12/2015 adaptations for hw v3.4a

USB HID device to provide configrable keyboard shortcuts for
various tasks (multimedia control, Photoshop,...)
Inteded for zAdafruit Pro Trinket http://www.adafruit.com/product/2000
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
- Limitations:
    # To easily distinguish between 'normal' keys and multimedia keys, KEYCODE_*_CONTROL/SHIFT/ALT/GUI can't be used. Keycodes below 110 (chosen arbitrarily) will be interpreted as 'normal' keys and keycodes above will be interpreted as multimedia keys. In the future, kecodes larger than 234 might be used control mouse.
*/

//setup USB
#include <Keyboard.h>
//setup WS2812B lib
#include <Adafruit_NeoPixel.h>
//include keycodes
#include "keycodesPM.h"
/*temp****************
//better do something like this:
define ProMicro or define ProTrinket
#ifdef ProMicro
  #include "keycodesPM.h"
#endif
#ifdef ProTrinket
  #include "keycodesPT.h"
#endif
*/
//do the same for pin config

//keypad & encoder definitions
int colPins[] = { 4, 19, 15, 14};  //select pins, driven LOW to select col => btn active LOW
int rowPins[] = {21, 16, 10};      //signal pins, read LOW means btn pressed
int encPins[] = {20, 18};          //encoder signal pins
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
int modeColor[modes][3] = {{16,16,16},{0,0,64},{64,0,0}};   //RGB values for every mode
int mode = 0;           //curent/starting mode
//btn to toggle through modes
int modeBtn = 3;
int modeBtnPrev = 1;
//WS2812B LED to indicate current mode
int ledPin = 9;
int ledCount = 4;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(ledCount, ledPin);

//key sets for different modes
//usb codes for buttons (4x3) in different modes (3). each button can cause 5 keystrokes + optional modifier. see ProTrinketHidCombo.h for more information.
//see additional file for codes. for multiple modifiers combine them with bitwise or.
int usb_codes[modes][rows][cols][6] = {
  //dialpad
  {      // \/ col; {modifier,keycode1,keycode2,keycode3,keycode4,keycode5}
    {{0,KEYCODE_1,0,0,0,0},{0,KEYCODE_2,0,0,0,0},{0,KEYCODE_3,0,0,0,0},{0,KEYCODE_4,0,0,0,0}},    // <-row
    {{0,KEYCODE_5,0,0,0,0},{0,KEYCODE_6,0,0,0,0},{0,KEYCODE_7,0,0,0,0},{0,KEYCODE_8,0,0,0,0}},
    {{0,KEYCODE_9,0,0,0,0},{0,KEYCODE_0,0,0,0,0},{0,KEYCODE_R,0,0,0,0},{0,KEYCODE_G,0,0,0,0}}
  },
  //multimedia
  {
    {{0,MMKEY_MUTE,0,0,0,0},{0,MMKEY_PLAYPAUSE,0,0,0,0},{1,KEYCODE_R,0,0,0,0},{0,0,0,0,0,0}},
    //mute,playpause,ctrl+r,
    {{8,KEYCODE_ARROW_LEFT,0,0,0,0},{3,KEYCODE_TAB,0,0,0,0},{1,KEYCODE_TAB,0,0,0,0},{8,KEYCODE_ARROW_RIGHT,0,0,0,0}},
    //win+<,ctrl+shift+tab,ctrl+tab,win+>
    {{8,0,0,0,0,0},{1,KEYCODE_C,0,0,0,0},{1,KEYCODE_V,0,0,0,0},{0,0,0,0,0,0}}
    //win,ctrl+c,ctrl+v,
  },
  //eagle
  {
    {{4,KEYCODE_F2,0,0,0,0},{0,KEYCODE_F2,0,0,0,0},{0,KEYCODE_F6,0,0,0,0},{1,KEYCODE_S,0,0,0,0}},
    //view all,redraw,grid toggle,save
    {{1,KEYCODE_M,0,0,0,0},{1,KEYCODE_G,0,0,0,0},{3,KEYCODE_A,0,0,0,0},{1,KEYCODE_D,0,0,0,0}},
    //move,group,add,delete
    {{1,KEYCODE_R,0,0,0,0},{3,KEYCODE_R,0,0,0,0},{3,KEYCODE_N,0,0,0,0},{0,KEYCODE_F5,0,0,0,0}}
    //route,ripup,name,view to cursor
  }
};
//encoder actions. 1 enc per col, cw & ccw direction (2) plus optional modifier (2)
//1st is ccw, 2nd is cw
int usb_codes_enc[modes][cols][2][6] = {
  //dialpad
  {
    {{0,81,0,0,0,0},{0,82,0,0,0,0}},
    {{0,79,0,0,0,0},{0,80,0,0,0,0}},
    {{0,30,0,0,0,0},{0,31,0,0,0,0}},
    {{0,30,0,0,0,0},{0,31,0,0,0,0}}
  },
  //multimedia encoders
  {
    {{0,MMKEY_VOL_DOWN,0,0,0,0},{0,MMKEY_VOL_UP,0,0,0,0}},          //volume down, up
    {{0,MMKEY_SCAN_PREV_TRACK,0,0,0,0},{0,MMKEY_SCAN_NEXT_TRACK,0,0,0,0}},      //prev, next track
    {{1,KEYCODE_Y,0,0,0,0},{1,KEYCODE_Z,0,0,0,0}},                  //undo, redo
    {{0,KEYCODE_ARROW_DOWN,0,0,0,0},{0,KEYCODE_ARROW_UP,0,0,0,0}}   //scroll down, up (arrow keys)
  },
  //eagle
  {
    {{0,KEYCODE_F4,0,0,0,0},{0,KEYCODE_F3,0,0,0,0}},    //zoom
    {{4,KEYCODE_2,0,0,0,0},{4,KEYCODE_1,0,0,0,0}},      //switch sch & brd
    {{3,KEYCODE_U,0,0,0,0},{1,KEYCODE_U,0,0,0,0}},      //ratsnest, ripup @;
    {{1,KEYCODE_Y,0,0,0,0},{1,KEYCODE_Z,0,0,0,0}}       //undo,redo
  }
};

void setup(){
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  
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
  /*
  for(int i=0; i<leds; i++){
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
    delay(200);
    digitalWrite(ledPins[i], LOW);
  }
  */
  strip.begin();
  strip.show();

  //USB
  //TIMSK0&=!(1<<TOIE0);
  Keyboard.begin();
  
  //while(!TrinketHidCombo.isConnected()){
    //TrinketHidCombo.poll();
  //turn on LED once USB connection is up
  //analogWrite(ledPins[mode], 64);
  strip.setPixelColor(0, strip.Color(modeColor[mode][0], modeColor[mode][1], modeColor[mode][2]));
  strip.show();
}

void loop(){
  //poll USB
  //TrinketHidCombo.poll();

  //poll inputs

  //poll mode button
  if(digitalRead(modeBtn) == 0 && modeBtnPrev == 1){
    modeBtnPrev = 0;
    mode += 1;
    if(mode >= modes){
      mode = 0;
    }
    //change LEDs
    /*
    //turn off all LEDs
    for(int i=0; i<leds; i++){
      digitalWrite(ledPins[i], LOW);
    }
    //turn on current LED if USB is still connected. prevents LEDs being on if PC isn't
    //if(TrinketHidCombo.isConnected()){
      analogWrite(ledPins[mode], 64);
    */
    strip.setPixelColor(0, strip.Color(modeColor[mode][0], modeColor[mode][1], modeColor[mode][2]));
    strip.show();
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
        executeKeystroke(usb_codes[mode][j][i]);
      }
      //key release
      else if(but == 1 && key_prev[j][i] == 0){
        //set new prev state
        key_prev[j][i] = 1;
      }
    }

    //poll encoder
    int A = digitalRead(encPins[0]);
    int B = digitalRead(encPins[1]);
    //detect rotation
    if(A == 0 && enc_prev[i] == 1){
      //change prev state
      enc_prev[i] = 0;

      //determine rotation direction (cw/ccw)
      if(B == 1){
        //ccw rotation

        //execute keystroke
        executeKeystroke(usb_codes_enc[mode][i][0]);
      }
      else if(B == 0){
        //cw rotation

        //execute keystroke
        executeKeystroke(usb_codes_enc[mode][i][1]);
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
    //delay(250);
  }
  // /cycle through cols
}

//differentiate between keycode and multimedia keycode
void executeKeystroke(int* keycombo){
  Serial.println(keycombo[1]);
  if(keycombo[1] < 110){    // || keycombo[1] == 224 || keycombo[1] == 225 || keycombo[1] == 227 || keycombo[1] == 228 || keycombo[1] == 230){
    if(keycombo[0]){        //press modifier only if there is one
      Keyboard.press(keycombo[0]);
    }
    Keyboard.press(keycombo[1]);
    Keyboard.releaseAll();      //release button(s)
  }
  else if(keycombo[1] >= 110 && keycombo[1] < 235){
    Keyboard.press(keycombo[1]);
    Keyboard.releaseAll();      //release button(s)
  }
}
