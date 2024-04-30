// Header file to contain all of the pins on the Teensy 4.1

#pragma once // Include this file only once

#include <pins_arduino.h> // Include the pins_arduino.h file to get the default Teensy pin definitions


//	*************************************************************************************************
//	Enumerations for Array'd Pins
//	*************************************************************************************************

// The Stepper Motors for the Gantry
typedef enum {
	GANTRY_LEFT_TOP_MOTOR,
	GANTRY_LEFT_BOTM_MOTOR,
	GANTRY_RIGHT_TOP_MOTOR,
	GANTRY_RIGHT_BOTM_MOTOR,
	NUM_MOTORS
} GantryMotor;



// The Servo Motors for the Gantry
typedef enum {
	HOURS_FIRST_DIGIT_SERVO,
	HOURS_SECOND_DIGIT_SERVO,
	MINS_FIRST_DIGIT_SERVO,
	MINS_SECOND_DIGIT_SERVO,
	NUM_SERVOS
} ServoMotors;




//	*************************************************************************************************
//	Pins for the Teensy 4.1
//	*************************************************************************************************


// I2C Pins to communicate with the ESP32-C3
// Pins 18 and 19 are the default I2C pins for the Teensy 4.1, defined in pins_arduino.h
// const uint8_t I2C_SDA = 18; // I2C Serial Data Line
// const uint8_t I2C_SCL = 19; // I2C Serial Clock Line


// Stepper Motor Drivers. Other(shared) SPI pins are defined in pins_arduino.h
const uint8_t StepperDriverCSPins[NUM_MOTORS] = {10, 29, 36, 37}; // Chip Select Pins for the Stepper Drivers



// Servos


// Gantry Limit Switches
typedef enum {
	GANTRY_LEFT_UP_LIMIT_SWITCH,
	GANTRY_LEFT_DOWN_LIMIT_SWITCH,
	GANTRY_LEFT_FW_LIMIT_SWITCH,
	GANTRY_LEFT_BW_LIMIT_SWITCH,
	GANTRY_RIGHT_UP_LIMIT_SWITCH,
	GANTRY_RIGHT_DOWN_LIMIT_SWITCH,
	GANTRY_RIGHT_FW_LIMIT_SWITCH,
	GANTRY_RIGHT_BW_LIMIT_SWITCH,
	NUM_LS
} GantryLimitSwitchPins;


// Block Detection Limit Switches


// Electromagnets


// Time Mode Switch


// LCD Screen


// RGB Strip Pins

