

float threshold_triceps = 4; // [V]
float threshold_biceps = 1; // [V]
float interogation_window = 20000; // [ms]
bool enable_move = true; // We initiate a switch to know if we move or not
bool motor_off = true; // Variable to know if the motor is powering
unsigned long time;
unsigned long first_detection = -1;
bool GO_state;

//const int Not_Moving = 0;
//const int MovingUp = 1;
//const int MovingDown = 2;

char Status = 'Not_Moving';
//int enum Status
//{
//  Not_Moving,  // 0
//  MovingUp,
//  MovingDown
//  }


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

/////////////// TO DO BEFORE VELOCITY CONTROL
// Use position control to set the position of the orthosis fully open or fully close

void loop() {
  Serial.print('\n');
  Serial.print("____ New sample_____");
  time = millis();
  int triceps_EGM = analogRead(A0);
  int biceps_EGM = 0;
  // int biceps_EGM = analogRead(A1);


  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage_triceps = triceps_EGM * (5.0 / 1023.0);
  float voltage_biceps = biceps_EGM * (5.0 / 1023.0);

  // Data display and logger
  // Serial.println(voltage_triceps);

  //  Serial.print(voltage_triceps);
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
    //////////////////////////// threshold trigering ////////////////////////////////////

    // TWO TRIGGERS ARE ACTIVE
    if (voltage_triceps >= threshold_triceps && voltage_biceps >= threshold_biceps)
    {
      first_detection = -1;
      Serial.print("The two signals are above the threshold, I don't know what to do !");
      Status = 'Not_Moving';
      // TO DO Something
    }
    
    // ONE TRIGGER IS ACTIVED
    else if (voltage_triceps >= threshold_triceps)
    {
      if (first_detection < 0) {
        first_detection = time;
      };
      GO_state = counter_state(first_detection, interogation_window, time);
      if (voltage_triceps >= threshold_triceps)
      {
        Serial.print("You're going down my n#&'a!");
        Status = 'GO_DOWN';
      }
    }
    
    //////////////////////////
    else if (voltage_biceps >= threshold_triceps)
    {
      Serial.print("I am going all the way up !");
      Status = 'GO_UP';
    }

    // NOTHING ACTIVE
    else
    {
      Serial.print("No signal to make me move ... yet ;)");
      Status = 'Not_Moving';
    }
  }

  delay(1000);

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

