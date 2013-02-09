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
 *
 *  For the latest version see: http://www.arduino.cc/
 */
 
 /* RetroCade Modifications (Brian Vogel):
  *      
  * added functionality for iteration, removing items, clearing the entire list and methods so queuelist could be used as a stack.  
  * 
  *  there seems to be some instability surrouding the dynamic memory allocations.
  */

// header defining the interface of the source.
#ifndef _QUEUELIST_H
#define _QUEUELIST_H

// include Arduino basic header.
#include <Arduino.h>
#include "Config.h"

	static unsigned int allocs;
	static int deallocs;

// the definition of the queue class.
template<typename T>
class QueueList {

  public:



    // init the queue (constructor).
    QueueList ();

    // clear the queue (destructor).
    ~QueueList ();

    // push an item to the queue.
    void push (const T i);
	void pushHead (const T i);
	void clear();

    // pop an item from the queue.
    T pop ();

	bool popValue (T value);

	//swaps the first and last items
	T swap();

    // get an item from the queue.
    T peek () const;

    // check if the queue is empty.
    bool isEmpty () const;



    // get the number of items in the queue.
    int count () const;

	T peekNext() ;
	T peekCurrent ();
	T popCurrent ();
	bool next ();
	void start();

	void QueueList<T>::dumpList();

  private:
    // the structure of each node in the list.
    typedef struct node {
      T item;      // the item in the node.
      node * next; // the next node in the list.
    } node;

    typedef node * link; // synonym for pointer to a node.

	int size;        // the size of the queue.
    link head;       // the head of the list.
    link tail;       // the tail of the list.
	
	link iterator;   
	link previous;
};

// init the queue (constructor).
template<typename T>
QueueList<T>::QueueList () {
  size = 0;       // set the size of queue to zero.
  head = NULL;    // set the head of the list to point nowhere.
  tail = NULL;    // set the tail of the list to point nowhere.
}

// clear the queue (destructor).
template<typename T>
QueueList<T>::~QueueList () {
   clear();
}

template<typename T>
void QueueList<T>::clear()
{
	  // deallocate memory space of each node in the list.
  for (link t = head; t != NULL; head = t) {
    t = head->next; delete head;
	deallocs++;
  }

  size = 0;       // set the size of queue to zero.
  tail = NULL;    // set the tail of the list to point nowhere.
}


// push an item to the queue.
template<typename T>
void QueueList<T>::push (const T i) {

  link t = tail;

  // create a new node for the tail.
  tail = (link) new node;

  allocs++;

  // if there is a memory allocation error.
  if (tail == NULL)  return;

  tail->next = NULL;
  tail->item = i;

  // check if the queue is empty.
  if (isEmpty ())
    // make the new node the head of the list.
    head = tail;
  else
    // make the new node the tail of the list.
    t->next = tail;
  
  // increase the items.
  size++;

}

template<typename T>
void QueueList<T>::pushHead (const T i) 
{
  link newNode = (link) new node;
  allocs++;

  if (newNode == NULL)    //possible memory allocation error
	  return;  

  newNode->next = head;
  newNode->item = i;
  head = newNode;
   
  if (isEmpty() )
  {
    tail = head;
  }
 
  size++;
}

template<typename T>
void QueueList<T>::start() {
	iterator = head;
}



// pop an item from the queue.
template<typename T>
bool QueueList<T>::next () {
  
  bool result = true;
  

  if (size<=1) 
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
		return iterator->next->item;
		
	return NULL;
}

template<typename T>
T QueueList<T>::peekCurrent () {
	return iterator->item;
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
	Serial.println("\tItems:");

	link value = head;

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
	    if (value == iterator->item)
		{
			result=popCurrent();
		}
	} while(next());

	return result==value;
}


template<typename T>
T QueueList<T>::popCurrent () 
{
	//Serial.println("popCurrent () ");
	//dumpList();

	T result = NULL;
	//unlink the current node
	if (1 == size || iterator==head)
	{
		result= pop();
	}
	else
	{
		result = iterator->item;

		if (iterator==tail)
		{
			tail=previous;
			tail->next = NULL;
			//Serial.println("iterator==tail");
		}
		else if (NULL!=previous)
		{
			previous->next = iterator->next;
			//Serial.println("(NULL!=previous)");

		}

		delete iterator;
		iterator = NULL;
		previous = NULL;
		size--;
		deallocs++;

		//Serial.println("-- stuff is reset now --");
	}

	//dumpList();
 
	return result;
}

template<typename T>
T QueueList<T>::swap()
{
	  //unlink the head and make the 2nd node the head
	  link node = head;  
	  head=node->next;   

	  //now move the old head to the tail
	  tail->next=node;
	  tail = node;
	  tail->next = NULL;
	  
	  return node->item;

}
 
// pop an item from the queue.
template<typename T>
T QueueList<T>::pop () {
  // check if the queue is empty.
  if (isEmpty ()) return NULL;

  // get the item of the head node.
  T item = head->item;

  // remove only the head node.
  link t = head->next; delete head; head = t;
  deallocs++;

  // decrease the items.
  size--;

  // return the item.
  return item;
}

// get an item from the queue.
template<typename T>
T QueueList<T>::peek () const {
  if (isEmpty ()) return NULL;
  return head->item;
}

// check if the queue is empty.
template<typename T>
bool QueueList<T>::isEmpty () const {
  return head == NULL;
}

// get the number of items in the queue.
template<typename T>
int QueueList<T>::count () const {
  return size;
}

 

#endif // _QUEUELIST_H
