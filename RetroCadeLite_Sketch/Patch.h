/*!
 *  @file		Patch.h
 *  Project		RetroCade
 *	@brief		RetroCade Library for the ZPUino
 *	Version		1.0
 *  @author		Brian Vogel
 *	@date		2/1/13
 *  License		GPL
 */

#ifndef LIB_Patch_H_
#define LIB_Patch_H_

#include "Voice.h"
#include "QueueList.h"

#define PATCH_MODE_UNISON 0x00
#define PATCH_MODE_POLY 0x01
#define PATCH_MODE_SPLIT 0x02

#define MAX_POLYPHONY 3

 
struct VoiceItem
{
  Voice* voice;
  boolean active;
  short note;
};

//typedef  int (Patch::*FredMemFn)(int i, double d);


class Patch
{
  

  public:
  typedef void (Patch::*NoteHandler) (int note, boolean active);
  typedef void (Patch::*IntHandler) (int param);
  
   void addVoice(Voice* voice);
   void reset();

   void setName(const char* name);
   void setMode(byte mode);
   void setNote(int note, boolean active);
   void setPitchBend(int amount);   
   void setParameter(byte number, byte value);  //internal parameter number   
   void setVoices(Voice* v1, Voice* v2, Voice* v3);   
   void setVoices(Voice* *voices[MAX_POLYPHONY]);

   byte getMode();
   byte getPolyphony();
   bool notesActive();
 protected:
   char instrumentName[17]; //instrument name
   byte patchMode;          //current patch mode
   byte polyphony;          //current voice polyphony
   uint32_t noteState[4];   //a table of bits indicating the state of notes 0-127
   
   VoiceItem voiceItems[MAX_POLYPHONY];    //available voices
   QueueList<VoiceItem*> activeVoiceQueue;
   QueueList<VoiceItem*> availableVoiceQueue;
   
   
   NoteHandler _setNote;
   IntHandler _setPitchBend;
     
   void setNoteState(int note, boolean state);

   void setNoteSplit(int note, boolean active);
   void setNotePoly(int note, boolean active);
   void setNoteUnison(int note, boolean active);
   
   void setPitchBendSplit(int range);
   void setPitchBendDefault(int range);
 
};

#endif
