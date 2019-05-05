//====================================================================
//Project Lynxmotion Phoenix
//
// Servo Driver - This version is setup to use the SSC-32 to control
// the servos.
//====================================================================

//Servo Pin numbers - May be SSC-32 or actual pins on main controller, depending on configuration.
#ifdef QUADMODE
const byte cCoxaPin[] PROGMEM = {
  cRRCoxaPin,  cRFCoxaPin,  cLRCoxaPin,  cLFCoxaPin};
const byte cFemurPin[] PROGMEM = {
  cRRFemurPin, cRFFemurPin, cLRFemurPin, cLFFemurPin};
const byte cTibiaPin[] PROGMEM = {
  cRRTibiaPin, cRFTibiaPin, cLRTibiaPin, cLFTibiaPin};
#ifdef c4DOF
const byte cTarsPin[] PROGMEM = {
  cRRTarsPin, cRFTarsPin, cLRTarsPin, cLFTarsPin};
#endif
#else
const byte cCoxaPin[] PROGMEM = {
  cRRCoxaPin,  cRMCoxaPin,  cRFCoxaPin,  cLRCoxaPin,  cLMCoxaPin,  cLFCoxaPin};
const byte cFemurPin[] PROGMEM = {
  cRRFemurPin, cRMFemurPin, cRFFemurPin, cLRFemurPin, cLMFemurPin, cLFFemurPin};
const byte cTibiaPin[] PROGMEM = {
  cRRTibiaPin, cRMTibiaPin, cRFTibiaPin, cLRTibiaPin, cLMTibiaPin, cLFTibiaPin};
#ifdef c4DOF
const byte cTarsPin[] PROGMEM = {
  cRRTarsPin, cRMTarsPin, cRFTarsPin, cLRTarsPin, cLMTarsPin, cLFTarsPin};
#endif
#endif


// Add support for running on non-mega Arduino boards as well.
#ifdef __AVR__
#if not defined(UBRR1H)
#if cSSC_IN == 0
#define SSCSerial Serial
#else
SoftwareSerial SSCSerial(cSSC_IN, cSSC_OUT);
#endif    
#endif
#endif

//=============================================================================
// Global - Local to this file only...
//=============================================================================

// definition of some helper functions
extern int SSCRead (byte* pb, int cb, word wTimeout, word wEOL);


//--------------------------------------------------------------------
//Init
//--------------------------------------------------------------------
void ServoDriver::Init(void) {
  SSCSerial.begin(cSSC_BAUD);

  // Lets do the check for GP Enabled here...
#ifdef OPT_GPPLAYER
  char abT[4];        // give a nice large buffer.
  byte cbRead;

  _fGPEnabled = false;  // starts off assuming that it is not enabled...
  _fGPActive = false;

#ifdef __AVR__
#if not defined(UBRR1H)
#if cSSC_IN != 0
  SSCSerial.listen();
#endif    
#endif    
#endif
  // Instead of hard checking version numbers instead ask it for
  // status of one of the players.  If we do not get a response...
  // probably does not support 
  SSCSerial.println(F("QPL0"));
  cbRead = SSCRead((byte*)abT, 4, 25000, (word)-1);

#ifdef DBGSerial
  DBGSerial.print(F("Check GP Enable: "));
  DBGSerial.println(cbRead, DEC);
#endif        
  if (cbRead == 4)
    _fGPEnabled = true;  // starts off assuming that it is not enabled...
  else
    MSound (2, 40, 2500, 40, 2500);
#endif
#ifdef cVoltagePin
  // Prime the voltage values...
  for (byte i=0; i < 8; i++)
	GetBatteryVoltage();
	
#endif
}
//--------------------------------------------------------------------
//GetBatteryVoltage - Maybe should try to minimize when this is called
// as it uses the serial port... Maybe only when we are not interpolating 
// or if maybe some minimum time has elapsed...
//--------------------------------------------------------------------

#ifdef cVoltagePin  
#ifndef CVADR1
#define CVADR1      30  // VD Resistor 1 - reduced as only need ratio... 30K and 10K
#define CVADR2      10  // VD Resistor 2
#endif

word  g_awVoltages[8]={
  0,0,0,0,0,0,0,0};
