
//Including Libraries
#include <Arduino.h>
#include <Debounce.h>
#include <BLEMidi.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

//BLEMIDI_CREATE_INSTANCE("EnterdeviceName", MIDI)       //uncomment and run once for adding a custom name on bluetooth device.


// For T18 v3.0 These GPIO is used for display control
#define OLED_SDA 21;
#define OLED_SCL 22;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define bitmap_height   128
#define bitmap_width    64

Adafruit_SH1106 display(21, 22);

bool isConnected = false;
unsigned long lastbuttonTime = 0;
unsigned long potTime = 0;
unsigned long voltageTime = 0;
unsigned long menuTime = 0;

float v;
int program = 0;
int Pushedchannel = 0;
int channel = 0;
int LastPot1;
int LastPot2;
int lastChannel;
double batVoltage;
float left;
int cc[2] = {101, 102};

/*Buttons and pots. BatteryvoltagePin is a voltagedivider connected to batteryvoltage. This value is used for
calculating the realtime voltage on 18650 battery.
*/
const int Button_1 = 12;
const int Button_2 = 13;
const int Button_3 = 14;
const int Button_4 = 25;
const int Button_5 = 26;
const int Button_6 = 27;
const int Potpin1 = 33;
const int Potpin2 = 34;
const int BatvoltagePin = 35;
const int DelayTime = 50;
const int ButtonDelay = 300;

// This lines includes debounce for every button connected.
Debounce Button1(Button_1, DelayTime, true);
Debounce Button2(Button_2, DelayTime, true);
Debounce Button3(Button_3, DelayTime, true);
Debounce Button4(Button_4, DelayTime, true);
Debounce Button5(Button_5, DelayTime, true);
Debounce Button6(Button_6, DelayTime, true);


