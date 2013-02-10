/*!
 *  @file		ListBase.h
 *  Project		RetroCade
 *	@brief		Pool based Linked List implementation
 *	Version		1.0
 *  @author		Brian Vogel
 *	@date		2/10/2013
 *  License		GPL
 *
 */
#ifndef _LISTBASE_H
#define _LISTBASE_H

// include Arduino basic header.
#include <Arduino.h>
#include "Config.h"

 
 
template<typename T> struct ListNode
{
	T value;      // the item in the node.
    ListNode* next; // the next node in the list.
};

template<typename T>
class ListBase {
	public:
	
	ListBase();
	//~ListBase();

	void clear();
	int count () const;
    bool isEmpty () const;

	//list management
	T push (); //add an item to the list from the pool
	void push (const T value); //add an item 
	void pushHead (const T  value);
	

	//iteration
	T peekNext() ;
	T peekCurrent ();
	T popCurrent ();
	bool next ();
	void start();
	void dumpList();
	
	//memory pooling
	void setPool(ListNode<T>* poolData,int poolSize);

	protected:
	void push (ListNode<T>*  nextNode);
	void pushHead (ListNode<T>*  nextNode);
	    
	//list management
	ListNode<T>* head;       // the head of the list.
    ListNode<T>* tail;       // the tail of the list.
	int size;				 //item count   

    //memory pooling
	ListNode<T>* getNode();
	void releaseNode(ListNode<T>* releaseNode);
	ListNode<T>* pool;
	int poolMaxSize;
	int poolIndex;



};

template<typename T>
ListBase<T>::ListBase()
{
	clear();
}
 
 
template<typename T>
ListNode<T>* ListBase<T>::getNode()
{
	ListNode<T>* result = NULL;

	//ListNode<T>* result = pool[poolIndex];
	//pool[poolIndex]=NULL;

	// don't let it go over
	if (++poolIndex>=poolMaxSize)
		poolIndex=poolMaxSize=1;

	return result;
}

template<typename T>
void ListBase<T>::clear()
{
  size = 0; 
  head = NULL;  
  tail = NULL; 
}

template<typename T>
void ListBase<T>::releaseNode(ListNode<T>* releaseNode)
{
	 //pool[poolIndex] = releaseNode;

	// don't let it go over
	if (--poolIndex<0)
		poolIndex=0;
}

// check if the queue is empty.
template<typename T>
bool ListBase<T>::isEmpty () const {
  return head == NULL;
}

// get the number of items in the queue.
template<typename T>
int ListBase<T>::count () const {
  return size;
}


template<typename T>
T ListBase<T>::push() 
{
	ListNode<T>*  poolItem = (ListNode<T>*) getNode();
	this->push(poolItem);
	return poolItem->value;
}

template<typename T>
void ListBase<T>::push (const T value)
{
	ListNode<T>* poolItem =  getNode();
	poolItem->value = value;
	this->push(poolItem);
}

#endif