word  g_wVoltageSum = 0;
byte  g_iVoltages = 0;

word ServoDriver::GetBatteryVoltage(void) {
  g_iVoltages = (++g_iVoltages)&0x7;  // setup index to our array...
  g_wVoltageSum -= g_awVoltages[g_iVoltages];
  g_awVoltages[g_iVoltages] = analogRead(cVoltagePin);
  g_wVoltageSum += g_awVoltages[g_iVoltages];

  return ((long)((long)g_wVoltageSum*125*(CVADR1+CVADR2))/(long)(2048*(long)CVADR2));  

}
#endif


//==============================================================================
// Quick and dirty helper function to read so many bytes in from the SSC with a timeout and an end of character marker...
//==============================================================================
int SSCRead (byte* pb, int cb, word wTimeout, word wEOL)
{
  int ich;
  byte* pbIn = pb;
  unsigned long ulTimeLastChar = micros();
  while (cb) {
    while (!SSCSerial.available()) {
      // check for timeout
      if ((word)(micros()-ulTimeLastChar) > wTimeout) {
        return (int)(pb-pbIn);
      }    
    }
    ich = SSCSerial.read();
    *pb++ = (byte)ich;
    cb--;

    if ((word)ich == wEOL)
      break;    // we matched so get out of here.
    ulTimeLastChar = micros();    // update to say we received something
  }

  return (int)(pb-pbIn);
}


//------------------------------------------------------------------------------------------
//[BeginServoUpdate] Does whatever preperation that is needed to starrt a move of our servos
//------------------------------------------------------------------------------------------
void ServoDriver::BeginServoUpdate(void)    // Start the update 
{
}

//------------------------------------------------------------------------------------------
//[OutputServoInfoForLeg] Do the output to the SSC-32 for the servos associated with
//         the Leg number passed in.
//------------------------------------------------------------------------------------------
#define cPwmDiv       991  //old 1059;
#define cPFConst      592  //old 650 ; 900*(1000/cPwmDiv)+cPFConst must always be 1500
// A PWM/deg factor of 10,09 give cPwmDiv = 991 and cPFConst = 592
// For a modified 5645 (to 180 deg travel): cPwmDiv = 1500 and cPFConst = 900.

void ServoDriver::OutputServoInfoForLeg(byte LegIndex, short sCoxaAngle1, short sFemurAngle1, short sTibiaAngle1)
{        
  word    wCoxaSSCV;        // Coxa value in SSC units
  word    wFemurSSCV;        //
  word    wTibiaSSCV;        //

  // The Main code now takes care of the inversion before calling.
  wCoxaSSCV = ((long)(sCoxaAngle1 +900))*1000/cPwmDiv+cPFConst;
  wFemurSSCV = ((long)((long)(sFemurAngle1+900))*1000/cPwmDiv+cPFConst);
  wTibiaSSCV = ((long)(sTibiaAngle1+900))*1000/cPwmDiv+cPFConst;


#ifdef cSSC_BINARYMODE
  SSCSerial.write(pgm_read_byte(&cCoxaPin[LegIndex])  + 0x80);
  SSCSerial.write(wCoxaSSCV >> 8);
  SSCSerial.write(wCoxaSSCV & 0xff);
  SSCSerial.write(pgm_read_byte(&cFemurPin[LegIndex]) + 0x80);
  SSCSerial.write(wFemurSSCV >> 8);
  SSCSerial.write(wFemurSSCV & 0xff);
  SSCSerial.write(pgm_read_byte(&cTibiaPin[LegIndex]) + 0x80);
  SSCSerial.write(wTibiaSSCV >> 8);
  SSCSerial.write(wTibiaSSCV & 0xff);

#else
  SSCSerial.print("#");
  SSCSerial.print(pgm_read_byte(&cCoxaPin[LegIndex]), DEC);
  SSCSerial.print("P");
  SSCSerial.print(wCoxaSSCV, DEC);
  SSCSerial.print("#");
  SSCSerial.print(pgm_read_byte(&cFemurPin[LegIndex]), DEC);
  SSCSerial.print("P");
  SSCSerial.print(wFemurSSCV, DEC);
  SSCSerial.print("#");
  SSCSerial.print(pgm_read_byte(&cTibiaPin[LegIndex]), DEC);
  SSCSerial.print("P");
  SSCSerial.print(wTibiaSSCV, DEC);

#endif        
  g_InputController.AllowControllerInterrupts(true);    // Ok for hserial again...
}


