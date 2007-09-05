/* Hashtable - a general purpose hash table
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/


#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "Hashtable.h"

// Every entry in the hashtable is embedded in this structure

struct Entry
{
  struct Entry *e_Next;
  void   *e_Key;
  void   *e_Value;
};


/************************** standard string hash functions **************************/

unsigned int stringHash(char *c)
{
  //unsigned int hash = 0;
  int len = strlen(c);
  
  return(*(int *)(c+len-4));  // erstmal zum Testen
}


int stringCompare(char *a,char *b)
{
  return(!strcmp(a,b));
}


/************************** Hashtable **************************/
// #pragma mark -


/** Erzeugt einen neuen Hashtable nach den Vorgaben des
 *  Benutzers.
 *
 *  @param capacity die Anfangskapazität des Hashtables. Sie wird zwar bei
 *    Bedarf vergrößert, aber das kann dann etwas länger dauern, da der Hashtable
 *    dann komplett neu aufgebaut werden muß. Voreingestellt sind 100 Einträge.
 *  @param loadFactor die gewünschte maximale Auslastung des Hashtables.
 *    Bei kleinen Werten muß der Hashtable häufiger neu aufgebaut werden und belegt
 *    mehr Speicher, ist aber schnell. Bei großen Werten wird das Beschreiben und
 *    Auslesen langsamer. Voreingestellt ist 0.75f.
 */

Hashtable::Hashtable(int capacity, float loadFactor)
{
	if (capacity < 0 || loadFactor <= 0)
		return;

	if (!capacity)
		capacity = 1;
	
	if (!(fTable = (struct Entry **)malloc(capacity * sizeof(struct Entry *))))
		return;
	memset(fTable,0,capacity * sizeof(struct Entry *));
	
	fThreshold = (int)(capacity * loadFactor);
	fModCount = 0;
	fLoadFactor = loadFactor;
	fCapacity = capacity;

	fHashFunc = (uint32 (*)(void *))stringHash;
	fCompareFunc = (int (*)(void *,void *))stringCompare;
}


Hashtable::~Hashtable()
{
	struct Entry **table = fTable;
	
	for(int32 index = fCapacity;--index >= 0;)
	{
		struct Entry *e,*next;

		for(e = table[index];e;e = next)
		{
			next = e->e_Next;
			free(e);
		}
	}
	free(table);
}


void Hashtable::SetHashFunction(uint32 (*func)(void *))
{
	fHashFunc = func;
}


void Hashtable::SetCompareFunction(int (*func)(void *,void *))
{
	fCompareFunc = func;
}


/** Testet, ob der Hashtable leer ist.
 *
 *  @return isEmpty true, wenn der Hashtable leer ist, false, wenn nicht
 */

bool Hashtable::IsEmpty()
{
	return fCount == 0;
}


/** Testet, ob ein bestimmter Schlüssel im Hashtable enthalten ist.
 *
 *  @param key der gesuchte Schlüssel
 *  @return success TRUE, wenn der Schlüssel enthalten ist, FALSE, wenn nicht
 */

bool Hashtable::ContainsKey(void *key)
{
	return GetHashEntry(key) ? true : false;
}


/** Sucht den zu einem Schlüssel (key) passenden Wert (value)
 *
 *  @param key der gesuchte Schlüssel
 *  @return value der zum Schlüssel gehörige Wert
 */

void *Hashtable::GetValue(void *key)
{
	struct Entry *e = GetHashEntry(key);

	return e ? e->e_Value : NULL;
}


/** Mit dieser Funktion wird ein neuer Eintrag bestehend aus einem
 *  Zugriffsschlüssel (key) und dessen Wert (value) in den Hashtable
 *  eingefügt.
 *  Gibt es diesen Schlüssel bereits im Hashtable, wird der alte
 *  Eintrag überschrieben.
 *
 *  @short einen neuen Eintrag in einen Hashtable einfügen
 *
 *  @param key der Schlüssel
 *  @param value der zugehörige Wert
 *  @return succes TRUE, wenn der Eintrag hinzugefügt wurde, FALSE, wenn nicht
 */

