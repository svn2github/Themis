#ifndef HASHTABLE_H
#define HASHTABLE_H
/* Hashtable - a general purpose hash table
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/

#include "Types.h"

#define HASH_EMPTY_NONE 0
#define HASH_EMPTY_FREE 1
#define HASH_EMPTY_DELETE 2

class Hashtable
{
	public:
		Hashtable(int capacity = 100,float loadFactor = 0.75);
		~Hashtable();

		void SetHashFunction(uint32 (*func)(void *));
		void SetCompareFunction(int (*func)(void *,void *));

		bool IsEmpty();
		bool ContainsKey(void *key);
		void *GetValue(void *key);

		bool Put(void *key,void *value);
		void *Remove(void *key);

		void MakeEmpty(int8 keyMode = HASH_EMPTY_NONE,int8 valueMode = HASH_EMPTY_NONE);
		
	protected:
		bool Rehash();
		struct Entry *GetHashEntry(void *key);

		int32	fCapacity,fCount,fThreshold,fModCount;
		float	fLoadFactor;
		struct Entry **fTable;
		uint32	(*fHashFunc)(void *);
		int		(*fCompareFunc)(void *,void *);
};

#endif  // HASHTABLE_H
