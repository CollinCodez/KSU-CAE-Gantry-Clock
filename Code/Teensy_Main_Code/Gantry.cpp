// Code for Managing the Gantry. This includes controlling all the stepper motors and moving the blocks around the clock.

#include <Arduino.h> // Include the Arduino library to use the Arduino functions
#include <SPI.h>
#include <HighPowerStepperDriver.h>
// #include <Bounce.h>


#include "Config.h"
#include "Gantry.h" // Include the header file for the Gantry code
#include "ShiftRegSteppers.h" // Needed to check if the display steppers are idle
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
	GANTRY_BLOCK_TOP = 200,
	GANTRY_ABS_BOTTOM = 400
} GantryVtPosition;



// The Steps of the Calibration Process
typedef enum {
	GANTRY_CALIBRATE_START,
	GANTRY_CALIBRATE_TOP,
	GANTRY_CALIBRATE_BOTTOM,
	GANTRY_CALIBRATE_FRONT,
	GANTRY_CALIBRATE_BACK,
	GANTRY_CALIBRATE_END
} GantryCalibrationStep;



// The Steps of Swapping Blocks
typedef enum {
	GANTRY_SWAP_START,					// Move to Top
	GANTRY_SWAP_MOVE_FORWARD,			// Move to above the display row
	GANTRY_SWAP_PICKUP_OLD,				// Pick up the block from the display row
	GANTRY_SWAP_RAISE_OLD,				// Move the block to the storage row
	GANTRY_SWAP_GO_TO_OLD_ROW,			// Move the block to the storage row
	GANTRY_SWAP_PLACE_OLD,				// Place the old block in the storage row
	GANTRY_SWAP_UP_FROM_OLD,			// Move up from the storage row
	GANTRY_SWAP_MOVE_TO_NEW,			// Move to the new block
	GANTRY_SWAP_PICKUP_NEW,				// Pick up the new block from the storage row
	GANTRY_SWAP_RAISE_NEW,				// Move the new block up
	GANTRY_SWAP_MOVE_NEW_FORWARD,		// Move the new block to the display row
	GANTRY_SWAP_PLACE_NEW,				// Place the new block in the display row
	GANTRY_SWAP_END						// End of the block swap process (move to middle position)
} GantryBlockSwapStep;



// The steps of the Homing Process
typedef enum {
	GANTRY_HOMEING_UP,
	GANTRY_HOMING_FORWARD
} GantryHomeStep;





//	*************************************************************************************************
//	Structs for the Gantry
//	*************************************************************************************************

// Struct to hold the information of the Gantry
typedef struct {
	GantryState state;	// The state of the Gantry
	GantryDirection dir;	// The direction the Gantry is moving

	union{// The current step of the calibration process or the block swap process. This is a union because the Gantry can only be in one of these states at a time.
		GantryCalibrationStep calStep;	// The current step of the calibration process
		GantryBlockSwapStep swapStep;	// The current step of the block swap process
		GantryHomeStep homeStep;		// The current step of the homing process
	};

	Block *block1;	// The first block to swap
	Block *block2;	// The second block to swap
	
	uint8_t currentX;	// The current X position of the Gantry
	uint8_t currentY;	// The current Y position of the Gantry
	uint8_t targetX;	// The target X position of the Gantry
	uint8_t targetY;	// The target Y position of the Gantry
} GantryInfo;





//	*************************************************************************************************
//	Local Variables for the Gantry code
//	*************************************************************************************************

GantryInfo gantryInfo;	// The information of the Gantry

HighPowerStepperDriver stepperDrivers[NUM_MOTORS];	// The stepper drivers for the Gantry motors

elapsedMicros timeSinceLastStep;	// The time since the last step of the Gantry motors


uint8_t blockDropHeightOffset = 50;	// The offset for the height to drop the blocks from the electromagnet




//	*************************************************************************************************
//	Local Functions for the Gantry code
//	*************************************************************************************************

