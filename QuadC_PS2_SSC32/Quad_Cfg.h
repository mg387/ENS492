//====================================================================
//Project Lynxmotion Phoenix
//Description: 
//    This is the hardware configuration for Kurt's Lynxmotion Quad
//    using CH3-R types of legs.
//  
//    This version of the Configuration file is set up to run on the
//    Lynxmotion BotboardDuino board, which is similar  to the Arduino Duemilanove
//
//    This version of configuration file assumes that the servos will be controlled
//    by a Lynxmotion Servo controller SSC-32 and the user is using a Lynxmotion 
//    PS2 to control the robot.
//
//Date: March 18, 2012
//Programmer: Kurt (aka KurtE)
//
//
//NEW IN V1.0
//   - First Release
//
//====================================================================
#ifndef QUAD_CFG_H
#define QUAD_CFG_H

//#define DEBUG

#ifdef __AVR__
#if defined(UBRR1H)
#define SSCSerial         Serial1
#define XBeeSerial        Serial3 //Serial2
#else
#define XBeeSerial        Serial
#define DontAllowDebug
#endif
#else  // For My Pic32 Mega shield...
#define SSCSerial         Serial1
#define XBeeSerial        Serial3
#endif

//==================================================================================================================================
// Define which input classes we will use. If we wish to use more than one we need to define USEMULTI - This will define a forwarder
//    type implementation, that the Inputcontroller will need to call.  There will be some negotion for which one is in contol.
//
//  If this is not defined, The included Controller should simply implement the InputController Class...
//==================================================================================================================================
#define QUADMODE            // We are building for quad support...
//#define OVERWRITE_GAITS     // testing gaits
//#define USEMULTI
//#define USEXBEE            // only allow to be defined on Megas...
#define USEPS2
//#define USECOMMANDER
//#define USESERIAL

// Do we want Debug Serial Output?
#define DBGSerial Serial
#define DEBUG_PS2_INPUT

// Some configurations will not allow this so if one of them undefine it
#if (defined USEXBEE) || (defined USECOMMANDER)
#ifdef DontAllowDebug
#undef DBGSerial
#endif
#endif

#ifdef USESERIAL
#undef DBGSerial
#endif

#ifdef DBGSerial
#define OPT_TERMINAL_MONITOR  // Only allow this to be defined if we have a debug serial port
#endif

#ifdef OPT_TERMINAL_MONITOR
//#define OPT_SSC_FORWARDER  // only useful if terminal monitor is enabled
//#define OPT_FIND_SERVO_OFFSETS    // Only useful if terminal monitor is enabled
#endif

//#define OPT_GPPLAYER

#define USE_SSC32
//#define	cSSC_BINARYMODE	1			// Define if your SSC-32 card supports binary mode.
#define cSSC_BAUD        38400   //SSC32 BAUD rate

//--------------------------------------------------------------------
// [Debug options]
//#define DEBUG_IOPINS    // used to control if we are going to use IO pins for debug support

//--------------------------------------------------------------------
//[Arduino Uno Pin Numbers]
#define SOUND_PIN    5        // Botboarduino JR pin number
// PS2 controller connections
#define PS2_DAT      8   
#define PS2_CMD      9
#define PS2_SEL      10
#define PS2_CLK      11

#define cSSC_OUT     13      	//Output pin for (SSC32 RX) on BotBoarduino (Yellow)
#define cSSC_IN      12      	//Input pin for (SSC32 TX) on BotBoarduino (Blue)

// XBee was defined to use a hardware Serial port
#define XBEE_BAUD      38400
#define SERIAL_BAUD    38400

// Define Analog pin and minimum voltage that we will allow the servos to run
#define cVoltagePin  0      // Use our Analog pin jumper here...
#define cTurnOffVol  470     // 4.7v
#define cTurnOnVol   520     // 5.5V - optional part to say if voltage goes back up, turn it back on...

//--------------------------------------------------------------------
//[SSC32 Pin Numbers]
#define cRRCoxaPin      0   //Rear Right Horizontal
#define cRRFemurPin     1   //Rear Right Vertical
#define cRRTibiaPin     2   //Rear Right Tibia

