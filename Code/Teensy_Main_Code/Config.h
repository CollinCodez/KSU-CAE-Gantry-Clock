// Configure Preprocessor Definitions & Macros here

#pragma once // Include this file only once


#define SERIAL_ENABLED 1 // Enable Serial Debugging


#if SERIAL_ENABLED	// Macro to print to serial if serial is enabled, otherwise do nothing
	#define SERIAL_PRINTF(format, ...) Serial.printf(format, __VA_ARGS__);
#else
	#define SERIAL_PRINTF(format, ...) {}
#endif



#define SD_LOGGING 0 // Enable SD Logging