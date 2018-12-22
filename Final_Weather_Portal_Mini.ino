/*The software that controls access to the Wunderground site is wonderfully done by Daniel Eichhorn. 
 * The other functions for the weather icons were adapted by me
 * for patterns that would play well on the Neopixels tube structure--feel free to modify or supply your own.
 */
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <JsonListener.h>
#include "WundergroundClient.h"
#define h0                1                  // This is used in the snow calculations
#define NUM_BALLS         3                 //Number or raindrops/swnowflakes
int counterOne=0;
float temp=0;
float george=0;                             // Height of snow or rain
float timerTwo=0;
int snow=0;
int iCon=6;
#define maxLeds 59                           //Number of LEDs in your strip
int numLeds=59;                             //Number of LEDs in your strip--this is also used in the temp calcs
#define PIN D5                               //Pin for the led output on WEMOS
int tell=1;
int sky[100];                                //array that holds clouds in position--can be adjusted for number of LEDS
int hold=0;
bool limiter=true;
//float timer=0;
int spot=0;
int spotPlus=0;
float tCycle[NUM_BALLS];                    //used in rain and snow effects
int pos[NUM_BALLS];
long tLast[NUM_BALLS];
float h[NUM_BALLS];
int COR[NUM_BALLS];
float GRAVITY[NUM_BALLS];
int pMax=0;
float timerFire=0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(59, PIN, NEO_GRB + NEO_KHZ800);  //Usual setup for the Neopixels


/**
 * Wunderground Settings
 */
const String  WUNDERGRROUND_API_KEY = "Your Api key"; //you know the drill--you have to get a key
const boolean IS_METRIC = true;
// to retrieve the ZMW-Code use
// 		http://api.wunderground.com/api/f21087d5adde757d/conditions/q/AK/ANCHORAGE.json
// for example and grab for the zmw ...
const String  WUNDERGROUND_ZMW_CODE = "99501.1.99999"; // Anchorage, Alaska
const String  WUNDERGRROUND_LANGUAGE = "";

// initiate the WundergoundClient
WundergroundClient wunderground(IS_METRIC);
String iconText;
int icon=2;


/**
 * WiFi Settings
 */
const char* ESP_HOST_NAME = "esp-" + ESP.getFlashChipId(); 
const char* WIFI_SSID     = "Name of your network"; //Name of your network
const char* WIFI_PASSWORD = "password of your network"; //password

// initiate the WifiClient
WiFiClient wifiClient;



/**
 * Helping funtions
 */
void connectWifi() {
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(20);
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");
  digitalWrite(D2,HIGH);  //Pin D2 is held HIGH when WiFi is connected--blue one on the board
  Serial.println();
}


/**
 * SETUP
 */
void setup() {
for(int i=0;i<100;i++) sky[i]=0;
Serial.begin(115200);
pinMode(D2,OUTPUT);  //sets up the Wifi pin
digitalWrite(D2,LOW);
connectWifi();
strip.begin();
strip.show(); // Initialize all pixels to 'off'
  for(int i=0; i<NUM_BALLS;i++){    //this is the setup for the falling rain/snow animation
    tLast[i]=millis();
    h[i]=0;
    COR[i]=random(2,12);
    GRAVITY[i]=1.5;    //gravity is a slower thing on this animation--way to fast if you use real value of over 9

}

}

