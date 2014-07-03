#include <CapacitiveSensor.h>

/*
 * Capacitive Touch Game Controller (prototype)
 * Dylan Dophemont 2014
 * ------------------------------------------------------------
 * This work is licensed under the Creative Commons Attribution
 * 4.0 International License. To view a copy of this license, 
 * visit http://creativecommons.org/licenses/by/4.0/. 
 * ------------------------------------------------------------
 */

// Serial SERIAL_OUTPUT modes
int NO_SERIAL_OUTPUT = 0; // Prints no data
int SENSOR_DATA = 1; // Prints only the sensor data
int BUTTON_PRESS = 2; // Prints only the button presses/values

/*
 * ---------------------- CONFIG - begin ----------------------
 */

// Sensor trigger values
// Determines when a sensor is triggered
long triggerDirectional = 30;
long triggerAction = 30;
long triggerTouchBody = 100;

// Set the serial SERIAL_OUTPUT mode here
int SERIAL_OUTPUT = SENSOR_DATA;

// These values determine how bright the leds will be (min 0, max 255)
int LED_CONTROLLER_ACTIVE = 50;
int LED_CONTROLLER_BEING_HELD = 100;
int LED_CONTROLLER_ACTION = 200;

/*
 * ---------------------- CONFIG - end ----------------------
 */

// Left touch pad
CapacitiveSensor sensUp = CapacitiveSensor(2,3);
CapacitiveSensor sensRight = CapacitiveSensor(2,4);
CapacitiveSensor sensDown = CapacitiveSensor(2,5);
CapacitiveSensor sensLeft = CapacitiveSensor(2,6);

// Right touch pad
CapacitiveSensor sensY = CapacitiveSensor(A0,A3);
CapacitiveSensor sensB = CapacitiveSensor(A0,A4);
CapacitiveSensor sensA = CapacitiveSensor(A0,A2);
CapacitiveSensor sensX = CapacitiveSensor(A0,A1);

// Senses if someone holds the controller
CapacitiveSensor sensTouchBody = CapacitiveSensor(11,12);

// Directional pad state
String UP = "up";
String DOWN = "down";
String LEFT = "left";
String RIGHT = "right";
String NO_DIRECTION = "none";

String direction = NO_DIRECTION;

// Action pad state
String A = "A";
String B = "B";
String X = "X";
String Y = "Y";
String NO_ACTION = "none";

String action = NO_ACTION;

bool bodyIsTouched = false;
bool isCalibrated = false;

int directionLed = 9;
int actionLed = 10;

void setup() {
  Serial.begin(9600); // Open serial connection
  pinMode(directionLed, OUTPUT);
  pinMode(actionLed, OUTPUT);
}


void loop() {
//    long start = millis();

    // The measured capacitance of each sensor
    long cUp = sensUp.capacitiveSensor(30);
    long cRight = sensRight.capacitiveSensor(30);
    long cDown = sensDown.capacitiveSensor(30);
    long cLeft = sensLeft.capacitiveSensor(30);
    
    long cA = sensA.capacitiveSensor(30);
    long cB = sensB.capacitiveSensor(30);
    long cX = sensX.capacitiveSensor(30);
    long cY = sensY.capacitiveSensor(30);
    
    long cTouchBody = sensTouchBody.capacitiveSensor(30);

    // Calibrate the touchpad its sensors when the player holds the controller
    if (cTouchBody > triggerTouchBody) {
      bodyIsTouched = true;
      calibrate();
    } else {
      isCalibrated = false;
      bodyIsTouched = false;
    }

    if (isCalibrated) {
      // Determine direction
      if (cUp > triggerDirectional) {
        direction = UP;
      } else if (cDown > triggerDirectional) {
        direction = DOWN;
      }
      
      if (cRight > triggerDirectional) {
        direction = RIGHT;
      } else if (cLeft > triggerDirectional) {
        direction = LEFT;
      }
      
      if (cUp < triggerDirectional && cDown < triggerDirectional && cLeft < triggerDirectional && cRight < triggerDirectional) direction = NO_DIRECTION;
      
      // Determine action
      if (cA < triggerAction && cB < triggerAction && cX < triggerAction && cY < triggerAction) {
        action = NO_ACTION;
      } else {
        if (cA > triggerAction) {
          action = A;
        }
        if (cB > triggerAction) {
          action = B;
        }
        if (cX > triggerAction) {
          action = X;
        }
        if (cY > triggerAction) {
          action = Y;
        }
      }
    }
    
    // Set the indicator leds
    if (bodyIsTouched && action == NO_ACTION) {
      analogWrite(actionLed, LED_CONTROLLER_BEING_HELD);
    } else {
      analogWrite(actionLed, LED_CONTROLLER_ACTIVE);
    }
    
    if (bodyIsTouched && direction == NO_DIRECTION) {
      analogWrite(directionLed, LED_CONTROLLER_BEING_HELD);
    } else {
      analogWrite(directionLed, LED_CONTROLLER_ACTIVE);
    }

    if (direction != NO_DIRECTION) {
      analogWrite(directionLed, LED_CONTROLLER_ACTION);
    }
    
    if (action != NO_ACTION) {
      analogWrite(actionLed, LED_CONTROLLER_ACTION);
    }

    // Print the serial data       
    if (SERIAL_OUTPUT == SENSOR_DATA) {
//      Serial.print(millis() - start);        // check on performance in milliseconds
      Serial.print("D:");
      Serial.print(cUp);
      Serial.print("\t");
      Serial.print(cDown);
      Serial.print("\t");
      Serial.print(cLeft);
      Serial.print("\t");
      Serial.print(cRight);
      Serial.print("\t");
      Serial.print("A:");
      Serial.print(cA);
      Serial.print("\t");
      Serial.print(cB);
      Serial.print("\t");
      Serial.print(cX);
      Serial.print("\t");
      Serial.print(cY);
      Serial.print("\t");
      Serial.print("L:");
      Serial.print(cTouchBody);
      Serial.print("\t");
//    }
//    
//    if (SERIAL_OUTPUT == BUTTON_PRESS) {
      Serial.print(direction);
      Serial.print("\t");
      Serial.println(action);
    }

    delay(400); // arbitrary delay to limit data to serial port 
}

// Calibrate touch pads
void calibrate() {
  if (!isCalibrated) {
    sensUp.reset_CS_AutoCal();
    sensDown.reset_CS_AutoCal();
    sensLeft.reset_CS_AutoCal();
    sensRight.reset_CS_AutoCal();
    
    sensA.reset_CS_AutoCal();
    sensB.reset_CS_AutoCal();
    sensX.reset_CS_AutoCal();
    sensY.reset_CS_AutoCal();
    
    Serial.println("Sensors calibrated");
    
    isCalibrated = true;
  }
}
