#include <Adafruit_NeoPixel.h>

const int BUTTON_PIN_0 = 0;    // the number of the pushbutton pin
const int NEOPIXEL_PIN = 12; // neopixel pin

const int NEOPIXEL_BRIGHTNESS = 50;
const int NEOPIXEL_COUNT = 3; 

// button state
bool oldState = HIGH;
volatile int buttonState = 0;         // variable for reading the pushbutton status


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN_0,  INPUT_PULLUP);

  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_0), pin_ISR, CHANGE);

  // Init neopixels
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Nothing here!
  
}

void pin_ISR() {
  buttonState = digitalRead(BUTTON_PIN_0);
  
  strip.setBrightness(NEOPIXEL_BRIGHTNESS);
  if (buttonState) {
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.show();
  } else {
    strip.setPixelColor(0, strip.Color(0, 255, 0));
    strip.show();
  }
  
}
