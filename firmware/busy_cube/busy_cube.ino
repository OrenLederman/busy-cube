#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Accel (hardware SPI)
const int8_t LIS3DH_CS = 0;

// hardware SPI
Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);
const int ACCEL_INT_PIN = 1;        // Pin for accelerometer interrupt. Must be an INT pin
#define CLICKTHRESHHOLD 80

// Pins
const int BUTTON_PIN_0 = 7;         // Pin for button 0. Has to be an interrupt pin 
const int BUTTON_PIN_1 = 2;         // Pin for button 1. Has to be an interrupt pin
const int BUTTON_PIN_2 = 3;         // Pin for button 2. Has to be an interrupt pin
const int NEOPIXEL_PIN = 12;        // neopixel pin
const int BUZZER_PIN = 5;        // buzzer

// buttons defs
const int NUM_BUTTONS = 3;          // Number of buttons
int button_pins[] = {BUTTON_PIN_0, BUTTON_PIN_1, BUTTON_PIN_2};

// Neopixel configuration
const int NEOPIXEL_BRIGHTNESS = 100;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_BUTTONS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  //while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  Serial.begin(9600);

  // Init colors
  init_button_color_numbers();
 
  // initialize the pushbutton pin as an input, and turning up the internal 
  // pullup pin
  for (int i=0; i < NUM_BUTTONS ;i++) {
    pinMode(button_pins[i],  INPUT_PULLUP);
  }

  pinMode(LED_BUILTIN,  OUTPUT);
  
  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_0), pin_ISR_0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1), pin_ISR_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_2), pin_ISR_2, CHANGE);

  // Init neopixels
  strip.begin();
  for (int i=0; i < NUM_BUTTONS; i++) {
      strip.setPixelColor(i, 0);
  }  
  strip.show();

  // Acceleromoter settings
  pinMode(ACCEL_INT_PIN,  INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ACCEL_INT_PIN), pin_ISR_ACCEL, CHANGE);
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("LIS3DH Couldnt start");
    //while (1);
  } else {
    Serial.println("LIS3DH found!");
  }
  
  lis.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G!
  
  Serial.print("Range = "); Serial.print(2 << lis.getRange());  
  Serial.println("G");

  // 0 = turn off click detection & interrupt
  // 1 = single click only interrupt output
  // 2 = double click only interrupt output, detect single click
  // Adjust threshhold, higher numbers are less sensitive
  lis.setClick(2, CLICKTHRESHHOLD);

  // Seem to need a delay here, otherwise, the neopixel gets turned on. Not sure why
  delay(2000);

  Serial.println("Ready");
  Serial.println(digitalPinToInterrupt(BUTTON_PIN_0));

  // Beep!
  /*
  int noteDuration = 1000 / 4;
  tone(BUZZER_PIN, 262, noteDuration);
  int pauseBetweenNotes = noteDuration * 1.30;
  delay(pauseBetweenNotes);
  // stop the tone playing:
  noTone(BUZZER_PIN);
  */
}

void loop() {
  strip.setBrightness(NEOPIXEL_BRIGHTNESS);
  for (int i=0; i < NUM_BUTTONS; i++) {
    if (is_new_press(i)) {
      Serial.println("button! (in loop)");
      bump_button_color(i);
      strip.setPixelColor(i, get_button_color(i));
    }
  }  
  
  strip.show();
}

// ----------------------------------------------------------- //
// Colors and color control
const uint16_t NUM_COLORS = 6;                          // How many colors?
const uint16_t WHEEL_POS[] = {0,85,170,42,127,212};     // List of colors (as Wheel positions..)
  /*
   * 0 - red
   * 85 - green
   * 170 - blue
   * 127 - light blue
   * 212 - purple
   * 42 - yello
   */

uint16_t button_color_numbers[NUM_BUTTONS]; // current color number of every button

void init_button_color_numbers() {
  for (int i=0; i < NUM_BUTTONS; i++) {
    button_color_numbers[i] = -1;
  }
}

uint16_t get_button_color_number(int button_id) {
  return button_color_numbers[button_id];
}

uint32_t get_button_color(int button_id) {
  uint16_t color_number = get_button_color_number(button_id);
  uint16_t wheel_pos = WHEEL_POS[color_number];
  uint32_t  c = Wheel(wheel_pos);
  return(c);

}

/**
 * changes the color of a button to the next color
 */
void bump_button_color(int button_id) {
  button_color_numbers[button_id] = (button_color_numbers[button_id] + 1) % NUM_COLORS;  
}


// Source - Adafruit's example sketches
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
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

// ----------------------------------------------------------- //
// button debounce code

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
const boolean BUTTON_PRESSED = LOW;  // Set to LOW if using internal pullup resistors

typedef struct 
  {
    int button_read = HIGH;     // the current reading from the input pin
    int button_state;           // the current state of the button (after debouncing)
    int last_button_state = HIGH;// the previous statue of the button
    unsigned long last_debounce_time = 0;  // the last time the output pin was toggled    
  } button_state_t;

button_state_t button_states[NUM_BUTTONS]; // buttons debouncing internal states

/**
 * Code for debouncing button
 */
boolean debounce_button(int button_id) {
  // reading
  button_states[button_id].button_read = digitalRead(button_pins[button_id]);
  
  // If the switch changed, due to noise or pressing:
  if (button_states[button_id].button_read != button_states[button_id].last_button_state) {
    // reset the debouncing timer
    button_states[button_id].last_debounce_time = millis();
  }

  if ((millis() - button_states[button_id].last_debounce_time) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (button_states[button_id].button_read != button_states[button_id].button_state) {
      button_states[button_id].button_state = button_states[button_id].button_read;
    }
  }

  button_states[button_id].last_button_state = button_states[button_id].button_read;
  return button_states[button_id].button_state;
}

// ----------------------------------------------------------- //
// button press tracking. I use this code to track when the button 
// was pressed.
boolean buttons_last_status[NUM_BUTTONS] = {false};

/**
 * Checks the current status (pressed or not), returns true
 * if the button is pressed, and it's a new press event
 */
boolean is_new_press(int button_id) {
  boolean is_pressed_now = is_button_pressed(button_id);
  boolean ret_val = false;
  if (is_pressed_now && !buttons_last_status[button_id]) {
    ret_val = true;
  }

  buttons_last_status[button_id] = is_pressed_now;
  return ret_val;
}

/**
 * Is the button in pressed state
 */
boolean is_button_pressed(int button_id) {
  if (debounce_button(button_id) == BUTTON_PRESSED) {
    return true;
  } else {
    return false;
  }
}

// ----------------------------------------------------------- //
// Interrupt functions
void pin_ISR_ACCEL() {
  Serial.println("accel int!");
 }


void pin_ISR_0() {
  Serial.println("button0 int!");
 }

void pin_ISR_1() {
  Serial.println("button1 int!");
}

void pin_ISR_2() {
  Serial.println("button2 int!");
}
