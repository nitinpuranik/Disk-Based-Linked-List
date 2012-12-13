/*=====================================================
Name        : link_memory.h
Author      : Nitin Puranik
Description : This file defines the structure of the
	      memory recycler class. This is a singleton
	      class and manages the reuse of freed memory
	      chunks used to hold Node objects.
 ======================================================*/

#include <vector>

class Memory {
	private:
		/* vector to hold the freed Node chunks */
		std::vector <Node*> mChunks;

		Memory ( ) { }

		/* Singleton nuisances */
		Memory (const Memory&);
		Memory& operator = (const Memory&);

	public:
		static Memory* GetSingleton ( ) {
			static Memory *obj = new Memory ( );
			return obj;
		}

		/* Release ownership of all the memory chunks held */
		~Memory ( ) {
			std::vector<Node*>::iterator it = mChunks.begin ( );

			for (; it != mChunks.end ( ); it++)
				delete (*it);

			mChunks.clear ( );
		}

		friend class List;
};
