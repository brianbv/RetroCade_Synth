/*
 *  QueueList.h
 *
 *  Library implementing a generic, dynamic queue (linked list version).
 *
 *  ---
 *
 *  Copyright (C) 2010  Efstathios Chatzikyriakidis (contact@efxa.org)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  ---
 *
 *  Version 1.0
 *
 *    2010-09-28  Efstathios Chatzikyriakidis  <contact@efxa.org>
 *
 *      - added exit(), blink(): error reporting and handling methods.
 *
 *    2010-09-25  Alexander Brevig  <alexanderbrevig@gmail.com>
 *
 *      - added setPrinter(): indirectly reference a Serial object.
 *
 *    2010-09-20  Efstathios Chatzikyriakidis  <contact@efxa.org>
 *
 *      - initial release of the library.
 *
 *  ---
 */
 
 /* RetroCade Modifications (Brian Vogel):
  *      
  * added functionality for iteration, removing values, clearing the entire list and methods so queuelist could be used as a stack.  
  * 
  *  there seems to be some instability surrouding the dynamic memory allocations.
  */

// header defining the interface of the source.
#ifndef _QUEUELIST_H
#define _QUEUELIST_H

// include Arduino basic header.
#include <Arduino.h>
#include "Config.h"
#include "ListBase.h"
  

template<typename T>
class QueueList : public ListBase<T> {

  public:
	//list
	void pushHead (const T  value);
	//void push(const T value);
	T push();

    T pop (); //removes and returns the head value
	bool popValue (T value);
	T swap();  //swaps the first and last value
    T peek () const; //returns the head value

	//iteration
	T peekNext() ;
	T peekCurrent ();
	T popCurrent ();
	bool next ();
	void start();
	void dumpList();

  protected:
	void push (ListNode<T>*  nextNode);
	void pushHead (ListNode<T>*  nextNode);
	 
  private:
	ListNode<T>* iterator;   
	ListNode<T>* previous;
};


// push an value to the tail of the queue
template<typename T>
void QueueList<T>::push (ListNode<T>*  nextNode) 
{
  ListNode<T>* t = this->tail;

  // create a new node for the tail.
  this->tail = nextNode;
  this->tail->next = NULL;

  // check if the queue is empty.
  if (0==this->size)
    this->head = this->tail;
  else
    t->next = this->tail;

  this->size++;
}

template<typename T>
void QueueList<T>::pushHead (const T value) 
{
	ListNode<T>* poolvalue =  this->getNode();
	poolvalue->value = value;
	pushHead(poolvalue);
}

template<typename T>
void QueueList<T>::pushHead (ListNode<T>*  newNode) 
{
  newNode->next = this->head;
  this->head = newNode;
   
  if (0==this->size) this->tail = this->head;

  this->size++;
}


template<typename T>
void QueueList<T>::start() {
	iterator = this->head;
}

// pop an value from the queue.
template<typename T>
bool QueueList<T>::next () {
  
  bool result = true;
  

  if (this->size<=1) 
  {
	result = false;
  }
  else if (NULL!=iterator-> next)
  {
	previous = iterator;
	iterator = iterator->next;
  }
  else
  {
	result = false;
  }
  
  return result; 
}

template<typename T>
T QueueList<T>::peekNext () {

	if (NULL!=iterator && 
	    NULL !=iterator->next)
		return iterator->next->value;
		
	return NULL;
}

template<typename T>
T QueueList<T>::peekCurrent () {
	return iterator->value;
}

template<typename T>
T QueueList<T>::popCurrent () 
{
	//Serial.println("popCurrent () ");
	//dumpList();

	T result = NULL;

	//unlink the current node
	if (1 == this->size || iterator==this->head)
	{
		result= pop();
	}
	else
	{
		result = iterator->value;

		if (iterator==this->tail)
		{
			this->tail=previous;
			this->tail->next = NULL;
			//Serial.println("iterator==tail");
		}
		else if (NULL!=previous)
		{
			previous->next = iterator->next;
			//Serial.println("(NULL!=previous)");

		}

		this->releaseNode(iterator);
		iterator = NULL;
		previous = NULL;
		this->size--;
		//Serial.println("-- stuff is reset now --");
	}

	//dumpList();
 
	return result;
}


template<typename T>
void QueueList<T>::dumpList () 
{
	#ifdef DEBUG

	Serial.println();
	Serial.print("QueueList (0x");
	Serial.print((int) this, HEX);
	Serial.print(") - ");
	Serial.print(allocs);
	Serial.print(',');
    Serial.println(deallocs);

	Serial.println("\thead\ttail\titerator\tprevios\tsize\t");
	Serial.print('\t');
	Serial.print((int) head,HEX);
	Serial.print('\t');
	Serial.print((int) tail,HEX);
	Serial.print('\t');
	Serial.print((int) iterator,HEX);
	Serial.print('\t');
	Serial.print((int) previous,HEX);
	Serial.print('\t');
	Serial.println(size);
	Serial.println("\tvalues:");

	ListNode<T>* value = head;

	do
	{
		Serial.print("\t");
		Serial.println((int)value,HEX);
		value= value->next;
	} while(value!=NULL);

	Serial.println("\n\n");

	#endif
}

template<typename T>
bool QueueList<T>::popValue (T value)
{
	T result;
	start();

	do
	{
	    if (value == iterator->value)
		{
			result=popCurrent();
		}
	} while(next());

	return result==value;
}



template<typename T>
T QueueList<T>::swap()
{
	  //unlink the head and make the 2nd node the head
	  ListNode<T>* node = this->head;  
	  this->head=node->next;   

	  //now move the old head to the tail
	  this->tail->next=node;
	  this->tail = node;
	  this->tail->next = NULL;
	  
	  return node->value;

}
 
// pop an value from the queue.
template<typename T>
T QueueList<T>::pop () {
  // check if the queue is empty.
  if (0==this->size) return NULL;

  // get the value of the head node.
  T value = this->head->value;

  // remove only the head node.
  ListNode<T>* t = this->head->next; 
  releaseNode(this->head); 
  this->head = t;
  
  // decrease the values.
  this->size--;

  // return the value.
  return value;
}

// get an value from the queue.
template<typename T>
T QueueList<T>::peek () const 
{
  if (0==this->size) return NULL;
  return this->head->value;
}



 

#endif // _QUEUELIST_H
