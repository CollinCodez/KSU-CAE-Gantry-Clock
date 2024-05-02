// Manage getting the time from the ESP32 and setting the internal RTC

#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h>

#include "Config.h"
#include "TimeManager.h"


//	*************************************************************************************************
//	Local Variables for the Time Manager code
//	*************************************************************************************************

// The address of the ESP32
const uint8_t ESP32_ADDRESS = 4;




//	*************************************************************************************************
//	Local Functions for the Time Manager code
//	*************************************************************************************************

/// @brief Get the unix time from the ESP32
void GetTimeFromESP32(){
	// Request the time from the ESP32
	Wire.requestFrom(ESP32_ADDRESS, 4);
	time_t time = 0;
	for (int i = 0; i < 4; i++){
		time |= Wire.read() << (i * 8);
	}

	// Set the time
	setTime(time);

	// Print the time
	SERIAL_PRINTF("Time aquired from from ESP32: %lu\t%u/%u/%u %u:%u:%u\n", time, month(), day(), year(), hour(), minute(), second());
}


//	*************************************************************************************************
//	Shared Functions for the Time Manager code
//	*************************************************************************************************

/// @brief Initialize the Time Manager
void InitTime()
{
	// Initialize the Wire library
	Wire.begin();
	GetTimeFromESP32();
}



/// @brief Update the time
void UpdateTime()
{
	// Get the time from the ESP32
	GetTimeFromESP32();
}