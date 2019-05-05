//=============================================================================
//Project Lynxmotion Phoenix
//Description: Phoenix software
//Software version: V2.0
//Date: 29-10-2009
//Programmer: Jeroen Janssen [aka Xan]
//         Kurt Eckhardt(KurtE) converted to C and Arduino
//   Kåre Halvorsen aka Zenta - Makes everything work correctly!     
//
// This version of the Phoenix code was ported over to the Arduino Environement
// and is specifically configured for the Lynxmotion BotBoarduino 
//
//=============================================================================
//
//KNOWN BUGS:
//    - Lots ;)
//NOTES:
// - Requires the Borboarduino and the SSC-32.
// - See Hex_CFG.h to onfigure options and see what pins are used.
// - Install all the provided libraires (those specific versions are required)
// - Update the SSC-32 firmware to its latest version. You can find it at: 
//     http://www.lynxmotion.com/p-395-ssc-32-servo-controller.aspx
//=============================================================================
// Header Files
//=============================================================================

#define DEFINE_HEX_GLOBALS
#if ARDUINO>99
#include <Arduino.h>
#else
#endif
#include <Wire.h>
#include <EEPROM.h>
#include <PS2X_lib.h>

#include <SoftwareSerial.h>
#include "Quad_CFG.h"
#include "Phoenix.h"
#include "Phoenix_Input_PS2.h"
#include "Phoenix_Driver_SSC32.h"
#include "Phoenix_Code.h"









//#ifdef QUADMODE
//#define ADD_GAITS
//#define PYPOSE_GAIT_SPEED 1 //98
////  Speed, Steps, Lifted, Front Down, Lifted Factor, Half Height, On Ground, 
////     Quad extra: COGAngleStart, COGAngleStep, CogRadius, COGCCW
////                      { RR, RF, LR, LF}
//PHOENIXGAIT APG_EXTRA[] = { 
//  {PYPOSE_GAIT_SPEED, 20, 5, 2, 2, 15, 3, 2500, 3600 / 20, 30, false, {11, 16, 1, 6}},          // test 20
//  {PYPOSE_GAIT_SPEED,  4, 2, 1, 2,  2, 1,    0,         0,  0, true,  {3, 1, 1, 3}},  // Amble
//  {PYPOSE_GAIT_SPEED,  6, 3, 2, 2,  3, 2,    0,         0,  0, true,  {1, 4, 4, 1}}  // Smooth Amble 
//};
//
////PHOENIXGAIT APG_EXTRA[] = { 
////  {PYPOSE_GAIT_SPEED, 8, 2, 1, 2, 6, 1, 0, 0,0, true, {7, 1, 3, 5}},   // ripple
////  {PYPOSE_GAIT_SPEED, 12, 2, 1, 2, 10, 1, 0, 0,0, true, {7, 1, 4, 10}},   // ripple
////  {PYPOSE_GAIT_SPEED, 4, 2, 1, 2, 2, 1, 0, 0, 0, true,{3, 1, 1, 3}},  // Amble
////  {PYPOSE_GAIT_SPEED, 6, 3, 2, 2, 3, 2, 0, 0,0, true, {1, 4, 4, 1}}  // Smooth Amble 
////};
//#endif

//  Speed, Steps, Lifted, Front Down, Lifted Factor, Half Height, On Ground, 
//     Quad extra: COGAngleStart, COGAngleStep, CogRadius, COGCCW
//                      { RR,  RF, LR,  LF}
// Example:
//    {DEFAULT_GAIT_SPEED, 16, 3, 2, 2, 12, 3, 2250, 3600/16, 30, true, {5, 9, 1, 13} GATENAME(s_szGN1)},            // Wave 16
//
//Lots of this info is reasonably easy to understand:
//
//    1.  the speed (how long should each step take)
//    2.  how many logical steps are there in the gait. In this case 16.
//    3.  How many steps should each leg be lifted
//    4.  When the leg should be put back down (sort of lifted/2)
//    5.  When leg is up multiple steps, how high should it be lifted on the first step…
//    6.  How many steps is the foot on the ground
//    7.  (Quad extras - This is Xan’s stuff for shifting the balance around in an arc, with relationships of how far, what angle to start off with…\
//    
//    Now the interesting thing about which legs move when. Simple at what step in the sequence does each foot lift, so this defines the order the feet move.
//    In this case, you see it is: LR, RR, RF, LF
//
//If you wish for a gait with different order like: LR, RF, LF, RR, just change these values around… But in that case the circular moving of the COG will not work, so in the Quad support section of the structure, change the TRUE to a FALSE…
