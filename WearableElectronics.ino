/*******************************************************************
 * Wearable Electronics Demo
 * 
 * This code provides functions for controlling a group of single-
 * color LEDs or an RGB LED with either a common annode (+) or
 * common cathode (-).
 * 
 * The default pin selections in this sample code are for
 * a Lilypad USB ATtiny85 board, so if you are using a different
 * board, you will need to make sure the pins selected in the
 * "RGBLED" object initialization are capable of PWM (analogWrite).
 * Because the Lilypad USB ATtiny85 has a limited number of pins,
 * the default pins are selected with the assumption that only 
 * one or the other of the LED group or  RGB LED will be used at
 * once. You will need to uncomment the desired code in the loop().
 * 
 * Default connections:
 *    LED Group - 0, 1, 2, 3, 4
 *    
 *    RGB LED   - R: 0, G: 1, B: 4 
 * 
 * Author: Michael Ploof for TinkerTech
 * Date: 10/25/18
 *******************************************************************/

/* 
 *  You can ignore everything until the comment 
 *  "Setup LED Group" near the bottom of the code 
 */

// This creates a variable "type" that will store 
// red, blue, and green values of a color
typedef struct _color{
  byte r;
  byte g;
  byte b;
}COLOR;

// This function lets us pick a color using a single 0-255 value
COLOR Wheel(byte WheelPos) {
  COLOR ret;
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    ret = COLOR{255 - WheelPos * 3, 0, WheelPos * 3};
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    ret = COLOR{0, WheelPos * 3, 255 - WheelPos * 3};
  }
  else{
    WheelPos -= 170;
    ret = COLOR{WheelPos * 3, 255 - WheelPos * 3, 0};
  }
  return ret; 
}

// This is a "class," which defines what an "LEDGroup" is and how it can be used
class LEDGroup {
  public:
    LEDGroup(int* pins, int count){
      led_count = count;
      led_pin = pins;
      for(int i = 0; i < led_count; i++){
        pinMode(led_pin[i], OUTPUT);
      }
    }

    void twinkle(int duration){
      long start_time = millis();
      while(millis() - start_time < duration){
        int this_led = random(led_count);
        if(this_led < led_count){
          digitalWrite(this_led, !digitalRead(this_led));
        }
        delay(15);
      }
      allOff();
    }

    void chase(int duration, int chase_speed){
      long start_time = millis();
      while(millis() - start_time < duration){
        for(int i = 0; i < led_count; i++){
          digitalWrite(i, HIGH);
          if(i == 0){
            digitalWrite(led_count - 1, LOW);
          }
          else{
            digitalWrite(i - 1, LOW);
          }
          
          delay(chase_speed);
        }
      }
      allOff();
    }

    void flash(int duration, int wait){
      long start_time = millis();
      while(millis() - start_time < duration){
        allOn();
        delay(wait);
        allOff();
        delay(wait);
      }
    }

    void bounce(int duration, int wait){
      long start_time = millis();
      while(millis() - start_time < duration){
        for(int i = 0; i < led_count; i++){
          digitalWrite(i, HIGH);
          if(i == 0){
            digitalWrite(led_count - 1, LOW);
          }
          else{
            digitalWrite(i - 1, LOW);
          }
          delay(wait);
        }
        for(int i = led_count - 2; i >= 1; i--){
          digitalWrite(i, HIGH);
          digitalWrite(i + 1, LOW);
          delay(wait);
        }
        allOff();
      }
      allOff();
    }

    void allOn(){
      for(int i = 0; i < led_count; i++){
        digitalWrite(i, HIGH); 
      }
    }

    void allOff(){
      for(int i = 0; i < led_count; i++){
        digitalWrite(i, LOW); 
      }
    }

  private:
    int led_count;
    int* led_pin;
};

// This is a "class," which defines what an "RGBLED" is and how it can be used
class RGBLED{
  public:
  
    RGBLED(int r, int g, int b, bool is_common_annode){
      common_annode = is_common_annode;
      r_pin = r;
      g_pin = g;
      b_pin = b;
      pinMode(r_pin, OUTPUT);
      pinMode(g_pin, OUTPUT);
      pinMode(b_pin, OUTPUT);
    }

    void setColor(int r, int g, int b){
      color.r = r;
      color.g = g;
      color.b = b;
      // If this is a common annode device, a high analog value
      // actually produces low brightness because it decreses the
      // potential difference between the anode voltage and the 
      // current sink (the PWM pin), so we need to invert the
      // value used for analogWrite().
      if(common_annode){
        r = 255 - r;
        g = 255 - g;
        b = 255 - b;
      }
      analogWrite(r_pin, r);
      analogWrite(g_pin, g);
      analogWrite(b_pin, b);
    }

    void setColor(COLOR new_color){
      setColor(new_color.r, new_color.g, new_color.b);
    }

    void cycle(int duration, int wait){
      long start_time = millis();
      byte i = 0;
      while(millis() - start_time < duration){
        setColor(Wheel(i));
        i++;
        i = i > 255 ? 0 : i;
        delay(wait);
      }
      allOff();
    }

    COLOR getColor(){
      return color;
    }

    void allOff(){
      setColor(0, 0, 0);
    }
  
  private:
    bool common_annode;
    int r_pin;
    int g_pin;
    int b_pin;
    COLOR color;
};

// Setup the LED group
const int LED_COUNT = 5;                        // The number of LEDs in the group
int led_pins[LED_COUNT] = {0, 1, 2, 3, 4};      // Which digital pins they're connected to
LEDGroup leds = LEDGroup(led_pins, LED_COUNT);  // Create the group object using the above parameters

// Setup the RGB LED
/*
 * All pins used for the rgb LED must support PWM. For the Digispark, these are
 * pins 0, 1, and 4
 * 
 * Parameters
 * r_pin - red pin
 * g_pin - green pin
 * b_pin - blue pin
 * is_common_annode - true if there's a shared + pin, false if there's a shared - pin
 */
RGBLED rgb = RGBLED(0, 1, 4, true);

// this function runs once at the beginning of the program
void setup() {

}

/*
 * ~~ Function durations and waits are in milliseconds ~~
 * 
 * LEDGroup Functions
 * twinkle(duration)
 * chase(duration, wait)
 * bounce(duration, wait)
 * flash(duration, wait)
 * 
 * RGB LED Functions
 * setColor(red, green, blue) - each value is from 0-255
 * cycle(duration, wait)
 * allOff() - turns off all leds
 */

// the loop function runs over and over again forever
void loop() {
    leds.twinkle(5000);
    leds.chase(5000, 100);
    leds.bounce(5000, 100);
    leds.flash(5000, 50);

    //rgb.cycle(5000, 10);
}
