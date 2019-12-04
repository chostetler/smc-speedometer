#include <SevenSeg.h>

// Choose the anode pins for the digits
const int num_of_digits = 3;
int digit_pins[num_of_digits] = {10, 6, 5};

// Initialize the seven segment display
SevenSeg disp(9, 12, 3, 8, 7, 2, 11);

int hall_effect_pin = 0;

// Useful variables for counting rotations. Revolutions is volatile because it will be changed 
// by the interrupt function
unsigned long start_time = 0;
unsigned long delta_t = 20000;
unsigned long current_time = 0;

// Size of the wheel and various helpful constants
// Change wheel_diameter_inches to whatever is needed
float wheel_diameter_inches = 20.0;
float wheel_circumference_inches = wheel_diameter_inches * 3.141592;
float inches_per_mile = 63360.0;

float current_mph = 0.0;

bool triggered = false;

void setup() {
  Serial.begin(9600);
  // Tell the library what common pins to use for digits, and what pin is for the decimal point
  disp.setDigitPins(num_of_digits, digit_pins);
  disp.setDPPin(4);
  disp.setDigitDelay(1000);

  // Use the hall effect sensor to trigger the calculation
  pinMode(hall_effect_pin, INPUT);

  String intro_string = "   SPEEDOTRON 2000   ";
  while (millis() <= (intro_string.length()-2)*200){
    int first_letter_index = round(millis() / 200);
    disp.write(intro_string.substring(first_letter_index, first_letter_index + 3));
  }
}

void loop() {
  current_time = millis();
  bool hall_effect_state = digitalRead(hall_effect_pin);
  if (hall_effect_state == LOW and !triggered){
    triggered = true;
    count_revolution();
  } else if (hall_effect_state == HIGH){
    triggered = false;
  }
  // Update display every second
  current_mph = convert_delta_t_to_mph(delta_t);
  
  //If there is more than 2 seconds with no revolutions, the speed is 0 mph
  if (current_time - start_time > 2000){
    current_mph = 0.0;
  }
  // Write the speed to the 7 segment display
  disp.write(current_mph);
}

void count_revolution(){
  // Whenever the revolution happens, mark down how long it took to turn, and set the new start time
  if (current_time - start_time > 50){
    delta_t = current_time - start_time;
    start_time = current_time;
  }
  Serial.print("Delta t: ");
  Serial.print(delta_t);
  Serial.print(" - MPH: ");
  Serial.println(convert_delta_t_to_mph(delta_t));
}

float convert_delta_t_to_mph(long dt){
  // This takes the time between the revolutions and returns the speed calculated from it
  float delta_t_seconds = dt / 1000.0;
  float in_per_second = wheel_circumference_inches / delta_t_seconds;
  float in_per_hour   = in_per_second * 3600.0;
  float mi_per_hour   = in_per_hour / inches_per_mile;
  float miles_per_hour_rounded = round(100 * mi_per_hour) / 100.0;
  //Serial.println(delta_t);
  return(miles_per_hour_rounded);
}
