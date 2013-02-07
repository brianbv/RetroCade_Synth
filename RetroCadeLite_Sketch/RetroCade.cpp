/*!
 *  @file		RetroCade.h
 *  Project		RetroCade Library
 *	@brief		RetroCade Library for the ZPUino
 *	Version		1.0
 *  @author		Jack Gassett 
 *	@date		9/11/12
 *  License		GPL
 */

#include "RetroCade.h"
#include "LiquidCrystal.h"
#include "binary.h"

#include "Patch.h"
#include "Menu.h"
#include "spaceinvaders.h"
 
#define FREQ 17000          //Freq for modplayer 
#define TIMEOUTMAX 30000    //Timeout for joystick 
#define INVADERSTIMERMAX 9000    //Timeout for LCD 



char smallfsModTrack[] = "track1.mod";
//char smallfsYmTrack[] = "track1.mod";

#define MAX_PATCHES 9          

 
LiquidCrystal lcd(WING_B_10, WING_B_9, WING_B_8, WING_B_7, WING_B_6, WING_B_5, WING_B_4);


void RETROCADE::setup()
{
	setupMegaWing();

	 ///Give some volume
	ym2149.V1.setVolume(11);
	ym2149.V2.setVolume(11);
    ym2149.V3.setVolume(11);   
    sid.setVolume(15);

    //Select an instrument for each SID Voice.
    sid.V1.setInstrument(0,0,15,0,0,0,0,1,0); //Calliope
    sid.V2.setInstrument(12,0,12,0,0,0,1,0,0); //Accordian
    sid.V3.setInstrument(0,9,0,0,0,1,0,0,512); //Harpsicord

	//init patch with defaults
	currentPatch.setVoices(&sid.V1,&sid.V2,&sid.V3);
    currentPatch.setMode(PATCH_MODE_UNISON);

	//retrocade.modplayer.setup();
	//retrocade.ymplayer.setup(&retrocade.ym2149); 
}


void RETROCADE::setupMegaWing()
{
  activeChannel = 0;
  activeInstrument = 0;
  smallfsActiveTrack = 0;
  timeout = TIMEOUTMAX;
  smallFs = false;
  sdFs = false;
  invadersCurLoc = 0;
  invadersCurSeg = 1;
  invadersTimer = INVADERSTIMERMAX;
  lcdMode = MENU_WELCOME;
  buttonPressed = None;
  

  SIGMADELTACTL=0x3;
  //Move the audio output to the appropriate pins on the Papilio Hardware
  pinMode(AUDIO_J1_L,OUTPUT);
  digitalWrite(AUDIO_J1_L,HIGH);
  //outputPinForFunction(AUDIO_J1_L, IOPIN_SIGMADELTA0);
  outputPinForFunction(AUDIO_J1_L, 8);
  pinModePPS(AUDIO_J1_L, HIGH);

  pinMode(AUDIO_J1_R,OUTPUT);
  digitalWrite(AUDIO_J1_R,HIGH);
  outputPinForFunction(AUDIO_J1_R, 8);
  //outputPinForFunction(AUDIO_J1_R, IOPIN_SIGMADELTA1);
  pinModePPS(AUDIO_J1_R, HIGH);
  
  pinMode(AUDIO_J2_L,OUTPUT);
  digitalWrite(AUDIO_J2_L,HIGH);
  outputPinForFunction(AUDIO_J2_L, 8);
  pinModePPS(AUDIO_J2_L, HIGH);

  pinMode(AUDIO_J2_R,OUTPUT);
  digitalWrite(AUDIO_J2_R,HIGH);
  outputPinForFunction(AUDIO_J2_R, 8);
  pinModePPS(AUDIO_J2_R, HIGH);
  
  //Move the second serial port pin to where we need it, this is for MIDI input.
  pinMode(SERIAL1RXPIN,INPUT);
  inputPinForFunction(SERIAL1RXPIN, 1);
  pinMode(SERIAL1TXPIN,OUTPUT);
  //digitalWrite(SERIAL1TXPIN,HIGH);
  outputPinForFunction(SERIAL1TXPIN, 6);
  pinModePPS(SERIAL1TXPIN, HIGH);
 
   //Start SmallFS
  if (SmallFS.begin()<0) {
	Serial.println("No SmalLFS found.");
  }
  else{
     Serial.println("SmallFS Started.");
     smallFs = true; 
  }
 
  //Setup SD Card
  initSD();
  
  //Setup Joystick
  pinMode(JSELECT, INPUT); 
  pinMode(JUP, INPUT); 
  pinMode(JDOWN, INPUT); 
  pinMode(JLEFT, INPUT); 
  pinMode(JRIGHT, INPUT);  
 
  //Setup LCD
  pinMode(WING_C_14, OUTPUT);     //Set contrast to GND
  digitalWrite(WING_C_14, LOW);   //Set contrast to GND    
 // set up the LCD's number of columns and rows:
 lcd.begin(16,2);
 // clear the LCD screen:
 lcd.clear();
 lcd.setCursor(0,1);
 lcd.print("RetroCade Synth");

 //Setup timer for YM and mod players
  TMR0CTL = 0;
  TMR0CNT = 0;
  TMR0CMP = ((CLK_FREQ/2) / FREQ )- 1;
  TMR0CTL = _BV(TCTLENA)|_BV(TCTLCCM)|_BV(TCTLDIR)|
  	_BV(TCTLCP0) | _BV(TCTLIEN);
  INTRMASK = BIT(INTRLINE_TIMER0); // Enable Timer0 interrupt
  INTRCTL=1;    

}


