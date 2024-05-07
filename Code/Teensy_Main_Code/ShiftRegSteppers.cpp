// This file manages the stepper motors that rotate the displayed blocks. These motors are all connected via shift registers.

#include "ShiftRegSteppers.h"
#include "Pins.h"



//	*************************************************************************************************
//	Local Enumerations for Shift Register Steppers
//	*************************************************************************************************

// Step Patterns for the Steppers
typedef enum {
	STEP_1,
	STEP_2,
	STEP_3,
	STEP_4,
	NUM_STEP_PATTERNS
} Step;



// The current state of a display stepper
typedef enum {
	SR_STEPPER_IDLE,
	SR_STEPPER_MOVING,
	SR_STEPPER_HOMING
} SRStepperState;



// The directions the steppers can move
typedef enum {
	SR_STEPPER_NO_DIR,
	SR_STEPPER_CW,
	SR_STEPPER_CCW
} SRStepperDirection;




//	*************************************************************************************************
//	Local Structs for the Shift Register Steppers code
//	*************************************************************************************************

// Struct to hold the information of a display stepper
typedef struct {
	SRStepperState state;	// The state of the stepper
	SRStepperDirection dir;	// The direction the stepper is moving
	Step currentStep;		// The current step of the stepper
	uint16_t currentPos;	// The current position of the stepper
	uint16_t targetPos;		// The target position of the stepper
} BlockStepperInfo;




//	*************************************************************************************************
//	Local Variables for the Shift Register Steppers code
//	*************************************************************************************************

BlockStepperInfo BlockSteppers[NUM_BLOCK_STEPPERS];	// The information of the Block Steppers

uint16_t stepData = 0;						// The current step pattern for all of the display steppers.

elapsedMillis displayStepperTimer;				// Timer for the stepper movement

const byte stepPatterns[NUM_STEP_PATTERNS] = {0b1010, 0b0110, 0b0101, 0b1001};

const uint16_t stepPeriodUs = 4900;				// The period of the steps in microseconds
// const uint16_t clockPeriodNs = 40;				// The period of the shift register clock in nanoseconds
const uint16_t stepsPerRevolution = 2048;		// The number of steps per revolution of the stepper motor




//	*************************************************************************************************
//	Local Low Level Functions for the Shift Register Steppers code
//	*************************************************************************************************

// Shift the data out to the shift registers
void outputStepData(){
	digitalWriteFast(DisplayStepperLatchPin, LOW);

	for (int8_t i = NUM_BLOCK_STEPPERS*4 -1; i >=0 ; i--){// Loop through all the stepper's pins
		delayNanoseconds(20);								// Delay for the minimum pulse duration of the Data Clock
		digitalWriteFast(DisplayStepperClockPin, LOW);

		digitalWriteFast(DisplayStepperDataPin, bitRead(stepData, i));
		delayNanoseconds(24); // Delay for the minimum time for Data before Clock
		digitalWriteFast(DisplayStepperClockPin, HIGH);
	}
	
	delayNanoseconds(18); // Delay for the minimum time for Data Clock before Latch Clock
	digitalWriteFast(DisplayStepperLatchPin, HIGH);
}// End of outputStepData








//	*************************************************************************************************
//	Local Higher level Functions for the Shift Register Steppers code
//	*************************************************************************************************

/// Set all the pins for all steppers to off
void clearSteppers(){
	// Set all the pins for all steppers to off
	stepData = 0;
	outputStepData();
}// End of clearSteppers



// Clear a specific stepper
void clearStepper(BlockStepper stepper){
	BlockSteppers[stepper].state = SR_STEPPER_IDLE;
	BlockSteppers[stepper].dir = SR_STEPPER_NO_DIR;
	stepData = stepData & ~(0b1111 << (stepper * 4));
}// End of clearStepper



// Set the step pattern for a stepper
void setNextStepData(BlockStepper stepper, Step currentStep, SRStepperDirection dir){
	switch(dir){
		case SR_STEPPER_NO_DIR:
			clearStepper(stepper);
			return;
		case SR_STEPPER_CW:
			BlockSteppers[stepper].currentStep = (Step)((currentStep + 1) % NUM_STEP_PATTERNS);
			break;
		case SR_STEPPER_CCW:
			BlockSteppers[stepper].currentStep = (Step)((currentStep - 1) % NUM_STEP_PATTERNS);
			break;
	}
	stepData = stepData & ~(0b1111 << (stepper * 4));		// Clear the current step's data
	stepData = stepData | (stepPatterns[BlockSteppers[stepper].currentStep] << (stepper * 4));	// Set the new step's data
}// End of setNextStepData




//	*************************************************************************************************
//	Shared Functions for the Shift Register Steppers code
//	*************************************************************************************************

