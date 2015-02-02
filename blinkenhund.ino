#include <Adafruit_NeoPixel.h>
#include <LSM303.h>
#include <Wire.h>
LSM303 compass;

#define PIN 8
//tmp note
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, PIN, NEO_RGB + NEO_KHZ800);

uint8_t low=40;
uint8_t high=100;

//direction varable
int8_t dir=1;



//color in percent
uint8_t red=10;
uint8_t green=100;
uint8_t blue=40;


uint32_t prevMillis=0;

uint16_t intervall=30;


uint8_t bright=low;
uint32_t color=0;

int16_t prevmeanaccel=1000;
int16_t prevmeanhead=1000;
//char report[80];



//generates rgb percents from heading and brightness
uint32_t headingToColor(uint16_t h,uint8_t b){

  uint8_t red=0;
  uint8_t green=0;
  uint8_t blue=0;
  if(h<90){
    red=100;
    green=100 - (h*11/10);
    blue=green;

  }
  else if (h<180){
    h=h-90;
    red=100 - (h*11/10);
    green=0;
    blue=h*11/10;
  }
  else if (h<270){
    h=h-180;
    red=0;
    green=h*11/10;
    blue=100 - (h*11/10);
  }
  else {
    h=h-270.0;
    red=h*11/10;
    green=100;
    blue=h*11/10;
  }
  uint32_t gabs= (uint32_t)(b*red/100) << 16 ;
  uint16_t rabs= (uint16_t)(b*green/100) << 8;
  uint8_t babs = (uint8_t)(b*blue/100) <<0;
  return (uint32_t) gabs+rabs+babs;
}





void setup() {
  strip.begin();
  Wire.begin();
  compass.init();
  compass.enableDefault();
  strip.show(); // Initialize all pixels to 'off'
  compass.m_min = (LSM303::vector<int16_t>){ 
    -2261,  -2222,  -5158    };
  compass.m_max = (LSM303::vector<int16_t>){ 
    +2945,  +2809,  +2326    };
  compass.read();

  Serial.begin(9600);   
  Serial.println("Start");
}


void loop() {


  uint32_t curMillis=millis();


  if( curMillis - prevMillis > intervall){
    prevMillis=curMillis;
    compass.read();
    uint16_t head=(uint16_t)compass.heading();
    int16_t accelz=(compass.a.z >> 4);
    int16_t meanaccel=(accelz+prevmeanaccel)/2;
    int16_t meanhead=(heading+prevmeanhead)/2;
    prevmeanhead=meanhead;
    prevmeanaccel=meanaccel;


    if (bright>=high){
      uint8_t dif = bright-high;
      if (dif>10){
        dir=-(dif>>2);
      }
      else{
        dir=-2;
      }
    }
    else if (bright <=low ){
      dir=1;
    }

    bright+=dir;

    if ((accelz-meanaccel)< -150){
      bright=255;
    }

    color=headingToColor(head,bright);

    for (uint8_t i = 0; i < strip.numPixels(); i++){
      strip.setPixelColor(i,color);

    }
    strip.show();

  } 

}





