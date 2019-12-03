#include <stdarg.h>
#include <SevenSeg.h>

// Give names to the pins to be used for each segment
int a_pin = 9;
int b_pin = 12;
int c_pin = 3;
int d_pin = 8;
int e_pin = 7;
int f_pin = 2;
int g_pin = 11;
int decimal_pin = 4;

// And the digit pins
const int num_of_digits = 3;
int digit_1 = 10;
int digit_2 = 6;
int digit_3 = 5;
int digit_pins[num_of_digits] = {digit_1, digit_2, digit_3};

int hall_effect_pin = 0;

// Initialize the seven segment display
SevenSeg disp(a_pin, b_pin, c_pin, d_pin, e_pin, f_pin, g_pin);

// Useful variables for counting rotations. Revolutions is volatile because it will be changed 
// by the interrupt function
volatile long start_time = 0;
volatile int delta_t = 2000000;

// Size of the wheel and various helpful constants
// Change wheel_diameter_inches to whatever is needed
float wheel_diameter_inches = 20.0;
float wheel_circumference_inches = wheel_diameter_inches * 3.141592;
float inches_per_mile = 63360.0;

volatile float current_mph = 0;

void setup() {
  Serial.begin(9600);
  // Tell the library what common pins to use for digits, and what pin is for the decimal point
  disp.setDigitPins(num_of_digits, digit_pins);
  disp.setDPPin(decimal_pin);

  // Set it up so magnet_detected will be called each time the magnet passes by the sensor (pin 0)
  // This runs in the background, so nothing needs to be explicitly put in the loop() function.
  pinMode(hall_effect_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(hall_effect_pin), count_revolution, FALLING); 
}

void loop() {
  //Serial.println(delta_t);
  // Update display every second
  current_mph = convert_delta_t_to_mph(delta_t);
  //Serial.println(millis() - start_time);
  if (millis() - start_time > 2000){
    current_mph = 0.0;
  }
  int ten_seconds = millis()/100;
  float seconds = ten_seconds/10.0;
  disp.write(current_mph);
}

void count_revolution(){
  delta_t = millis() - start_time;
  Serial.print("Revolution! Delta_t:");
  Serial.println(delta_t);
  start_time = millis();
}

float convert_delta_t_to_mph(float delta_t){
  float delta_t_seconds = delta_t / 1000.0;
  float in_per_second = wheel_circumference_inches / delta_t_seconds;
  float in_per_hour   = in_per_second * 3600.0;
  float mi_per_hour   = in_per_hour / inches_per_mile;
  float miles_per_hour_rounded = round(100 * mi_per_hour) / 100.0;
  Serial.println(in_per_second);
  current_mph = miles_per_hour_rounded;
  return(miles_per_hour_rounded);
}