bool Hashtable::Put(void *key, void *value)
{
	struct Entry *e = GetHashEntry(key);
	int hash = fHashFunc(key);
	int index;
	
	if (e)
		return true;
	
	fModCount++;
	if (fCount >= fThreshold)
		Rehash();
	
	index = hash % fCapacity;
	
	if (!(e = (struct Entry *)malloc(sizeof(struct Entry))))
		return false;
	
	e->e_Key = key;
	e->e_Value = value;
	e->e_Next = fTable[index];
	fTable[index] = e;  
	fCount++;
	
	return true;
}


/** @short entfernt einen Eintrag aus einem Hashtable
 *
 *  Mit dieser Funktion entfernt man den zum Schlüssel (key)
 *  gehörigen Eintrag aus dem Hashtable.
 *
 *  @param key der Schlüssel
 *
 *  @return value der zum Schlüssel gehörende Wert
 */

void *Hashtable::Remove(void *key)
{
	struct Entry **table,*e,*prev;
	uint32 hash,(*func)(void *);
	int32 index;
	
	table = fTable;
	hash = (func = fHashFunc)(key);
	index = hash % fCapacity;
	
	for(e = table[index],prev = NULL;e;e = e->e_Next)
	{
		if ((func(e->e_Key) == hash) && fCompareFunc(e->e_Key,key))
		{
			void *value;

			fModCount++;
			if (prev)
				prev->e_Next = e->e_Next;
			else
				table[index] = e->e_Next;
			
			fCount--;
			value = e->e_Value;
			free(e);

			return value;
		}
		prev = e;
	}
	return NULL;
}


void Hashtable::MakeEmpty(int8 keyMode = HASH_EMPTY_NONE,int8 valueMode = HASH_EMPTY_NONE)
{
	fModCount++;

	for(int32 index = fCapacity;--index >= 0;)
	{
		struct Entry *e,*next;

		for(e = fTable[index];e;e = next)
		{
			switch(keyMode)
			{
				case HASH_EMPTY_DELETE:
					delete e->e_Key;
					break;
				case HASH_EMPTY_FREE:
					free(e->e_Key);
					break;
			}
			switch(valueMode)
			{
				case HASH_EMPTY_DELETE:
					delete e->e_Value;
					break;
				case HASH_EMPTY_FREE:
					free(e->e_Value);
					break;
			}
			next = e->e_Next;
			free(e);
		}
		fTable[index] = NULL;
	}
	fCount = 0;
}


/** Der Hashtable wird in der Kapazität verdoppelt und neu
 *  aufgebaut.
 *
 *  @return success true, wenn die Kapazität verdoppelt werden konnte, false, wenn nicht
 */
 
bool Hashtable::Rehash()
{
	uint32 (*hashCode)(void *) = fHashFunc;
	struct Entry **oldtable = fTable,**newtable;
	int oldCapacity = fCapacity;
	int newCapacity,i;

	newCapacity = oldCapacity * 2 + 1;
	if (!(newtable = (struct Entry **)malloc(newCapacity * sizeof(struct Entry *))))
		return false;
	memset(newtable,0,newCapacity*sizeof(struct Entry *));

	fModCount++;
	fThreshold = (int)(newCapacity * fLoadFactor);
	fTable = newtable;
	fCapacity = newCapacity;

	for(i = oldCapacity;i-- > 0;)
	{
		struct Entry *old,*e = NULL;
		int index;
		
		for (old = oldtable[i];old;)
		{
			e = old;  old = old->e_Next;
			
			index = hashCode(e->e_Key) % newCapacity;
			e->e_Next = newtable[index];
			newtable[index] = e;
		}
	}
	free(oldtable);

	return true;
}


/** Gibt den zu einem Schlüssel passenden Eintrag eines Hashtables
 *  zurück.
 *
 *  @param key der zu suchende Schlüssel
 *  @return entry der gefundene Eintrag oder NULL
 */

struct Entry *Hashtable::GetHashEntry(void *key)
{
	struct Entry **table,*e;
	uint32 hash,(*func)(void *);
	
	table = fTable;
	hash = (func = fHashFunc)(key);
	
	for(e = table[hash % fCapacity];e;e = e->e_Next)
	{
		if ((func(e->e_Key) == hash) && fCompareFunc(e->e_Key,key))
			return e;
	}
	return NULL;
}

