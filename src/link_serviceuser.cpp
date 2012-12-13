/*=====================================================
Name        : link_serviceuser.cpp
Author      : Nitin Puranik
Description : This file holds the definitions of the 
	      methods that service user requests.
 ======================================================*/

#include <map>
#include "link_serializer.h"
#include "link_list.h"
#include "link_memory.h"

/* Declare the global externs */
extern Memory *pool;
extern Serializer *serial;
std::map <int, List *> list_table;

/* Creats a new linked list in memory */
void CreateNewList ( ) {

	List *l = new List;

	int listID = l->GetmID ( );

	/* Add the list to the table of in-memory lists */
	list_table[listID] = l;
	std::cout << "A new list with ID "<< listID << " has been created." << std::endl;
}

/*
 * Displays the linked list in a user friendly fashion.
 */
void DisplayList ( ) {

	int listID;

	std::cout << "Enter list ID: ";

	if (std::cin >> listID) {

		if (listID <= 0) {
			std::cout << "List " << listID << " does not exist." << std::endl;
			std::cin.clear();
			while (std::cin.get() != '\n');
			return;
		}

		List *memlist = NULL;
		List *disklist = NULL;

		if (serial->TestListExistsOnDisk (listID) == true) {
			if (serial->TestListRemoved (listID) == true) {
				std::cout << "List " << listID << " has already been removed." << std::endl;
				return; 
			}
			serial->DeSerialize (&disklist, listID);
		}

		std::map<int, List*>::iterator it;

		it = list_table.find (listID);
		
		if (it != list_table.end()) {
			memlist = it->second;
		}

		if (memlist == NULL && disklist == NULL) {
			std::cout << "List " << listID << " does not exist." << std::endl;
		}
		else if (disklist != NULL) {
			disklist->Display (memlist);
		}
		else {
			memlist->Display ( );
		}
	}
	else {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Invalid entry. Please try again." << std::endl;
	}
}


/* Add a new data item to an existing list */
void AddNodeToList ( ) {

	int listID;
	int data;

	std::cout << "Enter list ID: ";

	if (!(std::cin >> listID)) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Invalid entry. Please try again." << std::endl;
		return;
	}

	if (listID <= 0) {
		std::cout << "List " << listID << " does not exist." << std::endl;
		std::cin.clear();
		while (std::cin.get() != '\n');
		return;
	}

	std::cout << "Enter node item: ";

	if (!(std::cin >> data)) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Invalid entry. Please try again." << std::endl;
		return;
	}

	std::map<int, List*>::iterator it;

	it = list_table.find (listID);
	
	// Exists in memory	
	if (it != list_table.end()) {
		List *l = it->second;
		if (l->Insert (data) == 1) {
			list_table.erase (listID);
			delete(l);
		}

		std::cout << "Item " << data << " added to list " << listID
		<< " successfuly." << std::endl;
	}

	// Exists on disk
	else if (serial->TestListExistsOnDisk (listID) == true) {

		if (serial->TestListRemoved (listID) == false) {
			List *l = new List (listID);
			list_table[listID] = l;
			l->Insert (data);
			std::cout << "Item " << data << " added to list " << listID
			<< " successfuly." << std::endl;
		}
		else {
			std::cout << "List " << listID << " has already been removed." << std::endl;
		}
	}

	else
		std::cout << "List " << listID << " does not exist." << std::endl;
}


/* Remove a data item from an existing list */
void RemoveNodeFromList ( ) {

	int listID;
	int data;

	std::cout << "Enter list ID: ";

	if (!(std::cin >> listID)) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Invalid entry. Please try again." << std::endl;
		return;
	}

	if (listID <= 0) {
		std::cout << "List " << listID << " does not exist." << std::endl;
		std::cin.clear();
		while (std::cin.get() != '\n');
		return;
	}

	std::cout << "Enter node item: ";

	if (!(std::cin >> data)) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Invalid entry. Please try again." << std::endl;
		return;
	}

	std::map<int, List*>::iterator it;

	it = list_table.find (listID);
	
	// Exists in memory	
	if (it != list_table.end()) {
		List *l = it->second;
		l->Remove (data);
	}

	else {
		serial->SetNodeRemoved (listID, data);
	}
}


/* Remove the entire list out */
void RemoveList ( ) {

	int listID;
	bool removed = false;

	std::cout << "Enter list ID: ";

	if (!(std::cin >> listID)) {
		std::cin.clear();
		while (std::cin.get() != '\n');
		std::cout << "Invalid entry. Please try again." << std::endl << std::endl;
		return;
	}

	if (listID <= 0) {
		std::cout << "List " << listID << " does not exist." << std::endl;
		std::cin.clear();
		while (std::cin.get() != '\n');
		return;
	}

	if (serial->TestListExistsOnDisk (listID) == true) {
		if (serial->TestListRemoved (listID) == true) {
			std::cout << "List " << listID << " has already been removed." << std::endl;
			return; 
		}
		else serial->SetListRemoved (listID);
		removed = true;
	}

	std::map<int, List*>::iterator it;

	it = list_table.find (listID);
	
	// Exists in memory	
	if (it != list_table.end()) {
		List *l = it->second;
		delete(l);
		list_table.erase (listID);
		removed = true;
	}

	if (removed == true)
		std::cout << "List " << listID << " successfuly removed." << std::endl;
	else
		std::cout << "List " << listID << " does not exist." << std::endl;
}


/* Dump the in-memory lists out to disk and exit */
void ExitProgram ( ) {

	serial->CheckFileExists ( );

	std::map<int, List*>::iterator it = list_table.begin ( );

	for (; it != list_table.end ( ); it++) {
		List *l = it->second;
		int listID = l->GetmID ( );
		if (serial->TestListExistsOnDisk (listID) == true)
			serial->ExtendlastBlock (listID);

		serial->Serialize (l);
		delete(l);
	}

	serial->SetgID (1);
	delete (pool);
}