void loop() {

  if (millis()-timerFire> (600 * 1000)) { // Call up weather underground once every 10 minute--does anyone really care about current weather?
    timerFire=millis();
   
    
    Serial.println();
    Serial.println("\n\nNext Loop-Step: " + String(millis()) + ":");

    wunderground.updateConditions(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_ZMW_CODE);
//This is all the cool information you can use if you want--I only used the temp and the current conditions....
    Serial.println("wundergroundHours: " + wunderground.getHours());
    Serial.println("wundergroundMinutes: " + wunderground.getMinutes());
    Serial.println("wundergroundSeconds: " + wunderground.getSeconds());
    Serial.println("wundergroundDate: " + wunderground.getDate());

    Serial.println("wundergroundMoonPctIlum: " + wunderground.getMoonPctIlum());
    Serial.println("wundergroundMoonAge: " + wunderground.getMoonAge());
    Serial.println("wundergroundMoonPhase: " + wunderground.getMoonPhase());
    Serial.println("wundergroundSunriseTime: " + wunderground.getSunriseTime());
    Serial.println("wundergroundSunsetTime: " + wunderground.getSunsetTime());
    Serial.println("wundergroundMoonriseTime: " + wunderground.getMoonriseTime());
    Serial.println("wundergroundMoonsetTime: " + wunderground.getMoonsetTime());
    Serial.println("wundergroundWindSpeed: " + wunderground.getWindSpeed());
    Serial.println("wundergroundWindDir: " + wunderground.getWindDir());
    
    Serial.println("wundergroundCurrentTemp: " + wunderground.getCurrentTemp());
    Serial.println("wundergroundTodayIcon: " + wunderground.getTodayIcon());
    Serial.println("wundergroundTodayIconText: " + wunderground.getTodayIconText());
    Serial.println("wundergroundMeteoconIcon: " + wunderground.getMeteoconIcon(wunderground.getTodayIconText()));
    Serial.println("wundergroundWeatherText: " + wunderground.getWeatherText());
    Serial.println("wundergroundHumidity: " + wunderground.getHumidity());
    Serial.println("wundergroundPressure: " + wunderground.getPressure());
    Serial.println("wundergroundDewPoint: " + wunderground.getDewPoint());
    Serial.println("wundergroundPrecipitationToday: " + wunderground.getPrecipitationToday());

    Serial.println();
    Serial.println("---------------------------------------------------/\n");
    String iconTemp=wunderground.getCurrentTemp();
    float temp=iconTemp.toInt();
    temp=temp*1.8+32; //figure out the temp
    iconText= wunderground.getTodayIconText(); //I got lazy and only wanted 6 icons...you can do more if you want
  if (iconText == "chanceflurries") icon=1;
  if (iconText == "chancerain") icon=2;
  if (iconText == "chancesleet") icon=1;
  if (iconText == "chancesnow") icon=1;
  if (iconText == "chancetstorms") icon=6;
  if (iconText == "clear") icon=3;
  if (iconText == "cloudy") icon=5;
  if (iconText == "flurries") icon=1;
  if (iconText == "fog") icon=5;
  if (iconText == "hazy") icon=5;
  if (iconText == "mostlycloudy") icon=5;
  if (iconText == "mostlysunny") icon=3;
  if (iconText == "partlycloudy") icon=4;
  if (iconText == "partlysunny") icon=4;
  if (iconText == "sleet") icon=1;
  if (iconText == "rain") icon=2;
  if (iconText == "snow") icon=1;
  if (iconText == "sunny") icon=3;
  if (iconText == "tstorms") icon=6;
  Serial.print("temp");
  Serial.println(temp);
  Serial.print("icon");
  Serial.println(icon);
  timerTwo=millis();
  numLeds=map(temp,0,70,maxLeds,5);  //mapped the temp to height in pixels reversed--smaller flame for higher temp--its Alaska....
  numLeds=constrain(numLeds,5,maxLeds);
  iCon=icon;
  }
  //timer circuits for switching back between weather and temp every 30 seconds....
 for(int i=0;i<59;i++)strip.setPixelColor(i,0,0,0);  
 if((millis()-timerTwo>(30*1000))&&(icon==6)){
  icon=iCon;  
  timerTwo=millis();
 }
 else if((millis()-timerTwo>(30*1000))&&(icon==iCon)){
  icon=6;
  timerTwo=millis(); 
 }
 


 //calling the functions to do the icons
  switch(icon){
    case 1:
    snowstorm();
    break;
    case 2:
    rainstorm();
    break;
    case 3:
    rainbowCycle(20);
    break;
    case 4:
    partlycloudy();
    break;
    case 5:
    reallycloudy();
    break;
    case 6:
    Fire(55,120,15,numLeds);
    break;
  }
  }
//Function for clear skies
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
//Functions for temperature 
  void Fire(int Cooling, int Sparking, int SpeedDelay,int NUM_LEDS) {
  static byte heat[96];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}
  void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
