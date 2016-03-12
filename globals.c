#include<avr/pgmspace.h>
#include<avr/eeprom.h>
#include<avr-wifi.h>


char ip[32];
volatile uint16_t adc_data[2];
volatile uint8_t channel;
const float notes[12] PROGMEM = { 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.0, 415.30, 440.0, 466.16, 493.88};

char ssid[MAX_SSID_LEN] EEMEM = "Abhishek";
char passwd[MAX_SSID_LEN] EEMEM = "85128512";
const char web_page_title[] PROGMEM= "<html><head><title>WIFI Home Page</title></head>";
const char web_page_body1[] PROGMEM = "<body><h1>Welcome To WIFI Home Page</h1>";
const char web_page_body2[] PROGMEM = "<p>Current Room Temperature is: ";
const char web_page_body3[] PROGMEM = "<p>Potentiometer Reading: ";
const char web_page_body4[] PROGMEM = "<p>My Ip address is : ";
const char web_page_body5[] PROGMEM = "<form action=\"ap\">SSID:<br><input type=\"text\" name=\"id\" value=\"Mickey\"> <br> Password:<br> <input type=\"text\" name=\"passwd\" value=\"Mouse\">  <br><br>  <input type=\"submit\" value=\"Submit\"></form>";
const char web_page_end[] PROGMEM = "</body></html>";


const char ssid_update[] PROGMEM = "<html><head><title>WIFI Home Page - New SSID password set.</title></head><body><h1>SSID PASSWORD UPDATED</h1><p>Connection will be down. Please join AP \"Safui\" and visit page 192.168.4.1 to know updated ip address</p></body></html>"; 
const char error_page_1[] PROGMEM = "<html><head><title>WIFI Home Page - ERROR</title></head><body><h1>Ooop's Something bad has happened</h1></body></html>";
