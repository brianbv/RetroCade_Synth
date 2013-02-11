/*!
 *  @file		QueueStack.h
 *  Project		RetroCade
 *	@brief		Pool based Queue/Stack implementation
 *	Version		1.0
 *  @author		Brian Vogel
 *	@date		2/10/2013
 *  License		GPL
 *
 */
#ifndef _QueueStack_H
#define _QueueStack_H

// include Arduino basic header.
#include <Arduino.h>
#include "Config.h"

 
 
template<typename T> struct ListNode
{
	T value;      // the item in the node.
    ListNode* next; // the next node in the list.
};

template<typename T>
class QueueStack {
	public:
	
	QueueStack();
	//~QueueStack();

	void clear();
	int count () const;

	//list management
	T push (); //add an item to the list from the pool
	void push (const T value); //add an item 
	void pushHead (const T  value);
	bool popTail ();
	

	T pop();
	bool popValue (T value);
	T swap();  //swaps the first and last value
    T peek () const; //returns the head val

	//iteration
	T peekNext() ;
	T peekCurrent ();
	T popCurrent ();
	bool next ();
	void start();
	void dumpList();
	
	//memory pooling
	void setPool(ListNode<T>** poolData,int poolSize);

	protected:
	void push (ListNode<T>*  nextNode);
	void pushHead (ListNode<T>*  nextNode);
	
	//iteration
	ListNode<T>* iterator;   
	ListNode<T>* previous;

	//list management
	ListNode<T>* head;       // the head of the list.
    ListNode<T>* tail;       // the tail of the list.

	int size;				 //item count   

    //memory pooling
	ListNode<T>* getNode();
	void releaseNode(ListNode<T>* releaseNode);
	ListNode<T>** pool;
	int poolMaxSize;
	int poolIndex;



};

template<typename T>
QueueStack<T>::QueueStack()
{
	clear();
}
 
template<typename T>
void QueueStack<T>::clear()
{
  size = 0; 
  head = NULL;  
  tail = NULL; 
}

template<typename T>
void QueueStack<T>::setPool(ListNode<T>** pool,int size)
{
	this->poolMaxSize=size;
	this->pool = pool;
	this->poolIndex = 0;
}
 
template<typename T>
ListNode<T>* QueueStack<T>::getNode()
{
	ListNode<T>* result = (poolIndex>=poolMaxSize) ? pool[poolMaxSize-1] : pool[poolIndex];   
	++poolIndex;
	return result;
}


template<typename T>
void QueueStack<T>::releaseNode(ListNode<T>* releaseNode)
{
	--poolIndex;
	pool[poolIndex] = releaseNode;
}

 

// get the number of items in the queue.
template<typename T>
int QueueStack<T>::count () const {
  return size;
}


template<typename T>
T QueueStack<T>::push() 
{
	ListNode<T>*  poolItem = (ListNode<T>*) getNode();
	this->push(poolItem);
	return poolItem->value;
}

template<typename T>
void QueueStack<T>::push (const T value)
{
	ListNode<T>* poolItem =  getNode();
	poolItem->value = value;
	this->push(poolItem);
}


// push an value to the tail of the queue
template<typename T>
void QueueStack<T>::push (ListNode<T>*  nextNode) 
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
void QueueStack<T>::pushHead (const T value) 
{
	ListNode<T>* poolvalue =  this->getNode();
	poolvalue->value = value;
	pushHead(poolvalue);
}

template<typename T>
void QueueStack<T>::pushHead (ListNode<T>*  newNode) 
{
  newNode->next = this->head;
  this->head = newNode;
   
  if (0==this->size) this->tail = this->head;

  this->size++;
}


template<typename T>
void QueueStack<T>::start() {
	iterator = this->head;
}

// pop an value from the queue.
template<typename T>
bool QueueStack<T>::next () {
  
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
T QueueStack<T>::peekNext () {

	if (NULL!=iterator && 
	    NULL !=iterator->next)
		return iterator->next->value;
		
	return NULL;
}

template<typename T>
T QueueStack<T>::peekCurrent () {
	return iterator->value;
}

template<typename T>
T QueueStack<T>::popCurrent () 
{
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
		}
		else if (NULL!=previous)
		{
			previous->next = iterator->next;
		}

		this->releaseNode(iterator);
		iterator = NULL;
		previous = NULL;
		this->size--;
	}

	return result;
}


template<typename T>
void QueueStack<T>::dumpList () 
{
	#ifdef DEBUG

	Serial.println();
	Serial.print("QueueStack (0x");
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
bool QueueStack<T>::popValue (T value)
{
	T result;
	start();

	do
	{
	    if (value == iterator->value)
		{
			result=popCurrent();
			break;
		}
	} while(next());

	return result==value;
}

template<typename T>
bool QueueStack<T>::popTail ()
{
	start();

	do
	{
	    if (iterator==tail)
		{
			popCurrent();
			break;
		}
	} while(next());

	return iterator==tail;
}

template<typename T>
T QueueStack<T>::swap()
{
	  //unlink the head and make the 2nd node the head
	  ListNode<T>* node = this->head;  
	  this->head=node->next;   

	  //now move the old head to the tail
	  this->tail->next=node;
	  this->tail = node;
	  this->tail->next = NULL;
	  
	  return tail->value;
}

 
// pop an value from the queue.
template<typename T>
T QueueStack<T>::pop () {

  if (0==this->size) return NULL;
  T value = this->head->value;

  // remove only the head node.
  ListNode<T>* t = this->head->next; 
  releaseNode(this->head); 
  this->head = t;
  this->size--;
  return value;
}

// get an value from the queue.
template<typename T>
T QueueStack<T>::peek () const 
{
  if (0==this->size) return NULL;
  return this->head->value;
}



#endif