void RETROCADE::initSD()
{
  int i;
  USPICTL=BIT(SPICP1)|BIT(SPICPOL)|BIT(SPISRE)|BIT(SPIEN)|BIT(SPIBLOCK);
  outputPinForFunction( SDIPIN, IOPIN_USPI_MOSI );
  pinModePPS(SDIPIN,HIGH);
  pinMode(SDIPIN,OUTPUT);

  outputPinForFunction( SCKPIN, IOPIN_USPI_SCK);
  pinModePPS(SCKPIN,HIGH);
  pinMode(SCKPIN,OUTPUT);

  pinModePPS(CSPIN,LOW);
  pinMode(CSPIN,OUTPUT);

  inputPinForFunction( SDOPIN, IOPIN_USPI_MISO );
  pinMode(SDOPIN,INPUT);    

  Serial.println("Starting SD Card");
  
	digitalWrite(CSPIN,HIGH);

	for (i=0;i<51200;i++)
		USPIDATA=0xff;

	digitalWrite(CSPIN,LOW);

	for (i=0;i<51200;i++)
		USPIDATA=0xff;  

  if (!SD.begin(CSPIN)) {
	Serial.println("init failed!");
	Serial.println(SD.errorCode());
        sdFs = false;
  } else {
  	Serial.println("done.");
	sdFs = true;
  }  
  //root = SD.open("/");
}

void RETROCADE::setTimeout()
{
  //Serial.println("In SetTimeout");
  if (timeout!=0)
    timeout--;  
  if (invadersTimer!=0)
    invadersTimer--;      
}

byte RETROCADE::getActiveChannel()
{
  return activeChannel;
}

void RETROCADE::handleJoystick()
{ 
  //Serial.println(timeout);
  if (timeout==0) {
      //Serial.println("In Handle Joystick Timeout");
    if (!digitalRead(JUP)) {
      timeout = TIMEOUTMAX;
      buttonPressed = Up;
    } else if (!digitalRead(JDOWN)) {
      timeout = TIMEOUTMAX;
      buttonPressed = Down;
    } else if (!digitalRead(JRIGHT)) {
      if (lcdMode<LCDMODEMAX)
        lcdMode++;
      timeout = TIMEOUTMAX;
      buttonPressed = Right;
    } else if (!digitalRead(JLEFT)) {
      if (lcdMode!=0)
        lcdMode--;
      timeout = TIMEOUTMAX; 
      buttonPressed = Left;
    } else if (!digitalRead(JSELECT)) {
        timeout = TIMEOUTMAX; 
        buttonPressed = Select;      
    }         
  }  
  
  if (buttonPressed < 5) {
         RETROCADE::setLcdMode(lcdMode);
    }

    buttonPressed = None; 
    
}

