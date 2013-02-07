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

 
   void Patch::setParameter(byte number, byte value){}  //internal parameter number

   void Patch::reset()
   {
	   availableVoiceQueue.clear();
	   activeVoiceQueue.clear();

	   for (int i=0;i<MAX_POLYPHONY;i++)
	   {
		   voiceItems[i].voice= NULL;
	   }
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
		voiceItems[polyphony].voice= voice;
		voiceItems[polyphony].active= false;
		voiceItems[polyphony].note = -1;
		
		availableVoiceQueue.push( &voiceItems[polyphony] );
		polyphony++;		
   }
   
 
   byte Patch::getMode() { return patchMode; }

   byte Patch::getPolyphony() {return polyphony; } 

   void Patch::setMode(byte mode)
   {
		patchMode=mode;
		
		//memset(&noteState,0,32);
		
		switch(patchMode)
		{
			case PATCH_MODE_POLY: 
				_setNote = &Patch::setNotePoly;
			break;
			
			case PATCH_MODE_SPLIT: 				
				_setNote = &Patch::setNoteSplit;
			break;
			
			case PATCH_MODE_UNISON:
			default:
				_setNote = &Patch::setNoteUnison;
		}
		
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
			voiceItems[i].voice->handlePitchBend(bendRange);
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
		VoiceItem* currentVoice = NULL;
		
		//Serial.println("\nsetNotePoly()");
		//activeVoiceQueue.dumpList();

		if (active && activeVoiceQueue.count() == polyphony)
		{
			//take swaps the first and last items
			currentVoice=activeVoiceQueue.swap();
		}
		else if (active)
		{
		    currentVoice = availableVoiceQueue.pop();
		    activeVoiceQueue.push(currentVoice);
		}
		else if (activeVoiceQueue.count() > 0)
		{
		 
			activeVoiceQueue.start();	

			do
			{
			    //activeVoiceQueue.dumpList();
				currentVoice = activeVoiceQueue.peekCurrent();
					
				if (NULL!=currentVoice && currentVoice->note == note)
				{
					activeVoiceQueue.popCurrent();
					availableVoiceQueue.push(currentVoice);
					break;
				}
				else if (NULL==currentVoice)
				{
					break;
				}
 
			
			}
			while( activeVoiceQueue.next() );
 
			//Serial.println("== AFTER ==");
			//activeVoiceQueue.dumpList();
 
		}
		else
		{
			Serial.println("[ NOTE OFF CALLED: NO ACTIVE VOICES]");
		}


		//activeVoiceQueue.dumpList();
		//Serial.print("Active\tNonactive: ");
		//Serial.println("---\t---");
		//Serial.print(activeVoiceQueue.count());
		//Serial.print('\t');
		//Serial.print(availableVoiceQueue.count());
		//Serial.println("\n");

		if (NULL!=currentVoice)
		{
			currentVoice->note = note;
			currentVoice->voice->setNote(note,active);
			//Serial.print("-- finished set note:");
			//Serial.println(currentVoice->note);
		}
	 
   }
   
   /*
    *  returns true if any notes are active
	*/
   bool Patch::notesActive()
   {
		return   noteState[2]==0 ||
				 noteState[1]==0 ||
				 noteState[3]==0 ||
				 noteState[0]==0;
   }
 
   /*
    *  
	*/
   void Patch::setNoteUnison(int note, boolean active)
   {    
   
   		if (active || 
		    (!active &&  noteState[2]==0 &&
						 noteState[1]==0 &&
						 noteState[3]==0 &&
						 noteState[0]==0))
		{
	 
   
			for(byte i=0;i<polyphony;i++)
			{
				voiceItems[i].active=active;
				voiceItems[i].voice->setNote(note,active);
			}
		}
   }
  

 