#include<avr/pgmspace.h>



volatile char ip[32];
volatile uint16_t adc_data[2];
volatile uint8_t channel;
const float notes[12] PROGMEM = { 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.0, 415.30, 440.0, 466.16, 493.88};

const char web_page_title[] PROGMEM= "<html><head><title>WIFI Home Page</title></head>";
const char web_page_body1[] PROGMEM = "<body><h1>Welcome To WIFI Home Page</h1>";
const char web_page_body2[] PROGMEM = "<p>Current Room Temperature is: ";
const char web_page_body3[] PROGMEM = "<p>Potentiometer Reading: ";
const char web_page_body4[] PROGMEM = "<p>My Ip address is : ";
const char web_page_end[] PROGMEM = "</body></html>";
