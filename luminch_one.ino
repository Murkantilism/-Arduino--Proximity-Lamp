//  Luminch One - Copyright 2012 by Francisco Castro <http://fran.cc>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#define LED_PIN 3
#define SENSOR_PIN A0

#define SENSE_THRESHOLD 150
#define TRACK_THRESHOLD 230
#define DEBOUNCE_CYCLES 30
#define START_TRACKING_CYCLES 180
#define END_TRACKING_CYCLES 210
#define HAND_MINIMUM_CHANGE 15

int adc_input = 0;
int pwm_output = 0;
int stored_bright = 0xFF;
int target_bright = 0;
int hand_tracked_bright = 0;

boolean lamp_lighted = false;
boolean hand_tracking = false;

unsigned char hand_cycles = 0;
unsigned char debounce_cycles = 0;

int sample_1 = 0;
int sample_2 = 0;
int sample_3 = 0;
int sample_4 = 0;


void setup()  {
  analogWrite(LED_PIN, 0);
} 

void loop()  {
  sample_1 = analogRead(SENSOR_PIN); delay(1);
  sample_2 = analogRead(SENSOR_PIN); delay(1);
  sample_3 = analogRead(SENSOR_PIN); delay(1);
  sample_4 = analogRead(SENSOR_PIN); delay(1);
  adc_input = (sample_1 + sample_2 + sample_3 + sample_4) >> 2;

  if(debounce_cycles)
    debounce_cycles -= 1;
  else {
    if(hand_tracking) {
      if(adc_input > SENSE_THRESHOLD) {        
        if(adc_input > TRACK_THRESHOLD + 0xFF)
          hand_tracked_bright = 0;
        else if(adc_input < TRACK_THRESHOLD)
          hand_tracked_bright = 0xff;
        else
          hand_tracked_bright = 0xFF - (adc_input - TRACK_THRESHOLD);
        // I think this is where the light is turned on with a default birghtness
        if(abs(target_bright - hand_tracked_bright) > HAND_MINIMUM_CHANGE || !lamp_lighted) {
          target_bright = (target_bright + (hand_tracked_bright > 8 ? hand_tracked_bright : 8)) >> 1;
          lamp_lighted = true;
        }
        hand_cycles = 0;
      }
      else {
        target_bright = pwm_output;
        stored_bright = pwm_output;
        hand_cycles += 1;
        if(hand_cycles == END_TRACKING_CYCLES) {
          hand_tracking = false;
          hand_cycles = 0;
        }
      }      
    }
    else {
      if(adc_input > SENSE_THRESHOLD) {
        hand_cycles += 1;
        if(hand_cycles == START_TRACKING_CYCLES) {
          hand_tracking = true;
          hand_cycles = 0;
        }
      }
      else {
        if(hand_cycles) {
          lamp_lighted = !lamp_lighted;
          target_bright = lamp_lighted ? stored_bright : 0;
          debounce_cycles = DEBOUNCE_CYCLES;
        }
        hand_cycles = 0;
      }
    }
  }
  
  if(pwm_output != target_bright) {
    if(pwm_output > target_bright && pwm_output > 0) --pwm_output;
    if(pwm_output < target_bright && pwm_output < 0xFF) ++pwm_output;
    analogWrite(LED_PIN, pwm_output);
    delay((0xFF - abs(target_bright - pwm_output)) >> 5);
  }
}