#define cRFCoxaPin      8    //Front Right Horizontal
#define cRFFemurPin     9    //Front Right Vertical
#define cRFTibiaPin     10   //Front Right Tibia

#define cLRCoxaPin      16   //Rear Left Horizontal
#define cLRFemurPin     17   //Rear Left Vertical
#define cLRTibiaPin     18   //Rear Left Tibia

#define cLFCoxaPin      24   //Front Left Horizontal
#define cLFFemurPin     25   //Front Left Vertical
#define cLFTibiaPin     26   //Front Left Tibia

//--------------------------------------------------------------------
//[SERVO PULSE INVERSE]
#define cRRCoxaInv      1
#define cRRFemurInv     0
#define cRRTibiaInv     0

#define cRFCoxaInv      1
#define cRFFemurInv     1
#define cRFTibiaInv     1

#define cLRCoxaInv      0
#define cLRFemurInv     1
#define cLRTibiaInv     1

#define cLFCoxaInv      0
#define cLFFemurInv     0
#define cLFTibiaInv     0

//--------------------------------------------------------------------
//[MIN/MAX ANGLES]
#define cRRCoxaMin1     -600      //Mechanical limits of the Right Rear Leg
#define cRRCoxaMax1     600
#define cRRFemurMin1    -1000
#define cRRFemurMax1    750
#define cRRTibiaMin1    -500
#define cRRTibiaMax1    900
//#define cRRTarsMin1     -1300	//4DOF ONLY - In theory the kinematics can reach about -160 deg
//#define cRRTarsMax1	500	//4DOF ONLY - The kinematics will never exceed 23 deg though..

#define cRFCoxaMin1     -600      //Mechanical limits of the Right Front Leg
#define cRFCoxaMax1     600
#define cRFFemurMin1    -1000
#define cRFFemurMax1    750
#define cRFTibiaMin1    -500
#define cRFTibiaMax1    900
//#define cRFTarsMin1     -1300	//4DOF ONLY - In theory the kinematics can reach about -160 deg
//#define cRFTarsMax1	500	//4DOF ONLY - The kinematics will never exceed 23 deg though..

#define cLRCoxaMin1     -600      //Mechanical limits of the Left Rear Leg
#define cLRCoxaMax1     600
#define cLRFemurMin1    -1000
#define cLRFemurMax1    750
#define cLRTibiaMin1    -500
#define cLRTibiaMax1    900
//#define cLRTarsMin1     -1300	//4DOF ONLY - In theory the kinematics can reach about -160 deg
//#define cLRTarsMax1	500	//4DOF ONLY - The kinematics will never exceed 23 deg though..

#define cLFCoxaMin1     -600      //Mechanical limits of the Left Front Leg
#define cLFCoxaMax1     600
#define cLFFemurMin1    -1000
#define cLFFemurMax1    750
#define cLFTibiaMin1    -500
#define cLFTibiaMax1    900
//#define cLFTarsMin1     -1300	//4DOF ONLY - In theory the kinematics can reach about -160 deg
//#define cLFTarsMax1	500	//4DOF ONLY - The kinematics will never exceed 23 deg though..

//--------------------------------------------------------------------
// [LEG DIMENSIONS]
// Universal dimensions for each leg in mm
// Use these universal dimensions if each of the robot's legs has
// the same dimensions.
#define cXXCoxaLength     32    // This is for CH3-R with Type 3 legs
#define cXXFemurLength    75
#define cXXTibiaLength    140
//#define cXXTarsLength     85    // 4DOF only...

// Individual dimensions for each leg in mm
#define cRRCoxaLength     cXXCoxaLength	    //Right Rear leg
#define cRRFemurLength    cXXFemurLength
#define cRRTibiaLength    cXXTibiaLength
//#define cRRTarsLength	  cXXTarsLength	    //4DOF ONLY

#define cRFCoxaLength     cXXCoxaLength	    //Rigth front leg
#define cRFFemurLength    cXXFemurLength
#define cRFTibiaLength    cXXTibiaLength
//#define cRFTarsLength	  cXXTarsLength    //4DOF ONLY

