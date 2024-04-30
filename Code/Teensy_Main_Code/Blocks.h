// Structs and Enumerations for the blocks

#pragma once // Include this file only once

#include <Arduino.h>


#define MAX_FACES 6 // The maximum number of faces on one number block


//	*************************************************************************************************
//	Enumerations for the blocks
//	*************************************************************************************************

// Enumerations for identifying the blocks (each block will have a unique identifier)
typedef enum {
	HOURS_FIRST_DIGIT,
	HOURS_SECOND_DIGIT_ONE,
	HOURS_SECOND_DIGIT_TWO,
	MINS_FIRST_DIGIT,
	MINS_SECOND_DIGIT_ONE,
	MINS_SECOND_DIGIT_TWO,
	NUM_BLOCKS // Number of blocks
} BlockType;



// Enumerations for the columns
typedef enum {
	HOURS_FIRST_DIGIT_COLUMN,
	HOURS_SECOND_DIGIT_COLUMN,
	MINS_FIRST_DIGIT_COLUMN,
	MINS_SECOND_DIGIT_COLUMN,
	NUM_COLUMNS // 	 of columns
} BlockColumn;



// Enumerations for the rows
typedef enum {
	DISPLAY_ROW,
	MIDDLE_ROW,
	BACK_ROW,
	NUM_ROWS // Number of rows
} BlockRow;




//	*************************************************************************************************
//	Structs for the blocks
//	*************************************************************************************************

// Struct to hold the information of a block
typedef struct {
	const BlockType blockType;					// The type of the block
	uint8_t currentValue;						// The current value of the block
	bool isStored;								// If the block is currently stored
	const uint8_t numFaces;						// The number of faces on the block
	const uint8_t stepsPerFace;					// The number of steps per face
	const uint8_t minValue;						// The minimum value of the block
	const uint8_t maxValue;						// The maximum value of the block
	const BlockColumn column;					// The column where the block belongs
	const BlockRow storageRow;					// The row where the block gets stored
} Block;	