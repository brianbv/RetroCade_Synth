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
#include "QueueStack.h"

#define PATCH_MODE_UNISON 0x00
#define PATCH_MODE_POLY   0x01
#define PATCH_MODE_SPLIT  0x02

#define MAX_POLYPHONY 3
#define MAX_RUN 10
 
class Patch
{
  

  public:
  typedef void (Patch::*NoteHandler) (int note, bool active);
  typedef void (Patch::*IntHandler) (int param);
  
   Patch();
   void addVoice(Voice* voice);
   void reset();


   //setters
   void setName(const char* name);
   void setMode(byte mode);
   void setNote(int note, bool active);
   void setPitchBend(int amount);   
   void setParameter(byte number, byte value);  //internal parameter number   
   void setVoices(Voice* v1, Voice* v2, Voice* v3);   
   void setVoices(Voice* *voices[MAX_POLYPHONY]);

   //getters
   char* getName();
   byte getMode();
   byte getPolyphony();
   bool notesActive();

protected:
   char instrumentName[17]; //instrument name
   byte patchMode;          //current patch mode
   byte polyphony;          //current voice polyphony
   uint32_t noteState[4];   //a table of bits indicating the state of notes 0-127
   
   ListNode<Voice*> availableVoices[MAX_POLYPHONY]; //available voices
   ListNode<Voice*>* availableVoicePool[MAX_POLYPHONY]; //could allocate dynamically here

   ListNode<byte> activeNotes[MAX_RUN];
   ListNode<byte>* activeNotePool[MAX_RUN];

   QueueStack<byte> activeNoteStack;
   QueueStack<Voice*> voiceQueue;
   
   //NoteHandler _setNote;
   //IntHandler _setPitchBend;
     
   void setNoteState(int note,bool state);
   void setVoiceState(int note,bool state);

   void setNoteSplit(int note, bool active);
   void setNotePoly(int note, bool active);
   void setNoteUnison(int note, bool active);
   
   void setPitchBendSplit(int range);
   void setPitchBendDefault(int range);
 
};

#endif
