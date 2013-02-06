/*
Gadget Factory
RetroCade Lite Synthesizer
This example will let you play the YM2149 and SID audio chip integrated into the ZPUino Soft Processor using a MIDI instrument such as a keyboard or guitar.
To learn more about this example visit its home page at the Papilio RetroCade Wiki:
http://retrocade.gadgetfactory.net

To learn more about the Papilio and Gadget Factory products visit:
http://www.GadgetFactory.net

Hardware:
* Connect a RetroCade MegaWing

*******IMPORTANT********************
Be sure to load the ZPUino "RetroCade" variant to the Papilio's SPI Flash before loading this sketch.

created 2012
by Jack Gassett
http://www.gadgetfactory.net

License: GPL

ChangeLog:
1/8/2013        Version 1.01L
        -RetroCade Lite for the Papilio One 500K
                -No SD Card 
                -No ModPlayer
                -No YM Player
                
11/23/2012      Version 1.01
        -RetroCade Sketch
                -YM Player Volume Control
                
11/23/2012      Version 1.0
        -RetroCade Sketch
                -SID Volume CC added.
                -Crawling Space Invaders added to the Welcome LCD screen.
                -Added ability to browse and play YMD and MOD files from the LCD screen.
                -Ring Mod and Sync CC's added.
                -Added YM2149 Noise Frequency.
                -SID V1 Instrument select from LCD.
                -SID Pitch Control
                -HiHat and Kick split keyboard from mod files on MIDI channel 7

11/1/2012	Version .3
	-FlowStone Dashboard
		-Changed from SynthMaker to FlowStone
		-Added the ability to control the RetroCade without a MIDI output, can control over a Com Port instead.
		-Generated an EXE version of the Dashboard. VST will be generated when FlowStone 3.0 is released in November.
		-Zoom Controls added to Dashboard
			-Pressing Up Arrow toggles full screen.
			-While in full screen pressing Left Arrow fills the screen.
			-Pressing Down Arrow returns to normal size.
	-RetroCade Sketch
		-Control the RetroCade from a COM port or MIDI In
		-Play YMdat and Mod files from SD card or SmallFS filesystem.
			-Place a file name track[1-3].mod or track[1-3].ymd onto the SD card to override the files found in smallFS. Must power cycle the board for the files to be found.
			-YM files must be converted to ymd files using the process and tools detailed here: (http://audio.gadgetfactory.net/index.php?n=Main.YM2149Flash)
			-Mod files play out of RamFS filesystem which is located entirely in the 8MB SDRAM.
	-Merged RetroCade Bit File: Shortcut to load RetroCade-.01d-zpuino-1.0-PapilioPro-S6LX9-RetroCade-1.04.bit with the Papilio Loader
		-Changes from RetroCade Sketch are merged into this bit file.
		-Smallfs version of MOD and YM files are merged into this bit file.
		
10/8/2012	Version .2
	-SynthMaker Dashboard
		-Added Midi In and Midi Out Selectors to the front panel.
		-Added SID Volume
		-Fixed CC#'s for all Presets
		-Renamed Presets from Hoppa to User

9/26/2012	Version .1
	-ZPUino IDE: Shortcut to open RetroCade Sketch with the ZPUino IDE.
	-Merged RetroCade Bit File: Shortcut to load RetroCade-.01b-zpuino-1.0-PapilioPro-S6LX9-RetroCade-1.04.bit with the Papilio Loader
		-MOD files, YM2149, SID chips play on both left and right channel now.
		-Code runs from SDRAM providing 8MB of code space!
		-RetroCade Sketch is merged into this bit file.
		-Smallfs version of MOD and YM files are merged into this bit file.
	-Papilio Loader
	-SynthMaker Dashboard: The dashboard requires that Synthmaker be installed. MIDI selections are not moved to the main page yet.

*/

HardwareSerial Serial1(11);   //This is to define Serial1 for the ZPUino.



#include "MIDI.h"          //Be sure to change MIDI.h to user Serial1 instead of Serial
#include "SmallFS.h"
#include <LiquidCrystal.h>
#include <SD.h>
 
#include "RetroCade.h"
#include "RetroCadeMidi.h" //retrocade midi stuff

#undef DO_CHECKS
//#define DEBUG



void setup(){

  Serial.begin(115200);
  Serial1.begin(31250);

  //Setup pins for RetroCade MegaWing and initialize instruments
  retrocade.setup(); 
  InitializeRetroCadeMidi();
}


void _zpu_interrupt()
{
  //Serial.println("in zpu interupt");
  //retrocade.setTimeout();
}




void loop(){
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read(&Serial);
  MIDI.read(&Serial1);  
  retrocade.spaceInvadersLCD();        
  retrocade.handleJoystick();  
  retrocade.setTimeout();  
}



