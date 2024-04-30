// Code for Managing the Gantry. This includes controlling all the stepper motors and moving the blocks around the clock.

#include <Arduino.h> // Include the Arduino library to use the Arduino functions
#include <SPI.h>
#include <HighPowerStepperDriver.h>


#include "Gantry.h" // Include the header file for the Gantry code
#include "Pins.h"

//	*************************************************************************************************
//	Local Enumerations for the Gantry
//	*************************************************************************************************

// GantryLimitSwitches and GantryMotors are defined in Pins.h

// The Gantry Horizontal Positions. These are in the number of steps from the front of the clock
typedef enum {
	GANTRY_FRONT = 0,
	GANTRY_MIDDLE_HZ = 1500,
	GANTRY_BACK = 2000
} GantryHzPosition;



// The Gantry Vertical Positions. These are in the number of steps from the top of the clock
typedef enum {
	GANTRY_TOP = 0,
	GANTRY_MIDDLE_VT = 100,
	GANTRY_BOTTOM = 200
} GantryVtPosition;




//	*************************************************************************************************
//	Local Variables for the Gantry code
//	*************************************************************************************************

GantryInfo gantryInfo;	// The information of the Gantry

HighPowerStepperDriver stepperDrivers[NUM_MOTORS];	// The stepper drivers for the Gantry motors





//	*************************************************************************************************
//	Local Functions for the Gantry code
//	*************************************************************************************************

/// Step the Gantry in the direction set
void StepGantry(){
	if(gantryInfo.state != GANTRY_MOVING){// If the Gantry is not moving, return
		return;
	}

	for(uint8_t i = 0; i < NUM_MOTORS; i++){// Step each motor
		stepperDrivers[i].step();
	}

	// CHECK IF HITTING LIMITS
}



/// Change the Gantry Direction
/// @param dir The new direction of the Gantry.
///		Up and Down will move both motors of a pair in the same direction.
///		Left and Right will move the motors in opposite directions.
void ChangeGantryDirection(GantryDirection dir){
	// If the Gantry is already moving in the direction provided, return
	if(gantryInfo.dir == dir){
		return;
	}

	// Check what stepper motors directions need to be changed from the current direction
	switch(dir){
		case GANTRY_UP:
			stepperDrivers[GANTRY_LEFT_TOP_MOTOR].setDirection(0);
			stepperDrivers[GANTRY_LEFT_BOTM_MOTOR].setDirection(0);
			stepperDrivers[GANTRY_RIGHT_TOP_MOTOR].setDirection(1);
			stepperDrivers[GANTRY_RIGHT_BOTM_MOTOR].setDirection(1);
			break;
		case GANTRY_DOWN:
			stepperDrivers[GANTRY_LEFT_TOP_MOTOR].setDirection(1);
			stepperDrivers[GANTRY_LEFT_BOTM_MOTOR].setDirection(1);
			stepperDrivers[GANTRY_RIGHT_TOP_MOTOR].setDirection(0);
			stepperDrivers[GANTRY_RIGHT_BOTM_MOTOR].setDirection(0);
			break;
		case GANTRY_FW:
			stepperDrivers[GANTRY_LEFT_TOP_MOTOR].setDirection(0);
			stepperDrivers[GANTRY_LEFT_BOTM_MOTOR].setDirection(1);
			stepperDrivers[GANTRY_RIGHT_TOP_MOTOR].setDirection(0);
			stepperDrivers[GANTRY_RIGHT_BOTM_MOTOR].setDirection(1);
			break;
		case GANTRY_BW:
			stepperDrivers[GANTRY_LEFT_TOP_MOTOR].setDirection(1);
			stepperDrivers[GANTRY_LEFT_BOTM_MOTOR].setDirection(0);
			stepperDrivers[GANTRY_RIGHT_TOP_MOTOR].setDirection(1);
			stepperDrivers[GANTRY_RIGHT_BOTM_MOTOR].setDirection(0);
			break;
		default:
			return;
	}

	gantryInfo.dir = dir;
}// End of ChangeGantryDirection()



// Calibrate the Gantry
void CalibrateGantry(){

}// End of CalibrateGantry()





//	*************************************************************************************************
//	Shared Functions for the Gantry code
//	*************************************************************************************************

// Initialize the Gantry
void InitGantry(){
	SPI.begin();

	// Set the Chip Select Pins for the Stepper Drivers

	// Wait for the Stepper Drivers to initialize
	delay(1);

	for(uint8_t i = 0; i < NUM_MOTORS; i++){// Initialize the Stepper Drivers
		// Reset the driver to its default settings and clear latched status conditions.
		stepperDrivers[i].resetSettings();
		stepperDrivers[i].clearStatus();

		// Select auto mixed decay.  TI's DRV8711 documentation recommends this mode for most applications, and Pololu finds that it usually works well.
		stepperDrivers[i].setDecayMode(HPSDDecayMode::AutoMixed);

		// Set Current Limit for the Stepper Motor
		stepperDrivers[i].setCurrentMilliamps36v4(StepperCurrentLimit);

		// Set the Step Mode for the Stepper Motor
		stepperDrivers[i].setStepMode(HPSDStepMode::MicroStep1);

		// Enable the Motor Outputs
		stepperDrivers[i].enableDriver();
	}


}// End of InitGantry()



/// Get the current state of the Gantry
/// @return The current state of the Gantry.
GantryState GetGantryState(){
	return gantryInfo.state;
}



/// Get the current direction of the Gantry
/// @return The current direction of the Gantry.
GantryDirection GetGantryDirection(){
	return gantryInfo.dir;
}



/// Swap the blocks provided with their partners. This function will NOT handle swapping the blocks separately if that is needed.
/// That should be handled by the calling function in BlockManager.
/// @param block1 The first block to swap.
/// @param block2 The second block to swap. If nullptr, only block1 will be swapped.
void SwapBlocks(Block *block1, Block *block2 = nullptr){
	bool hasBlock2;
	if(block2 != nullptr){
		if(block1->storageRow != block2->storageRow){
			return;									// If the blocks are not in the same row, return. This should have been handled by the calling function.
		}
		hasBlock2 = true;
	} else {
		hasBlock2 = false;
	}


	// Pick up block(s) from display row


	// Move block(s) to storage row


	// Pick up new block(s) from other storage row
	

	// Move new block(s) to display row


	// Go to the Home Position
}// End of SwapBlocks()



// Move the Gantry toward the target position
void MoveGantry(){

}// End of MoveGantry()