//====================================================================
//Project Lynxmotion Phoenix
//Description: Phoenix, control file.
//The control input subroutine for the phoenix software is placed in this file.
//Can be used with V2.0 and above
//Configuration version: V1.0
//Date: 25-10-2009
//Programmer: Jeroen Janssen (aka Xan)
//             Kurt Eckhardt (aka KurtE) - converted to c ported to Arduino...
//
//Hardware setup: PS2 version
// 
//NEW IN V1.0
//- First Release
//
//Walk method 1:
//- Left StickWalk/Strafe
//- Right StickRotate
//
//Walk method 2:
//- Left StickDisable
//- Right StickWalk/Rotate
//
//
//PS2 CONTROLS:
//[Common Controls]
//- Start:          Turn on/off the bot
//- L1:             Toggle Shift mode
//- L2:             Toggle Rotate mode
//
//- Circle:         Toggle Single leg mode
//- Square:         Toggle Balance mode
//- Triangle:       Move body to 35 mm from the ground (walk pos) 
//                    and back to the ground
//
//- D-Pad up:       Body up 10 mm
//- D-Pad down:     Body down 10 mm
//- D-Pad left:     decrease speed with 50mS
//- D-Pad right:    increase speed with 50mS
//
//[Walk Controls]
//- select:         Switch gaits
//- Left Stick:     (Walk mode 1) Walk/Strafe
//                  (Walk mode 2) Disable
//- Right Stick:    (Walk mode 1) Rotate, 
//                  (Walk mode 2) Walk/Rotate
//- R1:             Toggle Double gait travel speed
//- R2:             Toggle Double gait travel length
//
//[Shift Controls]
//- Left Stick:     Shift body X/Z
//- Right Stick:    Shift body Y and rotate body Y
//
//[Rotate Controls]
//- Left Stick:     Rotate body X/Z
//- Right Stick:    Rotate body Y
//
//[Single leg Controls]
//- select;         Switch legs
//- Left Stick:     Move Leg X/Z (relative)
//- Right Stick:    Move Leg Y (absolute)
//- R2:             Hold/release leg position
//
//[GP Player Controls]
//- select:         Switch Sequences
//- R2:             Start Sequence
//
//====================================================================
// [Include files]
#if ARDUINO>99
#include <Arduino.h> // Arduino 1.0
#else
#include <Wprogram.h> // Arduino 0022
#endif
#include <PS2X_lib.h>

//[CONSTANTS]
#define WALKMODE          0
#define TRANSLATEMODE     1
#define ROTATEMODE        2
#define SINGLELEGMODE     3
#define GPPLAYERMODE      4


#define cTravelDeadZone 4      //The deadzone for the analog input from the remote
#define  MAXPS2ERRORCNT  5     // How many times through the loop will we go before shutting off robot?

#ifndef MAX_BODY_Y
#define MAX_BODY_Y 100
#endif

//=============================================================================
// Global - Local to this file only...
//=============================================================================
PS2X ps2x; // create PS2 Controller Class


// Define an instance of the Input Controller...
InputController  g_InputController;       // Our Input controller 


static short      g_BodyYOffset; 
static short      g_sPS2ErrorCnt;
static short       g_BodyYShift;
static byte        ControlMode;
static bool        DoubleHeightOn;
static bool        DoubleTravelOn;
static bool        WalkMethod;
byte            GPSeq;             //Number of the sequence
short              g_sGPSMController;    // What GPSM value have we calculated. 0xff - Not used yet

// some external or forward function references.
extern void PS2TurnRobotOff(void);

//==============================================================================
// This is The function that is called by the Main program to initialize
//the input controller, which in this case is the PS2 controller
//process any commands.
//==============================================================================

// If both PS2 and XBee are defined then we will become secondary to the xbee
void InputController::Init(void)
{
  int error;

  //error = ps2x.config_gamepad(57, 55, 56, 54);  // Setup gamepad (clock, command, attention, data) pins
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);  // Setup gamepad (clock, command, attention, data) pins

#ifdef DBGSerial
	DBGSerial.print("PS2 Init: ");
	DBGSerial.println(error, DEC);
#endif
  g_BodyYOffset = 0;    
  g_BodyYShift = 0;
  g_sPS2ErrorCnt = 0;  // error count

  ControlMode = WALKMODE;
  DoubleHeightOn = false;
  DoubleTravelOn = false;
  WalkMethod = false;

  g_InControlState.SpeedControl = 100;    // Sort of migrate stuff in from Devon.
}

//==============================================================================
// This function is called by the main code to tell us when it is about to
// do a lot of bit-bang outputs and it would like us to minimize any interrupts
// that we do while it is active...
//==============================================================================
void InputController::AllowControllerInterrupts(boolean fAllow)
{
  // We don't need to do anything...
}