#define cLRCoxaLength     cXXCoxaLength	    //Left Rear leg
#define cLRFemurLength    cXXFemurLength
#define cLRTibiaLength    cXXTibiaLength
//#define cLRTarsLength	  cXXTarsLength    //4DOF ONLY

#define cLFCoxaLength     cXXCoxaLength	    //Left front leg
#define cLFFemurLength    cXXFemurLength
#define cLFTibiaLength    cXXTibiaLength
//#define cLFTarsLength	  cXXTarsLength	    //4DOF ONLY


//--------------------------------------------------------------------
//[BODY DIMENSIONS]
// This first section defines the angle from the center of the body to each of the legs
#define cRRCoxaAngle1   -450   //Default Coxa setup angle, decimals = 1
#define cRFCoxaAngle1    450   //Default Coxa setup angle, decimals = 1
#define cLRCoxaAngle1   -450   //Default Coxa setup angle, decimals = 1
#define cLFCoxaAngle1    450   //Default Coxa setup angle, decimals = 1

// This second section defines the distances between the center of the body to each of the legs
#define cRROffsetX      -60    //Distance X from center of the body to the Right Rear coxa
#define cRROffsetZ       60    //Distance Z from center of the body to the Right Rear coxa
#define cRFOffsetX      -60    //Distance X from center of the body to the Right Front coxa
#define cRFOffsetZ      -60    //Distance Z from center of the body to the Right Front coxa

#define cLROffsetX       60    //Distance X from center of the body to the Left Rear coxa
#define cLROffsetZ       60    //Distance Z from center of the body to the Left Rear coxa
#define cLFOffsetX       60    //Distance X from center of the body to the Left Front coxa
#define cLFOffsetZ      -60    //Distance Z from center of the body to the Left Front coxa

//--------------------------------------------------------------------
//[START POSITIONS FEET]
#define cHexInitXZ   100 
#define CHexInitXZCos60  50        // COS(60) = .5
#define CHexInitXZSin60  87    // sin(60) = .866
#define CHexInitXZ45    71        // Sin and cos(45) .7071
#define CHexInitY 60 //30

#if 1
#define cRRInitPosX     CHexInitXZ45      //Start positions of the Right Rear leg
#define cRRInitPosY     CHexInitY
#define cRRInitPosZ     CHexInitXZ45

#define cRFInitPosX     CHexInitXZ45      //Start positions of the Right Front leg
#define cRFInitPosY     CHexInitY
#define cRFInitPosZ     -CHexInitXZ45

#define cLRInitPosX     CHexInitXZ45      //Start positions of the Left Rear leg
#define cLRInitPosY     CHexInitY
#define cLRInitPosZ     CHexInitXZ45

#define cLFInitPosX     CHexInitXZ45      //Start positions of the Left Front leg
#define cLFInitPosY     CHexInitY
#define cLFInitPosZ     -CHexInitXZ45


#else
#define cRRInitPosX     cHexInitXZ      //Start positions of the Right Rear leg
#define cRRInitPosY     CHexInitY
#define cRRInitPosZ     0

#define cRFInitPosX     cHexInitXZ      //Start positions of the Right Front leg
#define cRFInitPosY     CHexInitY
#define cRFInitPosZ     0

#define cLRInitPosX     CHexInitXZCos60      //Start positions of the Left Rear leg
#define cLRInitPosY     CHexInitY
#define cLRInitPosZ     0

#define cLFInitPosX     cHexInitXZ      //Start positions of the Left Front leg
#define cLFInitPosY     CHexInitY
#define cLFInitPosZ     0
#endif
//--------------------------------------------------------------------
//[Tars factors used in formula to calc Tarsus angle relative to the ground]
//#define cTarsConst	720	//4DOF ONLY
//#define cTarsMulti	2	//4DOF ONLY
//#define cTarsFactorA	70	//4DOF ONLY
//#define cTarsFactorB	60	//4DOF ONLY
//#define cTarsFactorC	50	//4DOF ONLY

#endif