//function for partly cloudy
void partlycloudy(){
   float timerClock=millis();
   spot=random(2,48);
   spotPlus=random(6,15);
  for(int i=spot;i<spot+spotPlus;i++)sky[i]=1;
  
  //Serial.println("random");
  limiter=true;
  while(limiter){
for(int i=maxLeds;i>0;i=i-1){
  hold=sky[i-1];
  sky[i]=hold;
}
sky[0]=0;
for(int i=0;i<maxLeds;i++)strip.setPixelColor(i,0,0,255);
    for(int j=0;j<maxLeds;j++){
      //Serial.println(sky[j]);
      if(sky[j]==1){
        
        strip.setPixelColor(j,155,155,155);
        //strip.show();
      }
      else {
        
         // strip.setPixelColor(j,0,0,255);
          //strip.show();
       
        
        delay(1);
      }
  }
  strip.show();
  for(int i=0;i<maxLeds;i++)strip.setPixelColor(i,0,0,0);
//delay(20);
 
      if(millis()-timerClock>random(1000,3000))limiter=false;
}

}
//function for full clouds--very common in Alaska
void reallycloudy(){
  
  float timerClock=millis();
   int color=random(1,4);
   spot=random(2,48);
   spotPlus=random(6,15);
  for(int i=spot;i<spot+spotPlus;i++)sky[i]=color;
  
  //Serial.println("random");
  limiter=true;
  while(limiter){
for(int i=maxLeds;i>0;i=i-1){
  hold=sky[i-1];
  sky[i]=hold;
}
sky[0]=0;
for(int i=0;i<maxLeds;i++)strip.setPixelColor(i,i*2,i*2,i*2);
    for(int j=0;j<maxLeds;j++){
      //Serial.println(sky[j]);
      if(sky[j]==1){
        
        strip.setPixelColor(j,155,155,155);
        //strip.show();
      }
      else if(sky[j]==2){
        strip.setPixelColor(j,178,178,178);
      }
      else if(sky[j]==3){
        strip.setPixelColor(j,204,218,229);
      }
        
        
          //strip.setPixelColor(j,12,12,30,12);
          //strip.show();
       
        
        delay(1);
      }
  
  strip.show();
  for(int i=0;i<maxLeds;i++)strip.setPixelColor(i,0,0,0);
//delay(20);
 
      if(millis()-timerClock>random(300,1000))limiter=false;
}
}
//function for rainstorm--you can adjust the number of drips being patterned and the height of the rainwater before emptying 
void rainstorm(){
  for(int i=0;i<NUM_BALLS;i++){
  tCycle[i]=millis()-tLast[i];
  h[i]=0.5*GRAVITY[i]*pow(tCycle[i]/1000,2.0);
  //Serial.println(h[i]);
  pos[i]=(maxLeds-(round(h[i]*maxLeds)))-COR[i];
  //Serial.println(pos[i]);
  if (pos[i]<pMax) {
    tLast[i]=millis();
    george=george+1;
    pMax=round(george/10);
  }
  }
  for(int i=0;i<maxLeds;i++)strip.setPixelColor(i,0,25,12);
  for(int i=0;i<pMax;i++)strip.setPixelColor(i,0,255,i*4);
  for(int i=0;i<NUM_BALLS;i++){
    strip.setPixelColor(pos[i],0,255,0);
    strip.setPixelColor(pos[i]+1,255,0,0);
    
  }
  strip.show();
  for (int i = 0 ; i < NUM_BALLS ; i++) {
    strip.setPixelColor(pos[i],0,0,0);
    strip.setPixelColor(pos[i]+1,0,0,0);
  }
 if(pMax>40)george=0;
}
//function for snow--not as common as you would think up here--you can adjust flakes and speed of falling
void snowstorm(){
  for(int i=0;i<NUM_BALLS;i++){
  tCycle[i]=millis()-tLast[i];
  h[i]=0.5*GRAVITY[i]*pow(tCycle[i]/1000,2.0);
  //Serial.println(h[i]);
  pos[i]=(maxLeds-(round(h[i]*maxLeds)))-COR[i];
  //Serial.println(pos[i]);
  if (pos[i]<pMax) {
    tLast[i]=millis();
    george=george+1;
    pMax=round(george/10);
  }
  }
  for(int i=0;i<maxLeds;i++)strip.setPixelColor(i,0,0,50);
  for(int i=0;i<pMax;i++)strip.setPixelColor(i,i*5,i*5,i*5);
  for(int i=0;i<NUM_BALLS;i++){
    strip.setPixelColor(pos[i],255,255,255);
    strip.setPixelColor(pos[i]+1,0,0,200);
    
  }
  strip.show();
  for (int i = 0 ; i < NUM_BALLS ; i++) {
    strip.setPixelColor(pos[i],0,0,0);
    strip.setPixelColor(pos[i]+1,0,0,0);
  }
 if(pMax>35)george=0;
  
 
}

  


  

  
  
 


