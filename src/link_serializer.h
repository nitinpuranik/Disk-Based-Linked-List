/*=====================================================
Name        : link_serializer.h
Author      : Nitin Puranik
Description : This header file defines the structure
	      of the disk serializer class.
 ======================================================*/

#include <fstream>

#define EOB  0xdeadbeef /* End of block in file */
#define TOMB 0xbaadf00d /* Tombstone for a removed node */

class List;

/*
 * A singleton class that handles the disk
 * functionalities such as serialization and
 * deserialization, disk updates and tests.
 */
class Serializer {
	private:
		std::fstream file;

	public:
		Serializer ( ) { }
		Serializer (const Serializer&);
		Serializer& operator = (const Serializer&);

		static Serializer* GetSingleton ( ) {
			static Serializer *obj = new Serializer ( );
			return obj;
		}

		void Serialize (List*);
		void DeSerialize (List**, const int&);
		void ExtendlastBlock (const int&);

		void CheckFileExists ( );
		bool TestListExistsOnDisk (const int&);
		bool TestListRemoved (const int&);
		void SetNodeRemoved (const int&, const int&);
		void SetListRemoved (const int&);
		void SetgID (const int&);
		int GetgID ( );
};
