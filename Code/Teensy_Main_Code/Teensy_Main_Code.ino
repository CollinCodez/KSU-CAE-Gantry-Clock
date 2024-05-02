// This is the main code for the Teensy 4.1, which is responsible for managing everything on the clock.


#include <Arduino.h> // Include the Arduino library to use the Arduino functions
#include <TimeAlarms.h> // Include the timealarms library to use the time alarms functions

#include "Config.h" 			// The config file to use the configuration variables
#include "TimeManager.h" 		// The time manager library deals with getting the time and setting the internal RTC
#include "BlockManager.h" 		// The block manager library deals with deciding which block to display and/or rotate and when to do so
#include "Gantry.h" 			// The gantry library manages moving the gantry to the correct position to move blocks
#include "ShiftRegSteppers.h" 	// The shift register steppers library manages the steppers that rotate the blocks, which are all controlled via shift registers



void setup() {
	#if (SERIAL_ENABLED)
		Serial.begin(115200);
	#endif

	InitTime();				// Initialize the time manager

	InitShiftRegSteppers();	// Initialize the shift register (display block rotation) steppers
}



void loop() {



	// Move things as needed. These functions will only run on internally managed intervals.
	MoveGantry();					// Move the Gantry.
	MoveDisplaySteppers();			// Move the display steppers.
}