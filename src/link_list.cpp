/*=====================================================
Name        : link_list.cpp
Author      : Nitin Puranik
Description : This file holds the definitions of
	      the List class member functions.
 ======================================================*/

#include "link_list.h"
#include "link_memory.h"
#include "link_serializer.h"

/* Define the statics and the singleton objects */
int List::gID;
Memory *pool = Memory::GetSingleton ( );
Serializer *serial = Serializer::GetSingleton ( );

List::~List ( ) {

	Node *temp = mHead;

	while (temp != NULL) {
		pool->mChunks.push_back (temp);
		temp = temp->mNext;
	}

	mHead = NULL;
}

/*
 * Insert a new item into the linked list. Reuse the memory
 * freed from older nodes. If the linked list size reaches
 * the defined MAX, then serialize the list out to disk and
 * release the occupied memory. The list then resides only
 * on disk. Return 0 or 1 to indicate whether the list was
 * serialized out to disk or not.
 */
int List::Insert (const int& data) {

	Node *node = NULL;

	/* No chunks available for recycling */
	if (pool->mChunks.empty ( ))
		node = new Node (data);

	/* Chunks available for recycling */
	else {
		Node *ptr = pool->mChunks.back ( );
		pool->mChunks.pop_back ( );

		node = new (ptr) Node (data);
	}

	/* Empty list */
	if (mHead == NULL)
		mHead = node;

	else {
		Node *temp = mHead;
		Node *prev = NULL;

		while (temp != NULL && temp->mData < data) {
			prev = temp;
			temp = temp->mNext;
		}

		if (prev == NULL) {
			node->mNext = temp;
			mHead = node;
		}

		else {
			prev->mNext = node;
			node->mNext = temp;
		}
	}

	mSize++;

	/*
	 * If the list reaches the defined MAX size,
	 * then we serialize the list out to disk.
	 */
	if (mSize == BLOCK) {
		if (serial->TestListExistsOnDisk (mID) == true)
			serial->ExtendlastBlock (mID);

		serial->Serialize (this);
		return 1;
	}

	return 0;
}


/*
 * An overload of the above insert function. This overload
 * will be called when we are rebuilding the list from disk.
 * during de-serialization. Overload needed so that we don't
 * serialize the list back again to disk.
 */
void List::Insert (const int& data, const int& foo) {

	Node *node = new Node (data);

	if (mHead == NULL)
		mHead = node;

	else {
		Node *temp = mHead;
		Node *prev = NULL;

		while (temp != NULL && temp->mData < data) {
			prev = temp;
			temp = temp->mNext;
		}

		if (prev == NULL) {
			node->mNext = temp;
			mHead = node;
		}

		else {
			prev->mNext = node;
			node->mNext = temp;
		}
	}
}


/*
 * Remove a given data item from the list.
 */
void List::Remove (const int& data) {

	if (mHead == NULL) {
		std::cout << "List " << mID << " is empty." << std::endl;
		return;
	}

	Node *temp = mHead;
	Node *prev = NULL;

	while (temp != NULL && temp->mData != data) {
		prev = temp;
		temp = temp->mNext;
	}

	/* The data item does not exist in memory */
	if (temp == NULL) {
		if (serial->TestListExistsOnDisk (mID) == false)
			std::cout << "Item " << data << " not found in List "
				<< mID << std::endl;
		else
			/* Remove the data item from the list on disk */
			serial->SetNodeRemoved (mID, data);
		return;
	}

	/* The item exists in memory */
	else if (prev == NULL)
		mHead = temp->mNext;

	else
		prev->mNext = temp->mNext;

	std::cout << "Item " << data << " removed successfuly "
	<< "from List " << mID << std::endl;

	/* Send out the freed chunk to the pool for recycling */
	pool->mChunks.push_back (temp);
	mSize--;
}


/*
 * The regular display function that displays
 * the items of the list in sorted order.
 */
void List::Display ( ) {

	if (mHead == NULL) {
		std::cout << "List " << mID << " is empty." << std::endl;
		return;
	}
	
	std::cout << "[List " << mID << "] " << mHead->mData;

	Node *temp = mHead->mNext;

	while (temp != NULL) {
		std::cout << " -> " << temp->mData;
		temp = temp->mNext;
	}

	std::cout << std::endl;
}


/*
 * An overload of the display function for the
 * case when part of the list is in memory and
 * part of it is on disk.
 */
void List::Display (const List *memlist) {

	Node *disktemp = mHead;
	Node *memtemp = (memlist == NULL) ? NULL : memlist->mHead;

	if (disktemp == NULL && memtemp == NULL) {
		std::cout << "List " << mID << " is empty." << std::endl;
		return;
	}

	std::cout << "[List " << mID << "] ";

	/* Merge the output of the disklist and memlist */
	while (disktemp != NULL && memtemp != NULL) {
		if (disktemp->mData < memtemp->mData) {
			std::cout << disktemp->mData << " -> ";
			disktemp = disktemp->mNext;
		}
		else {
			std::cout << memtemp->mData << " -> ";
			memtemp = memtemp->mNext;
		}
	}

	/* Print out the remaining items of the longer list */
	Node *temp;

	temp = (disktemp == NULL) ? memtemp : disktemp;
	std::cout << temp->mData;
	temp = temp->mNext;

	while (temp != NULL) {
		std::cout << " -> " << temp->mData;
		temp = temp->mNext;
	}

	std::cout << std::endl;
}


/*
 * Set up the global list ID for the List class
 * to be used while creating new lists.
 */
void List::SetGID ( ) {
	gID = serial->GetgID ( );

	if (gID == 0)
		serial->SetgID (0);
}
