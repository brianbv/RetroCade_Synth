/*!
 *  @file		RetroCadeMidi.h
 *  Project		RetroCade Midi Library
 *	@brief		RetroCade Library for the ZPUino
 *	Version		1.0
 *  @author		Brian Vogel
 *	@date		2/1/13
 *  License		GPL
 */

#include "MIDI.h"

RETROCADE retrocade;

void HandleControlChange(byte channel, byte number, byte value) {
  
 #ifdef DEBUG
  Serial.print("Change Control Channel: ");
  Serial.println(channel);
  Serial.print("Change Control Number: ");
  Serial.println(number);
  Serial.print("Change Control Value: ");
  Serial.println(value);
 #endif    
  
  //Define which voice responds to each channel
  switch (channel) {  
    case 1:
      retrocade.sid.V1.handleCC(number, value);
      break;
    case 2:
      retrocade.sid.V2.handleCC(number, value);
      break;
    case 3:
      retrocade.sid.V3.handleCC(number, value);
      break;
    case 4:
      retrocade.ym2149.V1.handleCC(number, value);
      break;
    case 5:
      retrocade.ym2149.V2.handleCC(number, value);
      break;
    case 6:
      retrocade.ym2149.V3.handleCC(number, value);
      break;      
    default:
      //return;
      break;       
  }  
 
  // These are active no matter which channel is selected.
  switch (number) {  
    case 9:
      if (value == 127)
        //retrocade.modplayer.loadFile("track1.mod");
      //retrocade.modplayer.play(value);
      break;
    case 10:
      if (value == 127)
        //retrocade.ymplayer.loadFile("track1.ymd");
      //retrocade.ymplayer.play(value);
      break;      
    case 11:
      if (value == 127)
        //retrocade.modplayer.loadFile("track2.mod");
      //retrocade.modplayer.play(value);
      break;
    case 12:
      if (value == 127)
        //retrocade.ymplayer.loadFile("track2.ymd");
      //retrocade.ymplayer.play(value);
      break;        
    case 13:
      if (value == 127)
        //retrocade.modplayer.loadFile("track3.mod");
      //retrocade.modplayer.play(value);
      break;  
    case 14:
      if (value == 127)
        //retrocade.ymplayer.loadFile("track3.ymd");
      //retrocade.ymplayer.play(value);
      break;         
    case 84:
      //retrocade.modplayer.volume(value <<1);
      break;    
    case 86:
      retrocade.sid.setVolume(value/8);
      break;     
    case 117:
      //retrocade.modplayer.play(!value);
      //retrocade.ymplayer.play(!value);
      break;      
    default:
      return;
      break;       
  }    

}


///byte attack, byte decay, byte sustain, byte rel, bool noise, bool square, bool sawtooth, bool triangle, int pwm)
void HandleProgramChange(byte channel, byte program)
{
  Serial.println(program);

  if (program == 1)
  {
    retrocade.sid.V1.setInstrument(0,0,15,0,0,0,0,1,0); //Calliope
    retrocade.sid.V2.setInstrument(12,0,12,0,0,0,1,0,0); //Accordian
    retrocade.sid.V3.setInstrument(0,9,0,0,0,1,0,0,512); //Harpsicord
    retrocade.currentPatch.setMode(PATCH_MODE_UNISON);
  }
  else if (program == 2)
  {
    retrocade.sid.V1.setInstrument(12,5,15,3,0,0,0,1,31); //Calliope
    retrocade.sid.V2.setInstrument(12,5,12,0,1,0,1,0,0); //Accordian
    retrocade.sid.V3.setInstrument(0,9,0,0,5,0,0,0,512); //Harpsicord
    retrocade.currentPatch.setMode(PATCH_MODE_UNISON);
  }
   else if  (program == 3)
  {
    retrocade.sid.V1.setInstrument(12,5,15,3,0,0,0,1,31); //Calliope
    retrocade.sid.V1.setInstrument(12,5,15,3,0,0,0,1,31); //Calliope
    retrocade.sid.V1.setInstrument(12,5,15,3,0,0,0,1,31); //Calliope
    retrocade.currentPatch.setMode(PATCH_MODE_POLY);
  }
   else if  (program == 4)
  {
     retrocade.sid.V3.setInstrument(3,9,0,3,0,1,0,1,256); //Harpsicord
     retrocade.sid.V3.setInstrument(3,9,0,3,0,1,0,1,256); //Harpsicord
    retrocade.sid.V3.setInstrument(3,9,0,3,0,1,0,1,256); //Harpsicord
    retrocade.currentPatch.setMode(PATCH_MODE_POLY);
  }
   else if  (program == 5)
  {
    retrocade.sid.V1.setInstrument(101,10,15,0, 0,1,0,1,1110); //Calliope
    retrocade.sid.V1.setInstrument(101,10,15,0, 0,1,0,1,1110); //Calliope
    retrocade.sid.V1.setInstrument(101,10,15,0, 0,1,0,1,1110); //Calliope
    retrocade.currentPatch.setMode(PATCH_MODE_POLY);
  }
   else if  (program == 6)
  {
    retrocade.sid.V1.setInstrument(3,5,5,0,0,1,1,1,32); //Calliope
    retrocade.sid.V1.setInstrument(3,5,5,0,0,1,1,1,32); //Calliope
    retrocade.sid.V1.setInstrument(3,5,15,0,0,1,1,1,32); //Calliope
    retrocade.currentPatch.setMode(PATCH_MODE_POLY);
  }
}

void HandlePitchBend(byte channel, int bend) { 
  #ifdef DEBUG
  Serial.print("Pitch Bend Received: ");
  Serial.println(channel);  
  Serial.println(bend); 
  #endif  
  byte activeChannel = retrocade.getActiveChannel();
  if ( activeChannel != 0 )
    channel = activeChannel;
  
  retrocade.currentPatch.setPitchBend(bend);

}

void HandleNoteOn(byte channel, byte pitch, byte velocity) 
{
	retrocade.currentPatch.setNote(pitch,true);
}

void HandleNoteOff(byte channel, byte pitch, byte velocity) 
{
	retrocade.currentPatch.setNote(pitch,false);
}


void InitializeRetroCadeMidi()
{
	  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Connect the HandleNoteOn function to the library, so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(HandleNoteOn); // Put only the name of the function
  MIDI.setHandleControlChange(HandleControlChange); // Put only the name of the function
  MIDI.setHandleNoteOff(HandleNoteOff); // Put only the name of the function
  MIDI.setHandleProgramChange(HandleProgramChange); // Put only the name of the function
  MIDI.setHandlePitchBend(HandlePitchBend); // Put only the name of the function
}