/// Step the Gantry in the direction set
void StepGantry(){
	switch(gantryInfo.dir){
		case GANTRY_NO_DIR:// If the Gantry is not moving, return
			return;
		case GANTRY_UP:// If the Gantry is moving up
			gantryInfo.currentY++;
			break;
		case GANTRY_DOWN:// If the Gantry is moving down
			gantryInfo.currentY--;
			break;
		case GANTRY_FW:// If the Gantry is moving forward
			gantryInfo.currentX++;
			break;
		case GANTRY_BW:// If the Gantry is moving backward
			gantryInfo.currentX--;
			break;
		default:
			return;
	}

	for(uint8_t i = 0; i < NUM_MOTORS; i++){// Step each motor
		stepperDrivers[i].step();
	}

	// CHECK IF HITTING LIMITS
	// switch(gantryInfo.dir){
	// 	case GANTRY_UP:
	// 		if(digitalRead(GantryLimitSwitches[GANTRY_TOP])){
	// 			gantryInfo.state = GANTRY_IDLE;
	// 		}
	// 		break;
	// 	case GANTRY_DOWN:
	// 		if(digitalRead(GantryLimitSwitches[GANTRY_BOTTOM])){
	// 			gantryInfo.state = GANTRY_IDLE;
	// 		}
	// 		break;
	// 	case GANTRY_FW:
	// 		if(digitalRead(GantryLimitSwitches[GANTRY_FRONT])){
	// 			gantryInfo.state = GANTRY_IDLE;
	// 		}
	// 		break;
	// 	case GANTRY_BW:
	// 		if(digitalRead(GantryLimitSwitches[GANTRY_BACK])){
	// 			gantryInfo.state = GANTRY_IDLE;
	// 		}
	// 		break;
	// 	default:
	// 		break;
	// }
	

}// End of StepGantry()



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



// Trigger the Gantry Homing Process
void HomeGantry(){
	gantryInfo.state = GANTRY_HOMING;
	if(gantryInfo.currentY > GANTRY_TOP){// If the Gantry is not at the top, move it up
		gantryInfo.homeStep = GANTRY_HOMEING_UP;
	}else if(gantryInfo.currentX > GANTRY_FRONT){// If the Gantry is not at the front, move it forward
		gantryInfo.homeStep = GANTRY_HOMING_FORWARD;
	}else{
		gantryInfo.state = GANTRY_IDLE;
	}
}// End of HomeGantry()



// Handle the Calibration of the Gantry
void CalibrateGantryProcess(){

}// End of CalibrateGantry()



// Check if the gantry is currently supposed to have a second block
bool HasSecondBlock(){
	if(gantryInfo.block2 != nullptr){
		return true;
	}
	return false;
}// End of HasSecondBlock()



