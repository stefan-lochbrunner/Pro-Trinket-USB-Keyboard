// use these masks with the "move" function
// #define MOUSEBTN_LEFT_MASK    0x01
// #define MOUSEBTN_RIGHT_MASK   0x02
// #define MOUSEBTN_MIDDLE_MASK  0x04

// LED state masks
// #define KB_LED_NUM      0x01
// #define KB_LED_CAPS     0x02
// #define KB_LED_SCROLL   0x04

// some convenience definitions for modifier keys
#define KEYCODE_MOD_LEFT_CONTROL  0x80
#define KEYCODE_MOD_LEFT_SHIFT    0x81
#define KEYCODE_MOD_LEFT_ALT    0x82
#define KEYCODE_MOD_LEFT_GUI    0x83
#define KEYCODE_MOD_RIGHT_CONTROL 0x84
#define KEYCODE_MOD_RIGHT_SHIFT   0x85
#define KEYCODE_MOD_RIGHT_ALT   0x86
#define KEYCODE_MOD_RIGHT_GUI   0x87

// some more keycodes
#define KEYCODE_A       'a'
#define KEYCODE_B       'b'
#define KEYCODE_C       'c'
#define KEYCODE_D       'd'
#define KEYCODE_E       'e'
#define KEYCODE_F       'f'
#define KEYCODE_G       'g'
#define KEYCODE_H       'h'
#define KEYCODE_I       'i'
#define KEYCODE_J       'j'
#define KEYCODE_K       'k'
#define KEYCODE_L       'l'
#define KEYCODE_M       'm'
#define KEYCODE_N       'n'
#define KEYCODE_O       'o'
#define KEYCODE_P       'p'
#define KEYCODE_Q       'q'
#define KEYCODE_R       'r'
#define KEYCODE_S       's'
#define KEYCODE_T       't'
#define KEYCODE_U       'u'
#define KEYCODE_V       'v'
#define KEYCODE_W       'w'
#define KEYCODE_X       'x'
#define KEYCODE_Y       'y'
#define KEYCODE_Z       'z'

#define KEYCODE_1       '1'
#define KEYCODE_2       '2'
#define KEYCODE_3       '3'
#define KEYCODE_4       '4'
#define KEYCODE_5       '5'
#define KEYCODE_6       '6'
#define KEYCODE_7       '7'
#define KEYCODE_8       '8'
#define KEYCODE_9       '9'
#define KEYCODE_0       '0'

#define KEYCODE_ENTER      0xB0
#define KEYCODE_ESC       0xB1
#define KEYCODE_BACKSPACE   0xB2
#define KEYCODE_TAB       0xB3
#define KEYCODE_SPACE     ' '
#define KEYCODE_MINUS     '-'
#define KEYCODE_EQUAL     '='
#define KEYCODE_SQBRAK_LEFT   '['
#define KEYCODE_SQBRAK_RIGHT  ']'
#define KEYCODE_BACKSLASH   ''\''

#define KEYCODE_COMMA     ','
#define KEYCODE_PERIOD      '.'
#define KEYCODE_SLASH     '/'

#define KEY_CAPS_LOCK   0xC1

#define KEYCODE_F1        0xC2
#define KEYCODE_F2        0xC3
#define KEYCODE_F3        0xC4
#define KEYCODE_F4        0xC5
#define KEYCODE_F5        0xC6
#define KEYCODE_F6        0xC7
#define KEYCODE_F7        0xC8
#define KEYCODE_F8        0xC9
#define KEYCODE_F9        0xCA
#define KEYCODE_F10       0xCB
#define KEYCODE_F11       0xCC
#define KEYCODE_F12       0xCD

// #define KEYCODE_PRINTSCREEN   0x46

#define KEYCODE_INSERT      0xD1
#define KEYCODE_HOME      0xD2
#define KEYCODE_PAGE_UP     0xD3
#define KEYCODE_DELETE      0xD4
#define KEYCODE_END       0xD5
#define KEYCODE_PAGE_DOWN   0xD6
#define KEYCODE_ARROW_RIGHT   0xD7
#define KEYCODE_ARROW_LEFT    0xD8
#define KEYCODE_ARROW_DOWN    0xD9
#define KEYCODE_ARROW_UP    0xDA

// #define KEYCODE_APP       0x65

#define KEYCODE_LEFT_CONTROL   0x80
#define KEYCODE_LEFT_SHIFT    0x81
#define KEYCODE_LEFT_ALT    0x82
#define KEYCODE_LEFT_GUI    0x83
#define KEYCODE_RIGHT_CONTROL    0x84
#define KEYCODE_RIGHT_SHIFT   0x85
#define KEYCODE_RIGHT_ALT   0x86
#define KEYCODE_RIGHT_GUI   0x87

// multimedia keys **not working for MP
#define MMKEY_KB_EXECUTE    0x74
#define MMKEY_KB_HELP     0x75
#define MMKEY_KB_MENU     0x76
#define MMKEY_KB_SELECT     0x77
#define MMKEY_KB_STOP     0x78
#define MMKEY_KB_AGAIN      0x79
#define MMKEY_KB_UNDO     0x7A
#define MMKEY_KB_CUT      0x7B
#define MMKEY_KB_COPY     0x7C
#define MMKEY_KB_PASTE      0x7D
#define MMKEY_KB_FIND     0x7E
#define MMKEY_KB_MUTE     0x7F // do not use
#define MMKEY_KB_VOL_UP     0x80 // do not use
#define MMKEY_KB_VOL_DOWN   0x81 // do not use

#define MMKEY_SCAN_NEXT_TRACK 0xB5
#define MMKEY_SCAN_PREV_TRACK 0xB6
#define MMKEY_STOP        0xB7

#define MMKEY_PLAYPAUSE     0xCD

#define MMKEY_MUTE        0xE2
#define MMKEY_BASSBOOST     0xE5
#define MMKEY_LOUDNESS      0xE7

#define MMKEY_VOL_UP      0xE9
#define MMKEY_VOL_DOWN      0xEA

// system control keys
// #define SYSCTRLKEY_POWER    0x01
// #define SYSCTRLKEY_SLEEP    0x02
// #define SYSCTRLKEY_WAKE     0x03
