////////////////////////////////////////////////////////////////////////////
/////////////////// DRIVERS DECLARATION ////////////////////////////////

#include <LucaLibs.h>
#include <mano_shield/mano_shield.cpp>
#include <Servomotors/ServomotorDynamixel.h>

//ns_shields::mano_shield m_shield(ns_shields::SHIELD_REV11);
//ns_shields::mano_shield m_shield(ns_shields::SHIELD_V12);
//ns_shields::mano_shield m_shield(ns_shields::SHIELD_V13);
ns_shields::mano_shield m_shield(ns_shields::SHIELD_V14);
//ns_shields::mano_shield m_shield(ns_shields::SHIELD_V15);

const uint8_t id = 1;
ns_servomotors::ServomotorDynamixel * motor;

//---
uint8_t state = true;
int speed = 0;
int speedfadeAmount = 5;
//---

////////////////////////////////////////////////////////////////////////////
/////////////////// CONTROL GLOBAL VARIABLE ////////////////////////////////
float threshold_triceps_up = 4; // [V]
float threshold_triceps_low = 2; //[V]
float threshold_biceps = 1; // [V]
float interogation_window = 2000; // [ms]
float interogation_window_down = 400;  // [ms]
bool enable_move = true; // We initiate a switch to know if we move or not
//bool motor_off = true; // Variable to know if the motor is powering
unsigned long time;
unsigned long first_detection = 0;
unsigned long last_detection = 0;
bool GO_state = false;
float target_speed;
char Status = 'Not_Moving'; // [GO_UP,GO_DOWN,Not_Moving]

void setup() {
  // init shield
  m_shield.init();

  // setup motor
  motor = new ns_servomotors::ServomotorDynamixel(id, *(m_shield.interface_dynamixel));
  motor->init();
  motor->setTorque(true);

  // set limits
  motor->setJointMode(0, 4095);  // MX

  // set speed
  motor->setSpeed(256);
  delay(500);

  // set positions
  //  motor->setPosition(100);
  //  delay(1000);
  //  motor->setPosition(1023);

  // set max speed
  //motor->setSpeed(1023);

  // set torque
  motor->setTorqueLimit(500);

  // put your setup code here, to run once:
  Serial.begin(9600);
}

/////////////// TO DO BEFORE VELOCITY CONTROL
// Use position control to set the position of the orthosis fully open or fully close

void loop() {
  Serial.print('\n');
  // Serial.print("____ New sample_____");
  time = millis();
  int triceps_EGM = analogRead(A0);
  int biceps_EGM = 0;
  // int biceps_EGM = analogRead(A1);

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage_triceps = triceps_EGM * (5.0 / 1023.0);
  float voltage_biceps = biceps_EGM * (5.0 / 1023.0);

  // Data display and logger
  // Serial.println(voltage_triceps);
  Serial.print('\n');
  Serial.print(voltage_triceps);
  //  Serial.print(",");
  //  Serial.println(-voltage_triceps);
  //  Serial.println(0);
  //  Serial.println(-1);

  if (enable_move == false)
  {
    Serial.print("We don't wanna move myman !");
  }
  else
  {
    if (GO_state == false)
    {
      //////////////////////////// threshold trigering ////////////////////////////////////
      // We are first checking if we are listening to threshold detection

      // TWO TRIGGERS ARE ACTIVE
      // TODO two signals are actually triceps, replace this with function noKnownSignal
      if (voltage_triceps >= threshold_triceps_up && voltage_biceps >= threshold_biceps)
      {
        first_detection = 0;
        Serial.print("The two signals are above the threshold, I don't know what to do !");
        Status = 'Not_Moving';
        // TO DO Something
      }

      // ONE TRIGGER IS ACTIVED
      //TODO replace this with function tricepsSignal
      else if (voltage_triceps >= threshold_triceps_up)
      {
        if (first_detection < 1) {
          first_detection = time;
        };
        GO_state = counter_state(first_detection, interogation_window, time);
        if (GO_state == true) {
          Status = 'GO_DOWN';
          Serial.print("Status GO down");
        };
      }


      //////////////////////////
      else if (voltage_biceps >= threshold_biceps)
      {
        Serial.print("I am going all the way up !");
        Status = 'GO_UP';
      }

      // NOTHING ACTIVE
      else
      {
        first_detection = 0;
        Serial.print("No signal to make me move ... yet ;)");
        Status = 'Not_Moving';
      }
    }

    //////////////////////////// Active phase ////////////////////////////////////
    // We are first checking if we are listening to threshold detection
    if (GO_state == true)
    {
      Serial.print(Status);
      /////////////// Check the status to know which way to control
      // Go down
      if (Status == 'GO_DOWN');
      {
        // Normal condition where we powering the arm down
        target_speed = 1;
        Serial.print("Unlimited Power !");
      }

      // Go Up
      if (Status == 'GO_UP');
      {
        // Normal condition where we powering the arm down
        target_speed = 1;
        Serial.print("Great Responsabilities !");
      }

      /////////////// Check for stops
      if (voltage_triceps < threshold_triceps_low)
      {
        if (last_detection < 1) {
          last_detection = time;
        };
        GO_state = ~ counter_state(last_detection, interogation_window_down, time);
        if (GO_state == false) {
          Status = 'Not_moving';
          Serial.print(Status);
        };

      }
    }
    delay(200);
  }
}
  bool counter_state(unsigned long first_detection, float interogation_window, float time)
  {
    float duration_detection = time - first_detection;
    if (duration_detection >= interogation_window)
    {
      Serial.print('\n');
      Serial.print("TRIGGER");
      Serial.print('\n');
      return true;
    }
    else
      return false;
  }