void RETROCADE::setLcdMode(byte mode)
{
	lcdMode=mode;

	switch (lcdMode) {
      case MENU_WELCOME:
		mainMenu();		
        break;
      case MENU_CHANNEL:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Channel");

        if (buttonPressed == Up) {
          if (activeChannel<7)
            activeChannel++;  
        }        
        if (buttonPressed == Down) {
          if (activeChannel!=0)
            activeChannel--; 
        }            
        lcd.setCursor(0,1);
        lcd.print(activeChannel);        
        break;
      case MENU_INSTRUMENT:
        instrumentJoystick();
        break;
      case MENU_MODFILE:
        smallfsActiveTrack = 0;
        modFileJoystick();
        break;
      case MENU_SMALLFSMODFILE:
        smallfsModFileJoystick(0);
        break;        
      case MENU_YMFILE:
        ymFileJoystick();
        break; 
      case MENU_SMALLFSYMFILE:   
        smallfsModFileJoystick(1);
        break;           
      case MENU_ABOUT:
        smallfsActiveTrack = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("RetroCade Synth");          
        lcd.setCursor(0,1);
        lcd.print("Version: 1.02L");          
        break;        
      default:
        //return;
        break; 
	}
}

void RETROCADE::ymFileJoystick() 
{
        smallfsActiveTrack = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("YMD File SD Card");
        if (buttonPressed == Down) {
          printFile("YMD");
        }      
        if (buttonPressed == Up) {
          //ymplayer.play(false);
        }            
        if (buttonPressed == Select) {
          //Serial.println("Select Pressed");
          //Serial.println(fileName);
          //ymplayer.loadFile(fileName);
          //ymplayer.play(true);  
          lcd.setCursor(0,1);   
          lcd.print(fileName);
          lcd.print(" ");
          //lcd.print(curFile.size(), DEC);
        }     
}

void RETROCADE::modFileJoystick() 
{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("MOD File SD Card");
        if (buttonPressed == Down) {
          printFile("MOD");
        }      
        if (buttonPressed == Up) {
          //modplayer.play(false);
        }            
        if (buttonPressed == Select) {
          //Serial.println("Select Pressed");
          //Serial.println(fileName);
          //modplayer.loadFile(fileName);
          //modplayer.play(true);  
          lcd.setCursor(0,1);   
          lcd.print(fileName);
          lcd.print(" ");
          //lcd.print(curFile.size(), DEC);
        }    
}

void RETROCADE::changePatch(int patch)
{

}

void RETROCADE::instrumentJoystick() 
{

  if(buttonPressed == Up)
  {
	  activeInstrument = MENU_NEXT(activeInstrument, MAX_PATCHES);
  }
  else if (buttonPressed == Down)
  {
	  activeInstrument = MENU_PREVIOUS(activeInstrument, MAX_PATCHES);
  }

  setActivePatch(activeInstrument);
}

/*  patchNumber: a number ranging from 1 to MAX_PATCHES
 * 
 */
void RETROCADE::setActivePatch(byte patchNumber)
{
	//TODO: read off of SD Card.
	if (patchNumber>=MAX_PATCHES) return;

	activeInstrument = patchNumber; //a little redudant since we already set this in intrument joystick but it's called from the MIDI callbacks.
	
	//TODO: read off SD card
	//(A, D, S, R), (Noise,Square,Sawtooth,Triangle) (PWM)
 
	if (patchNumber == 0)
	{
	sid.V1.setInstrument(0,0,15,0,0,0,0,1,0);  
	sid.V2.setInstrument(12,0,12,0,0,0,1,0,0);   
	sid.V3.setInstrument(0,9,0,0,0,1,0,0,512);  
	currentPatch.setMode(PATCH_MODE_UNISON);
	}
	else if (patchNumber == 1)
	{
	sid.V1.setInstrument(12,5,15,3,0,0,0,1,31); 
	sid.V2.setInstrument(12,5,12,0,1,0,1,0,0); 
	sid.V3.setInstrument(0,9,0,0,5,0,0,0,512);
	currentPatch.setMode(PATCH_MODE_UNISON);
	}
	else if (patchNumber == 2)
	{
	sid.V1.setInstrument(0,   5,   5,  100, 0,1,1,1,   0);
	sid.V2.setInstrument(0,  50,  27,  120, 0,1,1,1,   0); 
	sid.V3.setInstrument(0,  48,  54,    8, 0,1,1,1, 128); 
	currentPatch.setMode(PATCH_MODE_UNISON);
	}
	else if  (patchNumber == 3) //Flute
	{
	sid.V1.setInstrument(0,5,5,3,0,0,0,1,31); 
	sid.V2.setInstrument(0,5,5,3,0,0,0,1,31); 
	sid.V3.setInstrument(0,5,5,3,0,0,0,1,31); 
	currentPatch.setMode(PATCH_MODE_POLY);
	}
	else if  (patchNumber ==4) //Carinet
	{
	sid.V1.setInstrument(3,9,0,3,0,1,0,1,256);  
	sid.V2.setInstrument(3,9,0,3,0,1,0,1,256);  
	sid.V3.setInstrument(3,9,0,3,0,1,0,1,256); 
	currentPatch.setMode(PATCH_MODE_POLY);
	}
	else if  (patchNumber == 5) //Toy organ
	{
	sid.V1.setInstrument(101,10,15,0, 0,1,0,1,1110);  
	sid.V2.setInstrument(101,10,15,0, 0,1,0,1,1110); 
	sid.V3.setInstrument(101,10,15,0, 0,1,0,1,1110);  
	currentPatch.setMode(PATCH_MODE_POLY);
	}
	else if  (patchNumber == 6) //Mellodian
	{
	sid.V1.setInstrument(3,5,5,0,0,1,1,1,32); 
	sid.V2.setInstrument(3,5,5,0,0,1,1,1,32);  
	sid.V3.setInstrument(3,5,15,0,0,1,1,1,32); 
	currentPatch.setMode(PATCH_MODE_POLY);
	}

	if (MENU_INSTRUMENT == lcdMode)
	{
		byte data = currentPatch.getMode();

		//render the LCD
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Program ");
		lcd.print(patchNumber+1);

		lcd.print(" ");
 		if (data==PATCH_MODE_POLY) lcd.print("P");
		else if(data==PATCH_MODE_UNISON) lcd.print("U");
		//else (mode==PATCH_MODE_SPLIT) lcd.print("Split") // not implemented yet

		data = currentPatch.getPolyphony();
		lcd.print(data);

		lcd.setCursor(0,1);
		lcd.print("TODO:PName"); 

	}
}

