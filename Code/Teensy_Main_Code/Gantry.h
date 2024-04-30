// This is the header for the code which will move the Gantry. The Gantry is responsible for moving the blocks around the clock.

#pragma once // Include this file only once

#include <Arduino.h>

#include "Blocks.h"


//	*************************************************************************************************
//	Enumerations for the Gantry
//	*************************************************************************************************

// The states of the Gantry
typedef enum {
	GANTRY_IDLE,
	GANTRY_MOVING,
	GANTRY_ERROR
} GantryState;



// The directions the Gantry can move
typedef enum {
	GANTRY_NO_DIR,
	GANTRY_UP,
	GANTRY_DOWN,
	GANTRY_FW,
	GANTRY_BW//,
	// GANTRY_FW_UP,
	// GANTRY_FW_DOWN,
	// GANTRY_BW_UP,
	// GANTRY_BW_DOWN
} GantryDirection;





//	*************************************************************************************************
//	Structs for the Gantry
//	*************************************************************************************************

// Struct to hold the information of the Gantry
typedef struct {
	GantryState state;	// The state of the Gantry
	GantryDirection dir;	// The direction the Gantry is moving
	uint8_t currentX;	// The current X position of the Gantry
	uint8_t currentY;	// The current Y position of the Gantry
	uint8_t targetX;	// The target X position of the Gantry
	uint8_t targetY;	// The target Y position of the Gantry
} GantryInfo;




//	*************************************************************************************************
//	Shared Variables and Constants for the Gantry code
//	*************************************************************************************************

const uint16_t StepPeriodUs = 2000;
const uint16_t StepperCurrentLimit = 1700; // 1700mA





//	*************************************************************************************************
//	Function prototypes for the Gantry code
//	*************************************************************************************************

// Initialize the Gantry
void InitGantry();


/// Get the current state of the Gantry
/// @return The current state of the Gantry.
GantryState GetGantryState();


/// Get the current direction of the Gantry
/// @return The current direction of the Gantry.
GantryDirection GetGantryDirection();


/// Swap the blocks provided with their partners. This function will NOT handle swapping the blocks separately if that is needed.
/// That should be handled by the calling function in BlockManager.
/// @param block1 The first block to swap.
/// @param block2 The second block to swap. If nullptr, only block1 will be swapped.
void SwapBlocks(Block *block1, Block *block2 = nullptr);


// Move the Gantry toward the target position. This function will be called at a
// regular interval by the main loop to move the Gantry,
// and it will only move the gantry if it needs to.
void MoveGantry();