//==============================================================================
// This is The main code to input function to read inputs from the PS2 and then
//process any commands.
//==============================================================================
void InputController::ControlInput(void)
{
  boolean fAdjustLegPositions = false;
  // Then try to receive a packet of information from the PS2.
  // Then try to receive a packet of information from the PS2.
  ps2x.read_gamepad();          //read controller and set large motor to spin at 'vibrate' speed

    // Wish the library had a valid way to verify that the read_gamepad succeeded... Will hack for now
  if ((ps2x.Analog(1) & 0xf0) == 0x70) {
#ifdef DBGSerial
#ifdef DEBUG_PS2_INPUT
	if (g_fDebugOutput) {
		DBGSerial.print("PS2 Input: ");
		DBGSerial.print(ps2x.ButtonDataByte(), HEX);
		DBGSerial.print(":");
		DBGSerial.print(ps2x.Analog(PSS_LX), DEC);
		DBGSerial.print(" ");
		DBGSerial.print(ps2x.Analog(PSS_LY), DEC);
		DBGSerial.print(" ");
		DBGSerial.print(ps2x.Analog(PSS_RX), DEC);
		DBGSerial.print(" ");
		DBGSerial.println(ps2x.Analog(PSS_RY), DEC);
	}
#endif
#endif


    // In an analog mode so should be OK...
    g_sPS2ErrorCnt = 0;    // clear out error count...

    if (ps2x.ButtonPressed(PSB_START)) {// OK lets press start button
      if (g_InControlState.fRobotOn) {
        PS2TurnRobotOff();
      } 
      else {
        //Turn on
        g_InControlState.fRobotOn = 1;
        fAdjustLegPositions = true;
      }
    }

    if (g_InControlState.fRobotOn) {
      // [SWITCH MODES]

      //Translate mode
      if (ps2x.ButtonPressed(PSB_L1)) {// L1 Button Test
        MSound( 1, 50, 2000);  
        if (ControlMode != TRANSLATEMODE )
          ControlMode = TRANSLATEMODE;
        else {
            ControlMode = SINGLELEGMODE;
        }
      }

      //Rotate mode
      if (ps2x.ButtonPressed(PSB_L2)) {    // L2 Button Test
        MSound( 1, 50, 2000);
        if (ControlMode != ROTATEMODE)
          ControlMode = ROTATEMODE;
      }


      //[Common functions]
      //Switch Balance mode on/off 
      if (ps2x.ButtonPressed(PSB_SQUARE)) { // Square Button Test
        g_InControlState.BalanceMode = !g_InControlState.BalanceMode;
        if (g_InControlState.BalanceMode) {
          MSound(1, 250, 1500); 
        } 
        else {
          MSound( 2, 100, 2000, 50, 4000);
        }
      }

      //Stand up, sit down  
      if (ps2x.ButtonPressed(PSB_TRIANGLE)) { // Triangle - Button Test
        if (g_BodyYOffset>0) 
          g_BodyYOffset = 0;
        else
          g_BodyYOffset = 35;
        fAdjustLegPositions = true;
      }

      if (ps2x.ButtonPressed(PSB_PAD_UP)) {// D-Up - Button Test
        g_BodyYOffset += 10;

        // And see if the legs should adjust...
        fAdjustLegPositions = true;
        if (g_BodyYOffset > MAX_BODY_Y)
          g_BodyYOffset = MAX_BODY_Y;
      }

      if (ps2x.ButtonPressed(PSB_PAD_DOWN) && g_BodyYOffset) {// D-Down - Button Test
        if (g_BodyYOffset > 10)
          g_BodyYOffset -= 10;
        else
          g_BodyYOffset = 0;      // constrain don't go less than zero.

        // And see if the legs should adjust...
        fAdjustLegPositions = true;
      }

      if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) { // D-Right - Button Test
        if (g_InControlState.SpeedControl>0) {
          g_InControlState.SpeedControl = g_InControlState.SpeedControl - 50;
          MSound( 1, 50, 2000);  
        }
      }

      if (ps2x.ButtonPressed(PSB_PAD_LEFT)) { // D-Left - Button Test
        if (g_InControlState.SpeedControl<2000 ) {
          g_InControlState.SpeedControl = g_InControlState.SpeedControl + 50;
          MSound( 1, 50, 2000); 
        }
      }
      
      // We are optionally going to allow the user to modify the Initial Leg positions, when they
      // press the L3 button.
      byte lx = ps2x.Analog(PSS_LX);
      byte ly = ps2x.Analog(PSS_LY);

      //[Walk functions]
      if (ControlMode == WALKMODE) {
        //Switch gates
        if (ps2x.ButtonPressed(PSB_SELECT)            // Select Button Test
        && abs(g_InControlState.TravelLength.x)<cTravelDeadZone //No movement
        && abs(g_InControlState.TravelLength.z)<cTravelDeadZone 
          && abs(g_InControlState.TravelLength.y*2)<cTravelDeadZone  ) {
          g_InControlState.GaitType = g_InControlState.GaitType+1;                    // Go to the next gait...
          if (g_InControlState.GaitType<NUM_GAITS) {                 // Make sure we did not exceed number of gaits...
            MSound( 1, 50, 2000); 
          } 
          else {
            MSound(2, 50, 2000, 50, 2250); 
            g_InControlState.GaitType = 0;
          }
          GaitSelect();
        }

        //Double leg lift height
        if (ps2x.ButtonPressed(PSB_R1)) { // R1 Button Test
          MSound( 1, 50, 2000); 
          DoubleHeightOn = !DoubleHeightOn;
          if (DoubleHeightOn)
            g_InControlState.LegLiftHeight = 80;
          else
            g_InControlState.LegLiftHeight = 50;
        }

        //Double Travel Length
        if (ps2x.ButtonPressed(PSB_R2)) {// R2 Button Test
          MSound(1, 50, 2000); 
          DoubleTravelOn = !DoubleTravelOn;
        }

        // Switch between Walk method 1 && Walk method 2
        if (ps2x.ButtonPressed(PSB_R3)) { // R3 Button Test
          MSound(1, 50, 2000); 
          WalkMethod = !WalkMethod;
        }

        //Walking
        if (WalkMethod)  //(Walk Methode) 
          g_InControlState.TravelLength.z = (ps2x.Analog(PSS_RY)-128); //Right Stick Up/Down  

        else {
          g_InControlState.TravelLength.x = -(lx - 128);
          g_InControlState.TravelLength.z = (ly - 128);
        }

        if (!DoubleTravelOn) {  //(Double travel length)
          g_InControlState.TravelLength.x = g_InControlState.TravelLength.x/2;
          g_InControlState.TravelLength.z = g_InControlState.TravelLength.z/2;
        }

        g_InControlState.TravelLength.y = -(ps2x.Analog(PSS_RX) - 128)/4; //Right Stick Left/Right 
      }

      //[Translate functions]
      g_BodyYShift = 0;
      if (ControlMode == TRANSLATEMODE) {
        g_InControlState.BodyPos.x = (lx - 128)/2;
        g_InControlState.BodyPos.z = -(ly - 128)/3;
        g_InControlState.BodyRot1.y = (ps2x.Analog(PSS_RX) - 128)*2;
        g_BodyYShift = (-(ps2x.Analog(PSS_RY) - 128)/2);
      }

      //[Rotate functions]
      if (ControlMode == ROTATEMODE) {
        g_InControlState.BodyRot1.x = (ly - 128);
        g_InControlState.BodyRot1.y = (ps2x.Analog(PSS_RX) - 128)*2;
        g_InControlState.BodyRot1.z = (lx - 128);
        g_BodyYShift = (-(ps2x.Analog(PSS_RY) - 128)/2);
      }

      //Calculate walking time delay
      g_InControlState.InputTimeDelay = 128 - max(max(abs(lx - 128), abs(ly - 128)), abs(ps2x.Analog(PSS_RX) - 128));
    }

    //Calculate g_InControlState.BodyPos.y
    g_InControlState.BodyPos.y = min(max(g_BodyYOffset + g_BodyYShift,  0), MAX_BODY_Y);
    
    if (fAdjustLegPositions)
      AdjustLegPositionsToBodyHeight();    // Put main workings into main program file
  } 
  else {
    // We may have lost the PS2... See what we can do to recover...
    if (g_sPS2ErrorCnt < MAXPS2ERRORCNT)
      g_sPS2ErrorCnt++;    // Increment the error count and if to many errors, turn off the robot.
    else if (g_InControlState.fRobotOn)
      PS2TurnRobotOff();
    ps2x.reconfig_gamepad();
  }
}

//==============================================================================
// PS2TurnRobotOff - code used couple of places so save a little room...
//==============================================================================
void PS2TurnRobotOff(void)
{
  //Turn off
  g_InControlState.BodyPos.x = 0;
  g_InControlState.BodyPos.y = 0;
  g_InControlState.BodyPos.z = 0;
  g_InControlState.BodyRot1.x = 0;
  g_InControlState.BodyRot1.y = 0;
  g_InControlState.BodyRot1.z = 0;
  g_InControlState.TravelLength.x = 0;
  g_InControlState.TravelLength.z = 0;
  g_InControlState.TravelLength.y = 0;
  g_BodyYOffset = 0;
  g_BodyYShift = 0;
#ifdef OPT_SINGLELEG
  g_InControlState.SelectedLeg = 255;
#endif  
  g_InControlState.fRobotOn = 0;
  AdjustLegPositionsToBodyHeight();    // Put main workings into main program file
}