void RETROCADE::smallfsModFileJoystick(byte type) 
{
//  char smallfsModTrack[] = "track1.mod";
  lcd.clear();
  lcd.setCursor(0,0);
  if (type == 0){
    lcd.print("MOD File SmallFS");
    smallfsModTrack[7] = 0x6d;  //m
    smallfsModTrack[8] = 0x6f;  //o
    smallfsModTrack[9] = 0x64;  //d
  }
  if (type == 1){
    lcd.print("YMD File SmallFS");
    smallfsModTrack[7] = 0x79;  //y
    smallfsModTrack[8] = 0x6d;  //m
    smallfsModTrack[9] = 0x64;  //d
  }
  if (buttonPressed == Down) {          
    if (smallfsActiveTrack<=8) {
      smallfsActiveTrack++;
      smallfsModTrack[5] = (0x30 + smallfsActiveTrack );
    }
    else {
      smallfsActiveTrack=1;
      smallfsModTrack[5] = (0x30 + smallfsActiveTrack );
    }
    lcd.setCursor(0,1); 
    lcd.print(smallfsModTrack);
  }      
  if (buttonPressed == Up) {
//    if (type == 0)    
      //modplayer.play(false);
//    if (type == 1)
      //ymplayer.play(false);  
    lcd.setCursor(0,1);   
    lcd.print("Stop Track");    
  }            
  if (buttonPressed == Select) {
    Serial.println("Select Pressed");
    Serial.println(smallfsModTrack);
    if (type == 0){    
      //modplayer.loadFile(smallfsModTrack);
      //modplayer.play(true);  
    }
    if (type == 1){
      //ymplayer.loadFile(smallfsModTrack);
      //ymplayer.play(true);  
    }    
    lcd.setCursor(0,1);   
    lcd.print(smallfsModTrack);
  }    
}

void RETROCADE::printFile(const char* ext) {
     //curFile =  root.openNextFile();
//     if (! curFile) {
//       Serial.println("No File Opened");
//       root.rewindDirectory();
//       return;
//     }
//     fileName = curFile.name();
//     //Serial.println(fileName);
//     if (fileExtension(fileName,ext,3)) {
//       lcd.setCursor(0,1);   
//       lcd.print(fileName);
//       lcd.print(" ");
//       lcd.print(curFile.size(), DEC); 
//     } 
//     else
//       printFile(ext);
}

void RETROCADE::printDirectory(File dir, int numTabs) {
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
   }
}

boolean RETROCADE::smallFsActive() {
  return smallFs;
}

boolean RETROCADE::sdFsActive() {
  return sdFs;
}

