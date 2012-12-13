/*=====================================================
Name        : link_list.h
Author      : Nitin Puranik
Description : This header file defines the structure
	      of the Node class and the List class.
 ======================================================*/

#include <iostream>

/*
 * This macro tells what is the maximum number
 * of items there can be in a link list in memory
 * at a given time. If the list size exceeds this,
 * the list is serialized out to disk.
 */
#define BLOCK 10


/* Structure of the node class that makes up a list. */
class Node {
	private:
		int mData;	/* data item */
		Node *mNext;	/* Pointer to next item */

	public:
		Node (const int& val) : mData (val), mNext (NULL) { }

		~Node ( ) {
			mNext = NULL;
		}

		friend class List;
		friend class Serializer;
};


/* Structure of a singly linked list. Made up of nodes. */
class List {
	private:
		Node *mHead;	/* Head of the list */
		static int gID;	/* A static ID that assigns IDs to lists */
		int mID;	/* ID of the "this" list object */
		int mSize;	/* Number of items in the list */

	public:
		List ( ) : mHead (NULL), mID (++gID), mSize (0) { }

		List (const int& listID) :
			mHead (NULL), mID (listID), mSize (0) { }

		~List ( );

		int GetmID ( ) { return mID; }
		int Insert (const int&);
		void Insert (const int&, const int&);
		void Remove (const int&);
		void Display ( );
		void Display (const List*);
		static void SetGID ( );
		static int GetGID ( ) { return gID; }

		friend class Serializer;
};
