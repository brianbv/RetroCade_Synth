/*!
 *  @file		Voice.h
 *  Project		 
 *	@brief		RetroCade Library for the ZPUino
 *	Version		1.0
 *  @author		Brian Vogel
 *	@date		2/1/13
 *  License		GPL
 */

#ifndef LIB_Voice_H_
#define LIB_Voice_H_

#include <inttypes.h> 
#include <zpuino-types.h>
#include <zpuino.h>
#include "Arduino.h"

class Voice
{
  public:
   virtual void setNote(int note, boolean active);
   virtual int getNote();
   virtual void setFreq(int freq);
   virtual void setNoise(boolean active);
   virtual void handleCC(byte number, byte value);
   virtual int getCurrentFreq();
   virtual void handlePitchBend(int bend);

 protected:
	int note;
};

#endif