int RETROCADE::fileExtension(const char* name, const char* extension, size_t length)
{
  //Serial.println(extension);
  const char* ldot = strrchr(name, '.');
  if (ldot != NULL)
  {
    if (length == 0)
      length = strlen(extension);
    return strncmp(ldot + 1, extension, length) == 0;
  }
  return 0;
}

void RETROCADE::mainMenu()
{
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("RetroCade Synth");       
	
	
}

void RETROCADE::spaceInvadersLCD(){
/* 
  This Space Invaders alien crawling along the RetroCade LCD was created by JO3RI and adapted to the RetroCade by Jack Gassett.
  Please take a look at JO3RI's website for more cool hacks!
  
  Demo by JO3RI
  
  http://www.JO3RI.be/arduino/arduino-projects/lcd-16x2-demo
 */

#ifndef RETROCADE_EXTRA_LITE
 if (invadersTimer == 0 && lcdMode == MENU_WELCOME)
  {
       if (invadersCurSeg == 10){
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("RetroCade Synth");
        invadersCurSeg = 1;
        if (invadersCurLoc < 13)
          invadersCurLoc+=2;
        else
          invadersCurLoc = 0;         
       }
       else
         invadersCurSeg++;
       invadersTimer = INVADERSTIMERMAX;
       int a=invadersCurLoc;
       int b=a+1;
       int c=a+2;
       int d=a+3;
       int e=a+4;   
      switch (invadersCurSeg) {  
        case 1:
           lcd.createChar(0, charSP12a);
           lcd.createChar(1, charSP7a);
           lcd.createChar(2, charSP2a);
           lcd.createChar(3, charSP0);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)1);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)3);
           lcd.setCursor(e,0);
           lcd.write((byte)3);
          break;
        case 2:
           lcd.createChar(0, charSP0);
           lcd.createChar(1, charSP8b);
           lcd.createChar(2, charSP3b);
           lcd.createChar(3, charSP0);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)1);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)3);
           lcd.setCursor(e,0);
           lcd.write((byte)3);
          break;
        case 3:
           lcd.createChar(0, charSP0);
           lcd.createChar(1, charSP9a);
           lcd.createChar(2, charSP4a);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)1);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)0);
           lcd.setCursor(e,0);
           lcd.write((byte)0);
          break;
        case 4:
           lcd.createChar(0, charSP0);
           lcd.createChar(1, charSP10b);
           lcd.createChar(2, charSP5b);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)1);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)0);
           lcd.setCursor(e,0);
           lcd.write((byte)0);
          break;  
        case 5:
           lcd.createChar(0, charSP0);
           lcd.createChar(1, charSP11a);
           lcd.createChar(2, charSP6a);
           lcd.createChar(3, charSP1a);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)1);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)3);
           lcd.setCursor(e,0);
           lcd.write((byte)0);
          break;   
        case 6:
           lcd.createChar(0, charSP0);
           lcd.createChar(1, charSP12b);
           lcd.createChar(2, charSP7b);
           lcd.createChar(3, charSP2b);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)1);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)3);
           lcd.setCursor(e,0);
           lcd.write((byte)0);
          break;   
        case 7:
           lcd.createChar(0, charSP0);
           lcd.createChar(1, charSP0);
           lcd.createChar(2, charSP8a);
           lcd.createChar(3, charSP3a);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)1);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)3);
           lcd.setCursor(e,0);
           lcd.write((byte)0);
          break;   
        case 8:
           lcd.createChar(0, charSP0);
           lcd.createChar(2, charSP9b);
           lcd.createChar(3, charSP4b);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)0);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)3);
           lcd.setCursor(e,0);
           lcd.write((byte)0);
          break;   
        case 9:
           lcd.createChar(0, charSP0);
           lcd.createChar(2, charSP10a);
           lcd.createChar(3, charSP5a);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)0);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)3);
           lcd.setCursor(e,0);
           lcd.write((byte)0);
          break;  
        case 10:
           lcd.createChar(0, charSP0);
           lcd.createChar(2, charSP11b);
           lcd.createChar(3, charSP6b);
           lcd.createChar(4, charSP1b);
           lcd.setCursor(a,0);
           lcd.write((byte)0);
           lcd.setCursor(b,0);
           lcd.write((byte)0);
           lcd.setCursor(c,0);
           lcd.write((byte)2);
           lcd.setCursor(d,0);
           lcd.write((byte)3);
           lcd.setCursor(e,0);
           lcd.write((byte)4);
          break;        
        default:
          lcd.print("oops");
          break;  
      }
	}
#endif
  
  
}