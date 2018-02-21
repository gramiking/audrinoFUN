#include "FastLED.h"
#include <Ethernet.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

//LED SETUP
#define DATA_PIN    8
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
#define NUM_LEDS    100
CRGB leds[NUM_LEDS];

int r = 255;
int g = 255;
int b = 255;
String readString(20);
boolean doorOpen = false;

//SENSOR SETUP
#define SENSOR_PIN 2
int state; // 0 close - 1 open switch

void setup() {
  delay(2000); // 2 second delay for recovery
  Serial.begin(9600);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.show();

  // Door Setup
  pinMode(SENSOR_PIN, INPUT_PULLUP);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  //Ethernet Setup
  byte mac[] = { 0x32, 0xCE, 0xBE, 0xAE, (unsigned char)0xD23, 0x12 };
  //byte mac[] = { 0x32, 0xCE, 0xBE, 0xAE, 0x23, 0x12 };
  IPAddress ip(X, X, X, X); // change the X to your IP. ie: 192.168.1.1
  // initialize the ethernet device
  Ethernet.begin(mac, ip);
  //print out the IP address
  Serial.print("All is good! Your IP = ");
  Serial.println(Ethernet.localIP());

  //Check Sensor Data
  state = digitalRead(SENSOR_PIN);
  //Serial.println(state);
  Serial.print("Current Door Status = ");
  Serial.println(state);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()

{
   //Check Sensor Data
  state = digitalRead(SENSOR_PIN);
  //Serial.println(state);
  //int count = 0;
  
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) {
    gHue++;  //  } slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 8 ) {
      nextPattern();  // change patterns periodically
    }
    EVERY_N_MILLISECONDS( 20 ) { printSensor(); } // Checks open-close variable.
    //EVERY_N_SECONDS ( 1 ) { OpenWhite(r,g,b,255); } // Randomly calling all white LED for testing
  }
  }

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
  void printIPAddress()
  {
    Serial.print("My IP address: ");
    Serial.print(Ethernet.localIP());
    Serial.println();
  }
  void printSensor()
  {
    state = digitalRead(SENSOR_PIN);
    //Serial.println(state);
        if (state == HIGH){
     r = 255;
     g = 255;
     b = 255;
     BargeIN(r,g,b,255);
     delay(5000);
      } else{
      if (doorOpen) {
        doorOpen = false;
      }
    }
  }
  void BargeIN(int r, int g, int b, int bright)
  {
     FastLED.setBrightness(bright);
    fill_solid( leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
    delay(100);
  }

  void nextPattern()
  {
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
  }

  void rainbow()
  {
    // FastLED's built-in rainbow generator
    fill_rainbow( leds, NUM_LEDS, gHue, 7);
  }

  void rainbowWithGlitter()
  {
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
  }

  void addGlitter( fract8 chanceOfGlitter)
  {
    if ( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
  }

  void confetti()
  {
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV( gHue + random8(64), 200, 255);
  }

  void sinelon()
  {
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy( leds, NUM_LEDS, 20);
    int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
    leds[pos] += CHSV( gHue, 255, 192);
  }

  void bpm()
  {
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for ( int i = 0; i < NUM_LEDS; i++) { //9948
      leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
  }

  void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy( leds, NUM_LEDS, 20);
    byte dothue = 0;
    for ( int i = 0; i < 8; i++) {
      leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
      dothue += 32;
    }

  }
