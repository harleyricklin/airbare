/*
 * UdpServer for AirBare Ambient display.
 * Receives OSCMessage on port 1234 /temp with an integer of the temp as an argument
 * Changes the ambient neopixels to reflect the temp
*/
#include <Adafruit_NeoPixel.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    
#include <OSCBundle.h>
#include <OSCBoards.h>

EthernetUDP Udp;

#define PIN_3 12
#define PIN_4 11
#define PIN_6 44
#define PIN_7 42

#define LEDS 144
#define STRIPS 4

// the Arduino's Mac Address
// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] = {  
  0x90, 0xA2, 0xDA, 0x0F, 0x2D, 0x20 }; 

// the Arduino's IP
IPAddress ip(10, 0, 0, 100);

// port number to recieve OSC
const unsigned int inPort = 1234;  


// led strips in display
Adafruit_NeoPixel strip[] = {
      Adafruit_NeoPixel(LEDS, PIN_3, NEO_GRB + NEO_KHZ800),
      Adafruit_NeoPixel(LEDS, PIN_4, NEO_GRB + NEO_KHZ800),
      Adafruit_NeoPixel(LEDS, PIN_6, NEO_GRB + NEO_KHZ800),
      Adafruit_NeoPixel(LEDS, PIN_7, NEO_GRB + NEO_KHZ800)
    };

// color array from coldest to warmest 1-90 degrees    
uint32_t tempcolors[] = {
      strip[0].Color(5,0,255),  
      strip[0].Color(4,0,255),
      strip[0].Color(3,0,255),
      strip[0].Color(2,0,255),
      strip[0].Color(1,0,255),
      strip[0].Color(0,0,255),
      strip[0].Color(0,2,255),
      strip[0].Color(0,18,255),
      strip[0].Color(0,34,255),
      strip[0].Color(0,50,255),
      strip[0].Color(0,68,255),
      strip[0].Color(0,84,255),
      strip[0].Color(0,100,255),
      strip[0].Color(0,116,255),
      strip[0].Color(0,132,255),
      strip[0].Color(0,148,255),
      strip[0].Color(0,164,255),
      strip[0].Color(0,180,255),
      strip[0].Color(0,196,255),
      strip[0].Color(0,212,255),
      strip[0].Color(0,228,255),
      strip[0].Color(0,255,244),
      strip[0].Color(0,255,208),
      strip[0].Color(0,255,168),
      strip[0].Color(0,255,131),
      strip[0].Color(0,255,92),
      strip[0].Color(0,255,54),
      strip[0].Color(0,255,16),
      strip[0].Color(23,255,0),
      strip[0].Color(62,255,0),
      strip[0].Color(101,255,0),
      strip[0].Color(138,255,0),
      strip[0].Color(176,255,0),
      strip[0].Color(215,255,0),
      strip[0].Color(253,255,0),
      strip[0].Color(255,250,0),
      strip[0].Color(255,240,0),
      strip[0].Color(255,230,0),
      strip[0].Color(255,220,0),
      strip[0].Color(255,210,0),
      strip[0].Color(255,200,0),
      strip[0].Color(255,190,0),
      strip[0].Color(255,180,0),
      strip[0].Color(255,170,0),
      strip[0].Color(255,160,0),
      strip[0].Color(255,150,0),
      strip[0].Color(255,140,0),
      strip[0].Color(255,130,0),
      strip[0].Color(255,120,0),
      strip[0].Color(255,110,0),
      strip[0].Color(255,100,0),
      strip[0].Color(255,90,0),
      strip[0].Color(255,80,0),
      strip[0].Color(255,70,0),
      strip[0].Color(255,60,0),
      strip[0].Color(255,50,0),
      strip[0].Color(255,40,0),
      strip[0].Color(255,30,0),
      strip[0].Color(255,20,0),
      strip[0].Color(255,10,0),
      strip[0].Color(255,0,0),
      strip[0].Color(255,0,16),
      strip[0].Color(255,0,32),
      strip[0].Color(255,0,48),
      strip[0].Color(255,0,64),
      strip[0].Color(255,0,80),
      strip[0].Color(255,0,96),
      strip[0].Color(255,0,112),
      strip[0].Color(255,0,128),
      strip[0].Color(255,0,144),
      strip[0].Color(255,0,160),
      strip[0].Color(255,0,176),
      strip[0].Color(255,0,192),
      strip[0].Color(255,0,208),
      strip[0].Color(255,0,224),
      strip[0].Color(255,0,240),
      strip[0].Color(255,1,240),
      strip[0].Color(255,2,240),
      strip[0].Color(255,3,240),
      strip[0].Color(255,4,240),
      strip[0].Color(255,5,240),
      strip[0].Color(255,6,240),
      strip[0].Color(255,7,240),
      strip[0].Color(255,8,240),
      strip[0].Color(255,9,240),
      strip[0].Color(255,10,240),
      strip[0].Color(255,11,240),
      strip[0].Color(255,12,240),
      strip[0].Color(255,13,240),
      strip[0].Color(255,14,240)
    };

////////  START functions

// function to change pixel colors in ambient display as a result of an OSC call
void showPixel(OSCMessage &msg, int addrOffset ){

  int temperature = msg.getInt(0);

  Serial.println("show pixel");  
  Serial.print("temperature : ");
  Serial.println(temperature);

  if (temperature > 89) {
    temperature = 89;
  } else if (temperature < 0) {
    temperature = 0;
  }
  
  allStrips(tempcolors[temperature]);
}


// change all the leds on all the strips to the same color
void allStrips(uint32_t color) {
  for (int i=0; i<STRIPS; i++) {
    setPixels( i, color );
  }

}


// turn off all the leds in all strips 
void clearStrips(){
   allStrips(strip[i].Color(0, 0, 0));
}


// test out the strips
void initializeStrips(){
  Serial.println("initializeStrips : START");
  for (int i=0; i<STRIPS; i++) {
    initializeStrip(i);
    Serial.println(i);
  }
  delay(1500);
  clearStrips(-1);
  Serial.println("initializeStrips : DONE");
}


// set the pixels on strip s to color
void setPixels( int s, uint32_t color ) {
   int on = LEDS;
   
   // turn on
   for (int i=0; i< on; i++) {
     // only turn on 50% of the LEDs, for high density led strips (adafruit 144 pixels/meter neopixel strip)
     if (i % 2 ==0 ){
      strip[s].setPixelColor(i, color);
     }
   }

  strip[s].show(); 

}


// set strip s to blue
void initializeStrip(int s){
  strip[s].begin();
  setPixels(s, strip[s].Color(0, 0, 255));
}


/////////////////// END functions

void setup() {
  Serial.begin(9600);
  //setup ethernet part
  Ethernet.begin(mac,ip);
  Udp.begin(inPort);
  clearStrips(0);
  initializeStrips();
}


void loop(){ 
  OSCMessage messageIN;
  int size;

  if( (size = Udp.parsePacket())>0)  {
    while(size--){
      char buffer = Udp.read();
      Serial.print(buffer);
      messageIN.fill((uint8_t)buffer);
    }

    if(!messageIN.hasError()) messageIN.route("/temp", showPixel);
  }

}



