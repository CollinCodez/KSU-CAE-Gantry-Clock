// This is the Main file for the ESP32-C3, which will be used to retrieve the time via the network and send it to the Teensy 4.1 via i2c.


#include <Arduino.h>
#include <Wire.h> // Include Wire library for I2C communication
#include <WiFi.h> // Include WiFi library for ESP32
#include <WiFiManager.h>
#include <time.h> // Include time library for time-related functions


const char* publicNTPServerPool = "pool.ntp.org"; // Public NTP server address
const long gmtOffset_sec = -5 * 3600; // GMT offset in seconds (EST)
const int daylightOffset_sec = 3600; // Daylight saving time offset in seconds


void configModeCallback (WiFiManager *myWiFiManager) {
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP()); // Print IP address of soft AP
	Serial.println(myWiFiManager->getConfigPortalSSID()); // Print SSID of config portal
}



time_t getUnixTime() {
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo)) { // Get local time
		Serial.println("Failed to obtain time");
		return 0; // Return 0 if failed to obtain time
	}
	return mktime(&timeinfo); // Convert struct tm to Unix time
}



// Handle an I2C request from the Teensy / main microcontroller
void requestEvent() {
	time_t unixTime = getUnixTime(); // Get current Unix time
	Wire.write((byte*)&unixTime, sizeof(unixTime)); // Send Unix time to master
}



void setup() {
	Serial.begin(115200); // Initialize serial communication
	WiFiManager wifiManager; // Create WiFiManager object
	wifiManager.setAPCallback(configModeCallback); // Set callback for config mode
	wifiManager.autoConnect("Clock_Config_WiFi", "password"); // Connect to WiFi network
	Serial.println("WiFi connected!"); // Print message indicating WiFi connection
	// Serial.print("\nESP Board MAC Address: ");
	// Serial.println(WiFi.macAddress()); // Print MAC address of ESP board
	// Serial.println("\n");
	configTime(gmtOffset_sec, daylightOffset_sec, "10.128.10.31", "10.128.10.30", publicNTPServerPool); // Configure time settings
	Wire.begin(4); // Join I2C bus with address #4
	Wire.onRequest(requestEvent); // Register event handler for request
}



void loop() {
	delay(100); // Small delay to prevent excessive looping
}