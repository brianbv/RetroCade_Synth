/*!
 *  @file		Patch.h
 *  Project		RetroCade
 *	@brief		RetroCade Library for the ZPUino
 *	Version		1.0
 *  @author		Brian Vogel
 *	@date		2/1/13
 *  License		GPL
 */

#include "Patch.h"

	Patch::Patch()
	{
		//could use MALLOC for this instead

	}
 
   void Patch::setParameter(byte number, byte value){}  //internal parameter number


   void Patch::reset()
   {
		voiceQueue.clear();
		activeNoteStack.clear();

	   //reset pool ptrs	   
		for (int i=0;i<MAX_POLYPHONY;i++)
		{
			availableVoicePool[i]=&availableVoices[i];
		}

		for (int i=0;i<MAX_RUN;i++)
		{
			activeNotePool[i]=&activeNotes[i];
		}

	   voiceQueue.setPool(availableVoicePool, MAX_POLYPHONY);
	   activeNoteStack.setPool(activeNotePool,MAX_RUN);
   }

   char* Patch::getName()
   {
	   return instrumentName;
   }

   void Patch::setName(const char* name)
   {
	   strcpy(instrumentName,name);
   }

   void Patch::setVoices(Voice* v1, Voice* v2, Voice* v3)
   {
		polyphony=0;

		addVoice(v1);
		addVoice(v2);
		addVoice(v3);
   }
   
   void Patch::addVoice(Voice* voice)
   {
		availableVoices[polyphony].value = voice;	
		polyphony++;		
   }
   
 
   byte Patch::getMode() { return patchMode; }

   byte Patch::getPolyphony() {return polyphony; } 

   void Patch::setMode(byte mode)
   {
		patchMode=mode;
		reset();
		
		//switch(patchMode)
		//{
		//	case PATCH_MODE_POLY: 
		//		_setNote = &Patch::setNotePoly;
		//	break;
		//	
		//	case PATCH_MODE_SPLIT: 				
		//		_setNote = &Patch::setNoteSplit;
		//	break;
		//	
		//	case PATCH_MODE_UNISON:
		//	default:
		//		_setNote = &Patch::setNoteUnison;
		//}
		
   }
   /* Note handling */
   void Patch::setNote(int note, boolean active)
   {
		if (note < 0 || note > 127) 
			return;
				
		setNoteState(note,active);			


		switch(patchMode)
		{
			case PATCH_MODE_POLY: 
				setNotePoly(note,active);
			break;
			
			case PATCH_MODE_SPLIT: 				
				setNoteUnison(note,active);
			break;
			
			case PATCH_MODE_UNISON:
			default:
				setNoteUnison(note,active);
		}	
 
		//TODO: this singature is wrong.  it's causing a crash
		//(this->*_setNote)(note,active); 
   }   
   
   void Patch::setPitchBend(int bendRange)
   {
		for(byte i=0;i<polyphony;i++)
		{
			availableVoices[i].value->handlePitchBend(bendRange);
		}
   }
   
   /*  Sets the state of the note (on/off) in the table of notes. 
    *
	*/
   void Patch::setNoteState(int note, boolean active)
   {
		uint32_t noteIndex = note >> 5;            //get the note index (index 0-3) ;
        byte noteBit =  note -  (noteIndex * 32);
        uint32_t bitmask = (uint32_t)(1 << (noteBit)  );

		if (active)
		{
			noteState[noteIndex] |= bitmask;
		}
		else
		{
		    noteState[noteIndex] &= ~bitmask;
		}
   }
   
   
	  
   void Patch::setNoteSplit(int note, boolean active)
   {
   
   }
   
   void Patch::setNotePoly(int note, boolean active)
   {
		Voice* currentVoice = NULL;
		
		if (active && voiceQueue.count() == polyphony)
		{
			//take swaps the first and last items
			currentVoice=voiceQueue.swap();
		}
		else if (active)
		{
		    currentVoice=voiceQueue.push();
		}
		else if (voiceQueue.count() > 0)
		{
		 
			voiceQueue.start();	

			do
			{
			    //voiceQueue.dumpList();
				currentVoice = voiceQueue.peekCurrent();
					
				if (NULL!=currentVoice && currentVoice->getNote() == note)
				{
					voiceQueue.popCurrent();
					break;
				}
				else if (NULL==currentVoice)
				{
					break;
				}
 
			
			}
			while( voiceQueue.next() );
 
		}
		else
		{
			//Serial.println("[ NOTE OFF CALLED: NO ACTIVE VOICES]");
		}


		if (NULL!=currentVoice)
		{
			currentVoice->setNote(note,active);
		}
	 
   }
   
   /*
    *  returns true if any notes are active
	*/
   bool Patch::notesActive()
   {
		return   noteState[2]!=0 ||
				 noteState[1]!=0 ||
				 noteState[3]!=0 ||
				 noteState[0]!=0;
   }
 
   /*
    *  
	*/
   void Patch::setNoteUnison(int note, bool active)
   {   
	 
		bool anyNotesActive = notesActive();


		if (active)
		{
			setVoiceState(note,active);

			if (activeNoteStack.count() >= MAX_RUN)
			{
				activeNoteStack.popTail();
			}

			activeNoteStack.pushHead(note);

		}
		else
		{
			activeNoteStack.popValue(note);

			if (!anyNotesActive)
			{
				setVoiceState(note,active);
			}
			else //if (!previousNoteState)
			{
				note = activeNoteStack.peek();
				active= true;
				setVoiceState(note,active);
			}
		}		 
 
   }

  
   void Patch::setVoiceState(int note,bool active)
   {
	   	for(byte i=0;i<polyphony;i++)
		{
			availableVoices[i].value->setNote(note,active);
		}
   }
 