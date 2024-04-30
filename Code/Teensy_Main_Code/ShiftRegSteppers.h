// The shift register steppers are used for rotating 
#pragma once

#include <Arduino.h>

#include "Blocks.h"


//	*************************************************************************************************
//	Shared Enumerations for Shift Register Steppers
//	*************************************************************************************************

// Enumerations for the steppers that will rotate the displayed blocks. This is the same as the BlockColumn enumeration
typedef BlockColumn BlockStepper;
#define NUM_BLOCK_STEPPERS NUM_COLUMNS // Number of steppers






//	*************************************************************************************************
//	Shared Structs for the Shift Register Steppers code`
//	*************************************************************************************************





//	*************************************************************************************************
//	Shared Variables for the Shift Register Steppers code
//	*************************************************************************************************





//	*************************************************************************************************
//	Shared Functions for the Shift Register Steppers code
//	*************************************************************************************************





//	*************************************************************************************************
//	Shared Functions for the Shift Register Steppers code
//	*************************************************************************************************

// Initialize the shift register steppers
void initShiftRegSteppers();


/// Move a Stepper to a specific position
/// @param stepper The stepper to move
/// @param position The position to move to
void rotateToPositition(BlockStepper stepper, uint8_t position);


/// Move a stepper a given number of steps
/// @param stepper The stepper to move
/// @param steps The number of steps to move
void rotateSteps(BlockStepper stepper, int8_t steps);


/// Move a stepper to a given block face
/// @param stepper The stepper to move
/// @param block The block which is currently on the stepper
/// @param face The face to move to
void rotateToFace(BlockStepper stepper, Block *block, uint8_t face);


/// Move a Stepper to the 0 position / Home (where limit switch is triggered)
/// @param stepper The stepper to move to 0
void rotateToHome(BlockStepper stepper);


// Move Steppers if needed. This function will be called at a regular interval to move the steppers.
void moveDispalySteppers();