static const unsigned char guitar1 [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0xFF, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xE0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xF8,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFD, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xF8, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xF8, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xF8, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xC0, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x01, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFE, 0x1F, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF8, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x07, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x03, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x0F, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x3F, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x1F, 0xFF, 0xFF, 0xFF, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x3F, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x1F, 0xFF, 0xF8, 0x03, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0xFF, 0xFC, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xF0, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0F, 0xFC, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xC0, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x7F, 0x80, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x01, 0xFC, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFC, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x03, 0xE0, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFC, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x0F, 0xC0, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF8, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x1F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF8, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00,
0x3E, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xF0, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
0x7C, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xC0, 0x01, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF8, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x01, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF0, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF0, 0x00, 0x1F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF8, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x78, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x7C, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x3C, 0x00, 0x00, 0x1F, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x3E, 0x00, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1F, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0F, 0xC0, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0xE0, 0x00, 0x00, 0x1F, 0xF0, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0xF8, 0x00, 0x00, 0x07, 0xF0, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x7C, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0xF8, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x3F, 0xC0, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFF, 0xF0, 0x1F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x7F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char guitar2 [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0xFF, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xE0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xF8,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFD, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xF8, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xF8, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xF8, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xC0, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x01, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFE, 0x1F, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF8, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x07, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x1C,
0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0xFC,
0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x0F, 0x0F, 0xFC,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x03, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x1F, 0xFF, 0xFC,
0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x0F, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x1F, 0xFF, 0x9C,
0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x3F, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x1C, 0x78, 0x1C,
0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x1F, 0xFF, 0xFF, 0xFF, 0xDF, 0x00, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x00, 0x00, 0x3F, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x80, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x00, 0x1F, 0xFF, 0xF8, 0x03, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xC0, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x01, 0xFF, 0xFC, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xF0, 0x03, 0xE0, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x0F, 0xFC, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xC0, 0x03, 0xE0, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x7F, 0x80, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0xE0, 0x00, 0x1C, 0x00, 0xFC,
0x01, 0xFC, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFC, 0x00, 0x01, 0xE0, 0x00, 0x1C, 0x03, 0xFC,
0x03, 0xE0, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFC, 0x00, 0x03, 0xE0, 0x01, 0xFC, 0x07, 0xFC,
0x0F, 0xC0, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF8, 0x00, 0x07, 0xC0, 0x07, 0xFC, 0x07, 0x1C,
0x1F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF8, 0x00, 0x0F, 0x80, 0x0F, 0xFC, 0x07, 0x1C,
0x3E, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xF0, 0x00, 0x3F, 0x00, 0x0E, 0x1C, 0x07, 0xFC,
0x7C, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xC0, 0x01, 0xFE, 0x00, 0x0E, 0x1C, 0x03, 0xF8,
0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x1F, 0xF8, 0x00, 0x0F, 0xFC, 0x01, 0xF0,
0xF8, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x01, 0xFF, 0xC0, 0x00, 0x07, 0xF8, 0x00, 0x00,
0xF0, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF8, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00,
0xF0, 0x00, 0x1F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF8, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x78, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x7C, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x3C, 0x00, 0x00, 0x1F, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x3E, 0x00, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1F, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0F, 0xC0, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0xE0, 0x00, 0x00, 0x1F, 0xF0, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0xF8, 0x00, 0x00, 0x07, 0xF0, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x7C, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0xF8, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x3F, 0xC0, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFF, 0xF0, 0x1F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x7F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char guitar3 [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0xFF, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xE0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xF8,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFD, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xF8, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xF8, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xF8, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xC0, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x01, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFE, 0x1F, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF8, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x07, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x1C,
0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0xFC,
0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x0F, 0x0F, 0xFC,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x03, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x1F, 0xFF, 0xFC,
0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x0F, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x1F, 0xFF, 0x9C,
0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x3F, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x1C, 0x78, 0x1C,
0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x1F, 0xFF, 0xFF, 0xFF, 0xDF, 0x00, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x00, 0x00, 0x3F, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x80, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x00, 0x1F, 0xFF, 0xF8, 0x03, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xC0, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x01, 0xFF, 0xFC, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xF0, 0x03, 0xE0, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x0F, 0xFC, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xC0, 0x03, 0xE0, 0x00, 0x1C, 0x00, 0x1C,
0x00, 0x7F, 0x80, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0xE0, 0x00, 0x1C, 0x00, 0xFC,
0x01, 0xFC, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFC, 0x00, 0x01, 0xE0, 0x00, 0x1C, 0x03, 0xFC,
0x03, 0xE0, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFC, 0x00, 0x03, 0xE0, 0x01, 0xFC, 0x07, 0xFC,
0x0F, 0xC0, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF8, 0x00, 0x07, 0xC0, 0x07, 0xFC, 0x07, 0x1C,
0x1F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF8, 0x00, 0x0F, 0x80, 0x0F, 0xFC, 0x07, 0x1C,
0x3E, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xF0, 0x00, 0x3F, 0x00, 0x0E, 0x1C, 0x07, 0xFC,
0x7C, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xC0, 0x01, 0xFE, 0x00, 0x0E, 0x1C, 0x03, 0xF8,
0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x1F, 0xF8, 0x00, 0x0F, 0xFC, 0x01, 0xF0,
0xF8, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x01, 0xFF, 0xC0, 0x00, 0x07, 0xF8, 0x00, 0x00,
0xF0, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF8, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00,
0xF0, 0x00, 0x1F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF8, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x78, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x7C, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x78,
0x3C, 0x00, 0x00, 0x1F, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xF8,
0x3E, 0x00, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xF8,
0x1F, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x3F, 0x38,
0x0F, 0xC0, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x38,
0x03, 0xE0, 0x00, 0x00, 0x1F, 0xF0, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x38,
0x01, 0xF8, 0x00, 0x00, 0x07, 0xF0, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x38,
0x00, 0x7C, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x38,
0x00, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x38,
0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x01, 0xEE, 0x01, 0xF8,
0x00, 0x03, 0xF8, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFE, 0x03, 0xF8,
0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFE, 0x07, 0xF8,
0x00, 0x00, 0x3F, 0xC0, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x1E, 0x07, 0x38,
0x00, 0x00, 0x07, 0xFC, 0x00, 0x00, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFC, 0x07, 0xF8,
0x00, 0x00, 0x00, 0xFF, 0xF0, 0x1F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF8, 0x03, 0xF0,
0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x01, 0xE0,
0x00, 0x00, 0x00, 0x00, 0x7F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



void setup() {
  Serial.begin(115200);
  Serial.println("Initializing bluetooth");
  BLEMidiServer.begin("Basic MIDI device");
  Serial.println("Waiting for connections...");
  //BLEMidiServer.enableDebugging();  // Uncomment if you want to see some debugging output from the library (not much for the server class...)

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();

  pinMode(Button_1, INPUT_PULLUP);
  pinMode(Button_2, INPUT_PULLUP);
  pinMode(Button_3, INPUT_PULLUP);
  pinMode(Button_4, INPUT_PULLUP);
  pinMode(Button_5, INPUT_PULLUP);
  pinMode(Button_6, INPUT_PULLUP); 
  pinMode(BatvoltagePin, INPUT);
  pinMode(Potpin1, INPUT);
  pinMode(Potpin2, INPUT);

  // This stops potentiometer from sending values all the time. A value is sent when the new value is different from last value.
  LastPot1 = analogRead(Potpin1);
  LastPot2 = analogRead(Potpin2);

  BatteryStatus();
  startup();
}

void loop() {

  // Display and batteryvoltage will be running as normal, but buttons and potensiometer will not run before bluetooth is connected.

  screenShow();
  BatteryMain();
  
  if(BLEMidiServer.isConnected()) { 
      ButtonMain(); 
      PotensiometerMain();   
  }
}

void ButtonMain(){
  ButtonProgram();
  ButtonChannel();
  //buttonState();
}

void BatteryMain(){
  BatteryStatus();
  BatteryProsent();
}

void PotensiometerMain(){
  int PotensioMeter1 = Pot1();
  int PotensioMeter2 = Pot2();
  PotensioMeters(PotensioMeter1, PotensioMeter2);
}

//ButtonProgram will read buttons used for program switching. A 500 ms delay time is set to stop unit from sending a lot of messages to the computer. This delay can be adjusted if prosess works to slow.
void ButtonProgram(){
  unsigned long buttonTime = millis();

  if(Button5.read() == true || Button6.read() == true){           //when program buttons are pushed, channels are reset.
        channel = 0;
  } 
    
  if(Button5.read() == true  && (buttonTime - lastbuttonTime) > ButtonDelay  ) {
    program += 1;
    lastbuttonTime = buttonTime; 
  }

  if(Button6.read() == true  && (buttonTime - lastbuttonTime) > ButtonDelay  ) {
    program -= 1;
    lastbuttonTime = buttonTime; 
  }

  if(program != abs(program)) {
    program = 0;
  }

  else if(program > 3) {  // For more programs, increase these values.
    program = 3;
  }
}

//ButtonChannel will read buttons used for channel switching. A 500 ms delay time is set to stop unit from sending a lot of messages to the computer. This delay can be adjusted if prosess works to slow.
void ButtonChannel(){
  unsigned long buttonTime = millis();
  
  if(Button1.read() == true  && (buttonTime - lastbuttonTime) > ButtonDelay  ) {
    channel = 1;
    BLEMidiServer.programChange(program, channel);
    lastChannel = channel;
    lastbuttonTime = buttonTime; 
  }

  if(Button2.read() == true  && (buttonTime - lastbuttonTime) > ButtonDelay  ) {
    channel = 2;
    BLEMidiServer.programChange(program, channel);
    lastChannel = channel;
    lastbuttonTime = buttonTime; 
  }

  if(Button3.read() == true  && (buttonTime - lastbuttonTime) > ButtonDelay  ) {
    channel = 3;
    BLEMidiServer.programChange(program, channel);
    lastChannel = channel;
    lastbuttonTime = buttonTime; 
  }

  if(Button4.read() == true  && (buttonTime - lastbuttonTime) > ButtonDelay  ) {
    channel = 4;
    BLEMidiServer.programChange(program, channel);
    lastChannel = channel;
    lastbuttonTime = buttonTime; 
  } 
}

/*
 

//For connection with led indication lamps 
int buttonState(){
    
    switch(program){
        case 1:
            switch(channel){
              case 1:
              interval = 500;
              break;
              
              case 2:
              interval = 1000;
              break;
              
              case 3:
              interval = 1500;
              break;
              
              case 4:
              interval = 2000;
              break;
          
              default:
              digitalWrite(32, LOW);
              break;
            } 
        break;
        
        case 2:
            switch(channel){
              case 1:
              interval = 500;
              break;
              
              case 2:
              interval = 1000;
              break;
              
              case 3:
              interval = 1500;
              break;
              
              case 4:
              interval = 2000;
              break;
          
              default:
              digitalWrite(32, LOW);
              break;
            }   
        break;

        case 3:
            switch(channel){
              case 1:
              interval = 500;
              break;
              
              case 2:
              interval = 1000;
              break;
              
              case 3:
              interval = 1500;
              break;
              
              case 4:
              interval = 2000;
              break;
          
              default:
              digitalWrite(32, LOW);
              break;
            }   
        break;
    }
 
    if ((millis() - previousMillis) >= interval && channel != 0) {
        previousMillis = millis();
        if (ledState == LOW) {ledState = HIGH; } 
        else { ledState = LOW; }
        digitalWrite(32, ledState);
    }

    
  
}
*/


//BatteryStatus calculates batteryvoltage. This calculation is done by adding som fixed reference values, and including expected values from voltagedivider.
// Litiumbatteries should not be drained below 3 volt, else the battery might be permanent damaged.
void BatteryStatus() {
  int vref = 1100;
  float batValue = analogRead(BatvoltagePin);

    batVoltage = ((float)batValue/4095.0)*2.0*3.3*(vref/1000.0);
    //float batvoltage = map(batValue, 2620, 1890, 4.4, 3.3);
}

//BatteryProsent calculates batteryprosent from the batteryvoltage.
int BatteryProsent() {
  //float batvoltage = BatteryStatus();
  int batProsent = map(batVoltage, 4.4, 3.3, 100, 0);
  return batProsent;
}

//PotensioMeters sends analog values over bluetooth. For not sending a lot of messages, values will only be sent when a new value differce from last time.
// A difference value is set because of potensiometers precision. This value can be tuned if necessary.
void PotensioMeters(int PotensioMeter1, int PotensioMeter2) {
  
  
  if(lastChannel != 0) {                //When channel is switched, potentiometers will not change value on this channel.
    PotensioMeter1 = LastPot1;
    PotensioMeter2 = LastPot2;
    lastChannel = 0;  
  }
  unsigned long tid = millis();
  if(tid - potTime > 50) {
    if(PotensioMeter1 != LastPot1 && abs(PotensioMeter1 - LastPot1) > 1.8) {
      BLEMidiServer.controlChange(program, cc[0], PotensioMeter1);
      LastPot1 = PotensioMeter1;
    } 

    if(PotensioMeter2 != LastPot2 && abs(PotensioMeter2 - LastPot2) > 1.8) {
      BLEMidiServer.controlChange(program, cc[1], PotensioMeter2);
      LastPot2 = PotensioMeter2;
    }

    potTime = tid;
  }
}

//Pot1 & Pot2 reads value from potentiometers and give them the right values on a scale between 0 and 127.
int Pot1(){
  int Pot1 = analogRead(Potpin1);
  Serial.print(Pot1);
  int PotensioMeter1 = map(Pot1, 4095, 0, 127, 0);
  return PotensioMeter1; 
}

int Pot2(){
  int Pot2 = analogRead(Potpin2);
  int PotensioMeter2 = map(Pot2, 4095, 0, 127, 0);
  return PotensioMeter2; 
}

//ScreenShow choose what should be displayed. A delay time is set for how often the screen should be updated. If bluetooth is connected "BT" will show up in the left corner of the screen, else it will say not connected.
//Screen will show startup menu when program = 0. If program is not zero, screen will show "channel", "program" and "potentiometer" values.
void screenShow(){
  unsigned long screenTime = millis();                 //reset display
    
    if(BLEMidiServer.isConnected()){
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(2, 1);
      display.println("BT");
      screenPandC();
      //screenBat();
      screenPot();

    }
    else {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(2, 0);
      display.println("BT not connected");
    }
 /*   
     if((screenTime - LastscreenTime) > 1000) {
        display.display();      
      }
    
    if(menu == true) {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,16);
      display.println("Program UP \n to start");  
    }
*/
    screenBat();
    display.display();                            //runs display
  
}

// screenPot displays potentiometer values if a channel is chosen. 
void screenPot(){
    int p1 = Pot1();
    int p2 = Pot2();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(3, 50);
    display.print("P1: ");
    display.setCursor(87, 50);
    display.print("P2: ");
    
    if(channel != 0) {
      display.setCursor(25, 50);
      display.print(map(p1, 127, 0, 100, 0));
      display.setCursor(107, 50);
      display.print(map(p2, 127, 0, 100, 0));
    }
}

//screenPandC displays channel og program
void screenPandC(){
    display.setTextSize(2.5);
    display.setTextColor(WHITE);
    display.setCursor(10, 25);
    display.println("P: ");
    display.setCursor(35, 25);
    display.print(program + 1);
    display.setCursor(70, 25);
    display.println("CH: ");
    display.setCursor(105, 25);
    display.print(channel);
}


//screenBat displays batteryvoltage and uses batteryprosent to indicate the remaining batterylevel. Left is used as a variable to fill a drawn rounded rectangle.
void screenBat(){
  int BatProsent = BatteryProsent();

  if(millis() - voltageTime > 10000){
    v = batVoltage;
    voltageTime = millis();
  }
  
  if (BatProsent > 75 && BatProsent <= 100) {
    left = 1;
    }
  
  if (BatProsent > 50 && BatProsent <= 75){
      left = 0.75;
    }
  
  if (BatProsent > 25 && BatProsent <= 50){
      left = 0.50;
    }
    
  if (BatProsent <= 25) {
      left = 0.25;
  }
    
  display.drawRoundRect(110, 2, 15, 6, 2, WHITE);
  display.fillRoundRect(110, 2, 15*left, 6, 2, WHITE);
  
  if(BLEMidiServer.isConnected()){
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(75, 2); 
    display.println(v);
    display.setCursor(100, 2);  
    display.println("v");
  }
  

}

void startup(){
  int menuCount = 0;
  
  while(menuCount <= 2) {
  display.clearDisplay();
  
  if(millis() - menuTime <= 500) { 
  display.drawBitmap(0, 0,  guitar1, 128, 64, 1);
  display.display();
  }

  if(millis() - menuTime > 500 && millis() - menuTime <=1000) { 
  display.drawBitmap(0, 0,  guitar2, 128, 64, 1);
  display.display();
  }
  
  if(millis() - menuTime > 1000 && millis() - menuTime <=1500) { 
  display.drawBitmap(0, 0,  guitar3, 128, 64, 1);
  display.display();
  }

  if(millis() - menuTime > 2000) { 
  menuCount += 1;
  menuTime = millis();
  }
  }
  }
  
