// The shift register steppers are used for rotating 
#pragma once

#include <Arduino.h>

#include "Blocks.h"		// Include the Blocks header file to get the BlockStepper enumeration


//	*************************************************************************************************
//	Shared Enumerations for Shift Register Steppers
//	*************************************************************************************************

// Enumerations for the steppers that will rotate the displayed blocks. This is the same as the BlockColumn enumeration and is now defined in Blocks.h
// typedef BlockColumn BlockStepper;
// #define NUM_BLOCK_STEPPERS NUM_COLUMNS // Number of steppers






//	*************************************************************************************************
//	Shared Structs for the Shift Register Steppers code`
//	*************************************************************************************************





//	*************************************************************************************************
//	Shared Variables for the Shift Register Steppers code
//	*************************************************************************************************





//	*************************************************************************************************
//	Shared Functions for the Shift Register Steppers code
//	*************************************************************************************************

// Initialize the shift register steppers
void InitShiftRegSteppers();


/// Check if all the steppers are idle
/// @return True if all the steppers are idle, false otherwise
bool DisplaySteppersIdle();


/// Move a stepper a given number of steps
/// @param stepper The stepper to move
/// @param steps The number of steps to move
void RotateSteps(BlockStepper stepper, int8_t steps);


/// Move a Stepper to a specific position
/// @param stepper The stepper to move
/// @param position The position to move to
void RotateToPositition(BlockStepper stepper, uint8_t position);


/// Move a stepper to a given block face
/// @param stepper The stepper to move
/// @param block The block which is currently on the stepper
/// @param face The face to move to
void RotateToFace(BlockStepper stepper, Block *block, uint8_t face);


/// Move a Stepper to the 0 position / Home (where limit switch is triggered)
/// @param stepper The stepper to move to 0
void RotateToHome(BlockStepper stepper);


// Move Steppers if needed. This function will be called in the main loop, and ran at a regular interval managed internally
void MoveDisplaySteppers();
