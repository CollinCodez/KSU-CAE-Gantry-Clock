

#include "ShiftRegSteppers.h"



//	*************************************************************************************************
//	Local Enumerations for Shift Register Steppers
//	*************************************************************************************************

// Step Patterns for the Steppers
typedef enum {
	STEP_1 = 0b1010,
	STEP_2 = 0b0110,
	STEP_3 = 0b0101,
	STEP_4 = 0b1001
} StepPattern;



// The current state of a display stepper
typedef enum {
	SR_Stepper_IDLE,
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
	uint8_t currentStep;	// The current step of the stepper
	uint8_t currentPos;	// The current position of the stepper
	uint8_t targetPos;	// The target position of the stepper
} SRStepperInfo;




//	*************************************************************************************************
//	Local Variables for the Shift Register Steppers code
//	*************************************************************************************************

SRStepperInfo BlockSteppers[NUM_BLOCK_STEPPERS];	// The information of the Block Steppers





//	*************************************************************************************************
//	Local Functions for the Shift Register Steppers code
//	*************************************************************************************************

// Shift the data out to the shift registers
void shiftDataOut(){

}



/// Set all the pins for all steppers to off
void clearSteppers(){
	// Set all the pins for all steppers to off

}




//	*************************************************************************************************
//	Shared Functions for the Shift Register Steppers code
//	*************************************************************************************************




