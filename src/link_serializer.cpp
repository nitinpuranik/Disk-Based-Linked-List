/*=====================================================
Name        : link_serializer.cpp
Author      : Nitin Puranik
Description : This file holds the definitions of
	      the Serializer class member functions.
 ======================================================*/

#include "link_serializer.h"
#include "link_list.h"


/*
 * This method flattens out a linked list
 * out to disk. It writes out the list ID,
 * offset to the next block holding this list
 * and finally the node items making up the list.
 */
void Serializer::Serialize (List *l) {

	int flag = 1;
	int count = 0;

	file.open ("Linklist.bin", std::ios::binary | std::ios::out | std::ios::app);

	if (file.is_open()) {

		file.write ((char*)&(l->mID), sizeof (int));	/* Write out the ID */
		file.write ((char*)&flag, sizeof (int));	/* Write out the offset */

		Node *temp = l->mHead;

		while (temp != NULL) {
			file.write ((char*)&(temp->mData), sizeof (int));
			temp = temp->mNext;
			count++;
		}

		/*
		 * Do a padding of the trailing portion of the list
		 * block so that we get a constant sized block always.
		 */
		int tombstone = TOMB;

		for (; count < BLOCK; count++) {
			file.write ((char*)&tombstone, sizeof(int));
		}

		file.close();
	}
}


/*
 * This method unflattens the list out from disk
 * back into main memory. It reads all the chunks
 * scattered all over the disk into one long list.
 */
void Serializer::DeSerialize (List **l, const int& listID) {

	file.open ("Linklist.bin", std::ios::binary | std::ios::in);

	if (file.is_open()) {

		file.seekg (static_cast<int> (sizeof(int)), std::ios::beg);

		int ID = 0;
		while (file.good()) {

			file.read ((char*)&ID, sizeof(int));
		 	if (ID != listID)	/* Jump to the next block */

				file.seekg ((1 + BLOCK) * static_cast<int> (sizeof(int)), std::ios::cur);
			else	/* Got the first block */

				break;
		}

		int flag;
		*l = new List (listID);

		/* Read out the node data that make up the list */
		while (file.good()) {

			file.read ((char*)&flag, sizeof(int));

			int nodedata;

			for (int i = 0; i < BLOCK; i++) {

				file.read ((char*)&nodedata, sizeof(int));
				if (nodedata == static_cast<int> (EOB))
					break;

				if (nodedata != static_cast<int>(TOMB)) {
					(*l)->Insert (nodedata, 0);
				}
			}

			if (flag == 1) break;

			file.seekg (flag + static_cast<int> (sizeof(int)));
		}

		file.close ( );
	}
}

/*
 * This function checks if a given list has been
 * deleted. The disk continues to hold that list
 * and will be marked deleted. The file remains fragmented.
 */
bool Serializer::TestListRemoved (const int& listID) {

	file.open ("Linklist.bin", std::ios::binary | std::ios::in);

	if (file.is_open()) {

		file.seekg (static_cast<int> (sizeof(int)), std::ios::beg);

		int ID = 0;
		while (file.good()) {

			file.read ((char*)&ID, sizeof(int));
		 	if (ID != listID) /* Jump to the next block */

				file.seekg ((1 + BLOCK) * sizeof(int), std::ios::cur);
			else	/* Got the first block */

				break;
		}

		if (ID == listID) {

			int flag;
			file.read ((char*)&flag, sizeof(int));
			file.close ( );

			/* If flag is zero, it means the list has been deleted */
			return (flag == 0) ? true : false;
		}
	}

	return true;
}


/*
 * This function tests if the given list
 * exists on the disk, immaterial of whether
 * the list has been deleted or not.
 */
bool Serializer::TestListExistsOnDisk (const int& listID) {

	file.open ("Linklist.bin", std::ios::binary | std::ios::in);

	if (file.is_open()) {

		file.seekg (static_cast<int> (sizeof(int)));

		int ID = 0;
		while (file.good()) {

			file.read ((char*)&ID, sizeof(int));
		 	if (ID != listID) // Jump to next block
				file.seekg ((1 + BLOCK) * sizeof(int), std::ios::cur);
			else
				break;
		}

		if (ID == listID) {
			file.close ( );
			return true;
		}

		file.close ( );
		return false;
	}
	return false;
}


/*
 * This method marks the list on disk as deleted. The list
 * continues to reside on disk and hence fragments the file.
 */
