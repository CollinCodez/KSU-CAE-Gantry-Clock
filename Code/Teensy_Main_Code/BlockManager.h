// Header for the Block Management Code

#pragma once // Include this file only once

#include <Arduino.h>

#include "Blocks.h"


//	*************************************************************************************************
//	Enumerations for Block Management
//	*************************************************************************************************






//	*************************************************************************************************
//	Structs for the block management code
//	*************************************************************************************************






//	*************************************************************************************************
//	Shared Variables for the block management code
//	*************************************************************************************************





//	*************************************************************************************************
//	Function prototypes for the block management code
//	*************************************************************************************************

/// Initialize the blocks
void InitBlocks();


/// Verify that all the blocks are present.
/// @return True if all the blocks are present, false otherwise.
bool VerifyBlocks();