// Initialize the shift register steppers
void InitShiftRegSteppers(){
	pinMode(DisplayStepperDataPin, OUTPUT);
	pinMode(DisplayStepperClockPin, OUTPUT);
	pinMode(DisplayStepperLatchPin, OUTPUT);

	for(uint8_t i = 0; i < NUM_BLOCK_STEPPERS; i++){// Initialize the Block Steppers
		BlockSteppers[i].state = SR_STEPPER_IDLE;
		BlockSteppers[i].dir = SR_STEPPER_NO_DIR;
		BlockSteppers[i].currentStep = STEP_1;
		BlockSteppers[i].currentPos = 0;
		BlockSteppers[i].targetPos = 0;
	}// End of for

	// Rotate to the home position
	for(uint8_t i = 0; i < NUM_BLOCK_STEPPERS; i++){
		RotateToHome((BlockStepper)i);
	}// End of for
}// End of initShiftRegSteppers



/// Check if all the steppers are idle
/// @return True if all the steppers are idle, false otherwise
bool DisplaySteppersIdle(){
	for(uint8_t i = 0; i < NUM_BLOCK_STEPPERS; i++){
		if(BlockSteppers[i].state != SR_STEPPER_IDLE){
			return false;
		}
	}
	return true;
}// End of DisplaySteppersIdle



/// Move a stepper a given number of steps
/// @param stepper The stepper to move
/// @param steps The number of steps to move
void RotateSteps(BlockStepper stepper, int8_t steps){
	BlockSteppers[stepper].state = SR_STEPPER_MOVING;
	BlockSteppers[stepper].dir = (steps > 0) ? SR_STEPPER_CW : SR_STEPPER_CCW;
	BlockSteppers[stepper].targetPos = BlockSteppers[stepper].currentPos + steps;
}// End of rotateSteps



/// Move a Stepper to a specific position
/// @param stepper The stepper to move
/// @param position The position to move to
void RotateToPositition(BlockStepper stepper, uint8_t position){
	BlockSteppers[stepper].state = SR_STEPPER_MOVING;
	BlockSteppers[stepper].dir = (position > BlockSteppers[stepper].currentPos) ? SR_STEPPER_CW : SR_STEPPER_CCW;
	BlockSteppers[stepper].targetPos = position;
}// End of rotateToPositition



/// Move a stepper to a given block face
/// @param stepper The stepper to move
/// @param block The block which is currently on the stepper
/// @param face The face to move to
void RotateToFace(BlockStepper stepper, Block *block, uint8_t face){
	BlockSteppers[stepper].state = SR_STEPPER_MOVING;
	BlockSteppers[stepper].dir = (face > block->currentValue) ? SR_STEPPER_CW : SR_STEPPER_CCW;
	BlockSteppers[stepper].targetPos = face;
}// End of rotateToFace



/// Move a Stepper to the 0 position / Home (where limit switch is triggered)
/// @param stepper The stepper to move to 0
void RotateToHome(BlockStepper stepper){
	BlockSteppers[stepper].state = SR_STEPPER_HOMING;
	BlockSteppers[stepper].dir = SR_STEPPER_CW;
	BlockSteppers[stepper].targetPos = 0;
}// End of rotateToHome



// Move Steppers if needed. This function will be called in the main loop, and ran at a regular interval managed internally
void MoveDisplaySteppers(){
	// Check the time interval since the last time this function was called
	if(displayStepperTimer >= stepPeriodUs){
		displayStepperTimer = 0;	// Reset the timer

		bool isMoving = false;		// Flag to check if any of the steppers are moving

		// Loop through all the steppers
		for(uint8_t i = 0; i < NUM_BLOCK_STEPPERS; i++){
			// Check the stepper's state
			switch(BlockSteppers[i].state){
				case SR_STEPPER_IDLE:
					// Do nothing / keep the stepper in the idle state

					break;
				case SR_STEPPER_MOVING:
					// Verify that it should be moving, then move the stepper toward the target position
					// If it is, set the state to idle
					isMoving = true;
					if(BlockSteppers[i].currentPos != BlockSteppers[i].targetPos){
						setNextStepData((BlockStepper)i, BlockSteppers[i].currentStep, BlockSteppers[i].dir);
						BlockSteppers[i].currentPos += (BlockSteppers[i].dir == SR_STEPPER_CW) ? 1 : -1;
					}else{
						// Set the stepper to idle and turn off the stepper
						clearStepper((BlockStepper)i);
					}
					break;
				case SR_STEPPER_HOMING:
					// Move the stepper toward the home position until the limit switch is triggered, then set that as the home position
					isMoving = true;
					if(digitalRead(BlockRotationLimitSwitchPins[i])){
						setNextStepData((BlockStepper)i, BlockSteppers[i].currentStep, BlockSteppers[i].dir);
						BlockSteppers[i].currentPos += (BlockSteppers[i].dir == SR_STEPPER_CW) ? 1 : -1;
					}else{
						BlockSteppers[i].currentPos = 0; // Reset the home position to here
						clearStepper((BlockStepper)i);
					}
					break;
			}// End of switch
		}// End of for

		// If at least one stepper is moving, output the step data
		if(isMoving){
			outputStepData();
		}
	}// End of if displayStepperTimer
}// End of moveDisplaySteppers