void Serializer::SetListRemoved (const int& listID) {

	file.open ("Linklist.bin", std::ios::binary | std::ios::in | std::ios::out);

	if (file.is_open()) {

		file.seekg (static_cast<int> (sizeof(int)), std::ios::beg);

		int ID = 0;
		while (file.good()) {

			file.read ((char*)&ID, sizeof(int));
		 	if (ID != listID) // Jump to next block
				file.seekg ((1 + BLOCK) * static_cast<int> (sizeof(int)), std::ios::cur);
			else
				break;
		}

		if (ID == listID) {
			int flag = 0; /* Mark the list deleted with flag = 0 */
			file.seekp (0, std::ios::cur);
			file.write ((char*)&flag, sizeof(int));
		}

		file.close ( );
	}
}


/*
 * This method marks a particular node within a list
 * deleted. The node is tombstoned and not erased from
 * disk. The deleted node continues to occupy disk space.
 */
void Serializer::SetNodeRemoved (const int& listID, const int& data) {

	file.open ("Linklist.bin", std::ios::binary | std::ios::in | std::ios::out);

	if (file.is_open()) {

		file.seekg (static_cast<int> (sizeof(int)), std::ios::beg);

		int ID = 0;
		while (file.good()) {

			file.read ((char*)&ID, sizeof(int));
	 		if (ID != listID) // Jump to next block
				file.seekg ((1 + BLOCK) * sizeof(int), std::ios::cur);
			else
				break;
		}

		if (ID == listID) {

			int flag;
			file.read ((char*)&flag, sizeof(int));

			if (flag == 0) {
				std::cout << "List " << listID << " has already been removed." << std::endl;
			}

			else {
				while (file.good()) {

					int nodedata;

					for (int i = 0; i < BLOCK; i++) {

						file.read ((char*)&nodedata, sizeof(int));
						if (nodedata == data) {
							int tombstone = TOMB;
							file.seekp (static_cast<int> (-sizeof(int)), std::ios::cur);
							file.write ((char*)&tombstone, sizeof(int));
							break;
						}
						else if ((nodedata > data && nodedata != static_cast<int>(TOMB))
								|| (nodedata == static_cast<int>(static_cast<int> (EOB))))
							break;
					}

					if (nodedata == data) {
						std::cout << "Item " << data << " removed successfuly "
						<< "from List " << listID << std::endl;
						break;
					}

					if (flag == 1) { /* Last block for this ID */
						std::cout << "Item " << data << " not found in List "
						<< listID << std::endl;
						break;
					}

					file.seekg (flag + sizeof(int));
					file.read ((char*)&flag, sizeof(int));
				}
			}
		}
		else {
			std::cout << "Item " << data << " not found in List "
			<< listID << std::endl;
		}

		file.close ( );
	}
	else
		std::cout << "List " << listID << " does not exist." << std::endl;
}


/*
 * If part of the list in memory is on disk,
 * then this function updates the offset of that
 * disk block to point to the next new block
 * of the list, which will be appended to file.
 */
void Serializer::ExtendlastBlock (const int& listID) {

	file.open ("Linklist.bin", std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);

	if (file.is_open()) {
		int size;
		size = file.tellg();
		file.seekg (static_cast<int> (sizeof(int)), std::ios::beg);

		int ID = 0;
		while (file.good()) {
			file.read ((char*)&ID, sizeof(int));
		 	if (ID != listID) // Jump to next block
				file.seekg ((1 + BLOCK) * (sizeof(int)), std::ios::cur);
			else
				break;
		}

		int flag;
		file.read ((char*)&flag, sizeof(int));

		// While not the last block
		while (flag != 1) {
			file.seekg (flag + static_cast<int> (sizeof(int)));
			file.read ((char*)&flag, sizeof(int));
		}

		// Now we have the last block
		file.seekp (static_cast<int> (-sizeof(int)), std::ios::cur);
		file.write ((char*)&size, sizeof(int));
		file.close();
	}
}


/*
 * Get the highest ID of the list ever created.
 */
int Serializer::GetgID ( ) {

	int gID = 0;

	file.open ("Linklist.bin", std::ios::binary | std::ios::in);

	if (file.is_open()) {
		file.read ((char*)&gID, sizeof(int));
		file.close ( );
	}

	return gID;
}


/*
 * Store the highest ID of the list ever created.
 */
void Serializer::SetgID (const int& mode) {

	int gID = List::GetGID ( );

	if (mode == 0 /* new file */) 
	file.open ("Linklist.bin", std::ios::binary | std::ios::out | std::ios::in | std::ios::app);
	else /* old file */
	file.open ("Linklist.bin", std::ios::binary | std::ios::out | std::ios::in);

	if (file.is_open()) {
		file.write ((char*)&gID, sizeof(int));
		file.close ( );
	}
}


/*
 * A simple utility to check if the disk file exists.
 * Throw if file not found.
 */
void Serializer::CheckFileExists ( ) {
	file.open ("Linklist.bin", std::ios::binary | std::ios::out | std::ios::in);

	if (!file.is_open())
		throw "[Error] File not found or has been corrupted. Aborted.";

	file.close ( );
}