// Handle the Swapping of Blocks
void SwapBlocksProcess(){
	switch(gantryInfo.swapStep){
		case GANTRY_SWAP_START:
			// Move the Gantry to the top
			StepGantry();
			if((gantryInfo.currentY == GANTRY_TOP) || digitalRead(GANTRY_LEFT_UP_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_UP_LIMIT_SWITCH)){
				ChangeGantryDirection(GANTRY_FW);
				gantryInfo.swapStep = GANTRY_SWAP_MOVE_FORWARD;
			}
			break;
		case GANTRY_SWAP_MOVE_FORWARD:
			// Move the Gantry to the display row
			StepGantry();
			if((gantryInfo.currentX == GANTRY_FRONT) || digitalRead(GANTRY_LEFT_FW_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_FW_LIMIT_SWITCH)){
				ChangeGantryDirection(GANTRY_DOWN);
				gantryInfo.swapStep = GANTRY_SWAP_PICKUP_OLD;
			}
			break;
		case GANTRY_SWAP_PICKUP_OLD:
			// Pick up the old block
			if(!((gantryInfo.currentY >= GANTRY_MIDDLE_VT) && (!DisplaySteppersIdle()))){// Wait for the display steppers to be idle if the Gantry is at or below the middle vertical position
				StepGantry();
			}
			if((gantryInfo.currentY == GANTRY_BLOCK_TOP) || digitalRead(HOURS_SECOND_DIGIT_GANTRY_LS) || digitalRead(MINS_SECOND_DIGIT_GANTRY_LS)){// If the Gantry is at the block top position or detects a block
				// Turn on the electromagnet(s) to pick up the block(s)
				switch(gantryInfo.block1->column){
					case HOURS_SECOND_DIGIT_COLUMN:
						digitalWrite(HOURS_SECOND_DIGIT_EMAG, HIGH);
						break;
					case MINS_SECOND_DIGIT_COLUMN:
						digitalWrite(MINS_SECOND_DIGIT_EMAG, HIGH);
						break;
				}

				if(HasSecondBlock()){// If there is a second block, pick it up as well
					switch(gantryInfo.block2->column){
						case HOURS_SECOND_DIGIT_COLUMN:
							digitalWrite(HOURS_SECOND_DIGIT_EMAG, HIGH);
							break;
						case MINS_SECOND_DIGIT_COLUMN:
							digitalWrite(MINS_SECOND_DIGIT_EMAG, HIGH);
							break;
					}
				}

				gantryInfo.swapStep = GANTRY_SWAP_RAISE_OLD;
				ChangeGantryDirection(GANTRY_UP);
			}
			break;
		case GANTRY_SWAP_RAISE_OLD:
			// Raise the old block up
			StepGantry();
			if((gantryInfo.currentY == GANTRY_TOP) || digitalRead(GANTRY_LEFT_UP_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_UP_LIMIT_SWITCH)){
				ChangeGantryDirection(GANTRY_BW);
				gantryInfo.swapStep = GANTRY_SWAP_GO_TO_OLD_ROW;

				// Save the block's storage position as the target position
				switch(gantryInfo.block1->storageRow){
					case MIDDLE_ROW:
						gantryInfo.targetX = GANTRY_MIDDLE_HZ;
						break;
					case BACK_ROW:
						gantryInfo.targetX = GANTRY_BACK;
						break;
				}

			}
			break;
		case GANTRY_SWAP_GO_TO_OLD_ROW:
			// Move the old block to the storage row
			StepGantry();
			if((gantryInfo.currentX == gantryInfo.targetX) || digitalRead(GANTRY_LEFT_BW_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_BW_LIMIT_SWITCH)){
				ChangeGantryDirection(GANTRY_DOWN);
				gantryInfo.swapStep = GANTRY_SWAP_PLACE_OLD;
			}
			break;
		case GANTRY_SWAP_PLACE_OLD:
			// Place the old block in the storage row
			StepGantry();
			if((gantryInfo.currentY ==(GANTRY_BLOCK_TOP - blockDropHeightOffset)) || digitalRead(GANTRY_LEFT_DOWN_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_DOWN_LIMIT_SWITCH)){
				// Turn off the electromagnet(s) to release the block(s)
				digitalWrite(HOURS_SECOND_DIGIT_EMAG, LOW);
				digitalWrite(MINS_SECOND_DIGIT_EMAG, LOW);

				ChangeGantryDirection(GANTRY_UP);
				gantryInfo.swapStep = GANTRY_SWAP_UP_FROM_OLD;
			}
			break; 
		case GANTRY_SWAP_UP_FROM_OLD:
			// Move up from the storage row
			StepGantry();
			if((gantryInfo.currentY == GANTRY_TOP) || digitalRead(GANTRY_LEFT_UP_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_UP_LIMIT_SWITCH)){
				ChangeGantryDirection(GANTRY_FW);
				gantryInfo.swapStep = GANTRY_SWAP_MOVE_TO_NEW;
				
				// Save the new block's storage position (the row that this block was NOT fromas the target position
				switch(gantryInfo.block1->storageRow){
					case MIDDLE_ROW:
						gantryInfo.targetX = GANTRY_BACK;
						break;
					case BACK_ROW:
						gantryInfo.targetX = GANTRY_MIDDLE_HZ;
						break;
				}

				if(gantryInfo.targetX > gantryInfo.currentX){
					ChangeGantryDirection(GANTRY_FW);
				}else{
					ChangeGantryDirection(GANTRY_BW);
				}
			}
			break;
		case GANTRY_SWAP_MOVE_TO_NEW:
			// Move to the new block
			StepGantry();
			if((gantryInfo.currentX == gantryInfo.targetX) || digitalRead(GANTRY_LEFT_FW_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_FW_LIMIT_SWITCH) || digitalRead(GANTRY_LEFT_BW_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_BW_LIMIT_SWITCH)){
				ChangeGantryDirection(GANTRY_DOWN);
				gantryInfo.swapStep = GANTRY_SWAP_PICKUP_NEW;
			}
			break;
		case GANTRY_SWAP_PICKUP_NEW:
			// Pick up the new block
			StepGantry();

			if((gantryInfo.currentY == GANTRY_BLOCK_TOP) || digitalRead(HOURS_SECOND_DIGIT_GANTRY_LS) || digitalRead(MINS_SECOND_DIGIT_GANTRY_LS)){// If the Gantry is at the block top position or detects a block
				// Turn on the electromagnet(s) to pick up the block(s)
				switch(gantryInfo.block1->column){
					case HOURS_SECOND_DIGIT_COLUMN:
						digitalWrite(HOURS_SECOND_DIGIT_EMAG, HIGH);
						break;
					case MINS_SECOND_DIGIT_COLUMN:
						digitalWrite(MINS_SECOND_DIGIT_EMAG, HIGH);
						break;
				}

				if(HasSecondBlock()){// If there is a second block, pick it up as well
					switch(gantryInfo.block2->column){
						case HOURS_SECOND_DIGIT_COLUMN:
							digitalWrite(HOURS_SECOND_DIGIT_EMAG, HIGH);
							break;
						case MINS_SECOND_DIGIT_COLUMN:
							digitalWrite(MINS_SECOND_DIGIT_EMAG, HIGH);
							break;
					}
				}

				gantryInfo.swapStep = GANTRY_SWAP_MOVE_NEW_FORWARD;
				ChangeGantryDirection(GANTRY_UP);
			}
			break;
		case GANTRY_SWAP_RAISE_NEW:
			// Raise the new block up
			StepGantry();
			if((gantryInfo.currentY == GANTRY_TOP) || digitalRead(GANTRY_LEFT_UP_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_UP_LIMIT_SWITCH)){
				ChangeGantryDirection(GANTRY_FW);
				gantryInfo.swapStep = GANTRY_SWAP_MOVE_NEW_FORWARD;
			}
			break;
		case GANTRY_SWAP_MOVE_NEW_FORWARD:
			// Move the new block to the display row
			StepGantry();
			if((gantryInfo.currentX == GANTRY_FRONT) || digitalRead(GANTRY_LEFT_FW_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_FW_LIMIT_SWITCH)){
				ChangeGantryDirection(GANTRY_DOWN);
				gantryInfo.swapStep = GANTRY_SWAP_PLACE_NEW;
			}
			break;
		case GANTRY_SWAP_PLACE_NEW:
			// Place the new block in the display row
			StepGantry();
			if((gantryInfo.currentY == GANTRY_BLOCK_TOP) || digitalRead(GANTRY_LEFT_DOWN_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_DOWN_LIMIT_SWITCH)){
				// Turn off the electromagnet(s) to release the block(s)
				digitalWrite(HOURS_SECOND_DIGIT_EMAG, LOW);
				digitalWrite(MINS_SECOND_DIGIT_EMAG, LOW);

				ChangeGantryDirection(GANTRY_UP);
				gantryInfo.swapStep = GANTRY_SWAP_END;
			}
			break;
		case GANTRY_SWAP_END:
			// Move to the middle position
			StepGantry();
			if((gantryInfo.currentY == GANTRY_MIDDLE_VT) || digitalRead(GANTRY_LEFT_UP_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_UP_LIMIT_SWITCH)){
				gantryInfo.state = GANTRY_IDLE;
			}
			break;
	}
}// End of SwapBlocksProcess()