//--------------------------------------------------------------------
//[CommitServoDriver Updates the positions of the servos - This outputs
//         as much of the command as we can without committing it.  This
//         allows us to once the previous update was completed to quickly 
//        get the next command to start
//--------------------------------------------------------------------
void ServoDriver::CommitServoDriver(word wMoveTime)
{
#ifdef cSSC_BINARYMODE
  byte    abOut[3];
#endif

  g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...

#ifdef cSSC_BINARYMODE
  abOut[0] = 0xA1;
  abOut[1] = wMoveTime >> 8;
  abOut[2] = wMoveTime & 0xff;
  SSCSerial.write(abOut, 3);
#else
  //Send <CR>
  SSCSerial.print("T");
  SSCSerial.println(wMoveTime, DEC);
#endif

  g_InputController.AllowControllerInterrupts(true);    

}

//--------------------------------------------------------------------
//[FREE SERVOS] Frees all the servos
//--------------------------------------------------------------------
void ServoDriver::FreeServos(void)
{
  g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...
  for (byte LegIndex = 0; LegIndex < 32; LegIndex++) {
    SSCSerial.print("#");
    SSCSerial.print(LegIndex, DEC);
    SSCSerial.print("P0");
  }
  SSCSerial.print("T200\r");
  g_InputController.AllowControllerInterrupts(true);    
}

//--------------------------------------------------------------------
//Function that gets called from the main loop if the robot is not logically
//     on.  Gives us a chance to play some...
//--------------------------------------------------------------------
void ServoDriver::IdleTime(void)
{
}

#ifdef OPT_TERMINAL_MONITOR  
extern void FindServoOffsets(void);
extern void SSCForwarder(void);

//==============================================================================
// ShowTerminalCommandList: Allow the Terminal monitor to call the servo driver
//      to allow it to display any additional commands it may have.
//==============================================================================
void ServoDriver::ShowTerminalCommandList(void) 
{
#ifdef OPT_FIND_SERVO_OFFSETS
  DBGSerial.println(F("O - Enter Servo offset mode"));
#endif        
#ifdef OPT_SSC_FORWARDER
  DBGSerial.println(F("S - SSC Forwarder"));
#endif        
}

//==============================================================================
// ProcessTerminalCommand: The terminal monitor will call this to see if the
//     command the user entered was one added by the servo driver.
//==============================================================================
boolean ServoDriver::ProcessTerminalCommand(byte *psz, byte bLen)
{
#ifdef OPT_FIND_SERVO_OFFSETS
  if ((bLen == 1) && ((*psz == 'o') || (*psz == 'O'))) {
    FindServoOffsets();
  }
#endif
#ifdef OPT_SSC_FORWARDER
  if ((bLen == 1) && ((*psz == 's') || (*psz == 'S'))) {
    SSCForwarder();
  }
#endif
  return true;	// Currently not using the return value

}

//==============================================================================
// SSC Forwarder - used to allow things like Lynxterm to talk to the SSC-32 
// through the Arduino...  Will see if it is fast enough...
//==============================================================================
#ifdef OPT_SSC_FORWARDER
void  SSCForwarder(void) 
{
  int sChar;
  int sPrevChar; 
  DBGSerial.println("SSC Forwarder mode - Enter $<cr> to exit");

  for (;;) {
    if ((sChar = DBGSerial.read()) != -1) {
      SSCSerial.write(sChar & 0xff);
      if (((sChar == '\n') || (sChar == '\r')) && (sPrevChar == '$'))
        break;    // exit out of the loop
      sPrevChar = sChar;
    }


    if ((sChar = SSCSerial.read()) != -1) {
      DBGSerial.write(sChar & 0xff);
    }
  }
  DBGSerial.println("Exited SSC Forwarder mode");
}
#endif // OPT_SSC_FORWARDER

#endif  //
