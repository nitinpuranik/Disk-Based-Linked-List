/*=====================================================
Name        : link_main.cpp
Author      : Nitin Puranik
Description : User Interface class
 ======================================================*/

#include "link_list.h"
/*
 * The initial user interface. Provides the user with the
 * menu items using which the application can be navigated.
 */

void CreateNewList      ( );
void DisplayList        ( );
void AddNodeToList      ( );
void RemoveNodeFromList ( );
void RemoveList		( );
void ExitProgram	( );

void LinklistSystem ( ) {

	unsigned short choice;

	std::cout << " ----------------------------------------------------------"  << std::endl;
	std::cout << "|                  Linked List Serializer                  |" << std::endl;
	std::cout << " ----------------------------------------------------------"  << std::endl;

	while (1) {

		std::cout << std::endl;
		std::cout << "MENU" << std::endl;
		std::cout << "[1] Add a new list" << std::endl << "[2] Display a given list" << std::endl
		<< "[3] Add a node to any given list" << std::endl << "[4] Remove a node from a list" << std::endl
		<< "[5] Remove a list" << std::endl <<"[6] Exit program" << std::endl << std::endl << "Enter your choice: ";

		if (std::cin >> choice) {

			switch (choice) {

			case 1:
				CreateNewList ( );
				break;

			case 2:
				DisplayList ( );				
				break;

			case 3:
				AddNodeToList ( );
				break;

			case 4:
				RemoveNodeFromList ( );
				break;

			case 5:
				RemoveList ( );
				break;

			case 6:
				ExitProgram ( );
				return;

			default:
				std::cin.clear ( );
				while (std::cin.get ( ) != '\n');
				std::cout << "Invalid choice. Please try again." << std::endl;
				break;
			}
		}

		else {
			std::cin.clear();
			while (std::cin.get() != '\n');
			std::cout << "Invalid choice. Please try again." << std::endl;
		}
	}
}

/*
 * A handy utility method that sets up
 * the initial parameters.
 */
void Bootup ( ) {
	List::SetGID ( );
}


/* Entry point to the program */
int main (int argc, char **argv) {

	Bootup ( );
	try {
		LinklistSystem( );
	}
	catch (const char *message) {
		std::cout << message << std::endl;
	}

	return 0;
}