// Handle the Homing of the Gantry
void HomeGantryProcess(){
	switch(gantryInfo.homeStep){
		case GANTRY_HOMEING_UP:
			ChangeGantryDirection(GANTRY_UP);
			StepGantry();
			if(digitalRead(GANTRY_LEFT_UP_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_UP_LIMIT_SWITCH)){
				gantryInfo.homeStep = GANTRY_HOMING_FORWARD;
			}
			break;
		case GANTRY_HOMING_FORWARD:
			ChangeGantryDirection(GANTRY_FW);
			StepGantry();
			if(digitalRead(GANTRY_LEFT_FW_LIMIT_SWITCH) || digitalRead(GANTRY_RIGHT_FW_LIMIT_SWITCH)){
				gantryInfo.state = GANTRY_IDLE;
			}
			break;
	}
}





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

	// Set up and turn off the electromagnets
	pinMode(HOURS_SECOND_DIGIT_EMAG, OUTPUT);
	pinMode(MINS_SECOND_DIGIT_EMAG, OUTPUT);

	digitalWrite(HOURS_SECOND_DIGIT_EMAG, LOW);
	digitalWrite(MINS_SECOND_DIGIT_EMAG, LOW);


	// Set up the Gantry Limit Switches

	// Set the Gantry to the Home Position              TODO
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
	if(block2 != nullptr){
		if(block1->storageRow != block2->storageRow){
			SERIAL_PRINTF("ERROR: %s\n", "Gantry was told to move blocks that are not in the same row.");
			return;									// If the blocks are not in the same row, return. This should have been handled by the calling function.
		}
	}

	// Put the blocks in the GantryInfo struct
	gantryInfo.block1 = block1;
	gantryInfo.block2 = block2;

	// Set the Gantry to the Swap Blocks state
	gantryInfo.state = GANTRY_SWAPPING_BLOCKS;
	gantryInfo.swapStep = GANTRY_SWAP_START;

	if((gantryInfo.currentY == GANTRY_MIDDLE_VT) && (gantryInfo.currentX == GANTRY_FRONT)){// If the Gantry is at the middle vertical position, skip to pickup the old block
		gantryInfo.swapStep = GANTRY_SWAP_PICKUP_OLD;
		ChangeGantryDirection(GANTRY_DOWN);
	}else{
		gantryInfo.swapStep = GANTRY_SWAP_START;
		ChangeGantryDirection(GANTRY_UP);

	}
}// End of SwapBlocks()



// Move the Gantry toward the target position
void MoveGantry(){
	if(timeSinceLastStep >= StepPeriodUs){// If it is time to step the Gantry
		timeSinceLastStep = 0;
		switch(gantryInfo.state){
			case GANTRY_IDLE:
			case GANTRY_ERROR:
				// Do nothing
				break;
			case GANTRY_CALIBRATING:
				CalibrateGantryProcess();
				break;
			case GANTRY_SWAPPING_BLOCKS:
				SwapBlocksProcess();
				break;
			case GANTRY_HOMING:
				HomeGantryProcess();
		}
	}
}// End of MoveGantry()