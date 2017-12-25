#include <Adafruit_NeoPixel.h>

// Pins
const int BUTTON_PIN_0 = 0;    // Pin for button 0. Has to be an interrupt pin 
const int BUTTON_PIN_1 = 1;    // Pin for button 1. Has to be an interrupt pin
const int BUTTON_PIN_2 = 2;    // Pin for button 2. Has to be an interrupt pin

const int NEOPIXEL_PIN = 12; // neopixel pin

int buttonRead = HIGH;           // the current reading from the input pin
int buttonState;           // the current state of the button (after debouncing)
int lastButtonState = HIGH;       // the previous statue of the button

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Neopixel configuration
const int NEOPIXEL_BRIGHTNESS = 50;
const int NEOPIXEL_COUNT = 3; 

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
  Serial.begin(9600);
  
  // initialize the pushbutton pin as an input, and turning up the internal 
  // pullup pin
  pinMode(BUTTON_PIN_0,  INPUT_PULLUP);

  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_0), pin_ISR_0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1), pin_ISR_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_2), pin_ISR_2, CHANGE);

  // Init neopixels
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // reading
  buttonRead = digitalRead(BUTTON_PIN_0);
  
  // If the switch changed, due to noise or pressing:
  if (buttonRead != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (buttonRead != buttonState) {
      buttonState = buttonRead;
      if (buttonState) {
        Serial.write("Released\n");
      } else {
        Serial.write("Pressed\n");
      }
    }
  }

  strip.setBrightness(NEOPIXEL_BRIGHTNESS);
  if (buttonState) {
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.show();
  } else {
    strip.setPixelColor(0, strip.Color(0, 255, 0));
    strip.show();
  }

  
  lastButtonState = buttonRead;

}

// Interrupt functions
void pin_ISR_0() {
 }

void pin_ISR_1() {
}

void pin_ISR_2() {
}
