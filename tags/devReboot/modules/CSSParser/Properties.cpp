/* Properties.cpp - CSS properties related stuff
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Properties.h"


/***************************** UnitValue *****************************/

UnitValue::UnitValue()
{
	value = 0; flags = 0;
}


/***************************** FontFamily *****************************/
// #pragma mark -


FontFamily::~FontFamily()
{
	free(name);
}


/***************************** Flags *****************************/
// #pragma mark -


Flags::Flags()
{
	Reset();
}


void
Flags::Reset()
{
	flags[0] = flags[1] = flags[2] = 0;
}


void
Flags::Set(uint32 flag,bool set = true)
{
	int index = flag/32;
	flag = 1L << (flag % 32);

	flags[index] = (flags[index] & ~flag) | (set ? flag : 0);
}


bool
Flags::IsSet(uint32 flag)
{
	return (flags[flag/32] & (1L << (flag % 32))) > 0;
}


Flags &
Flags::operator|=(const Flags &or)
{
	flags[0] |= or.flags[0];
	flags[1] |= or.flags[1];
	flags[2] |= or.flags[2];
	return *this;
}


bool
Flags::operator!=(int)
{
	return flags[0] || flags[1] || flags [2];
}


/***************************** Properties *****************************/
// #pragma mark -


Properties::Properties()
{
	Clear();
}


Properties::~Properties()
{
	// fonts
	for(int index = fontFamilies.CountItems();index-- > 0;)
		delete (FontFamily *)fontFamilies.ItemAt(index);

	fontFamilies.MakeEmpty();
	counter.MakeEmpty(HASH_EMPTY_DELETE);

	/* todo: delete every single property */
}


void
Properties::Clear()
{
	int offset = (int)&((Properties *)NULL)->counter;
	memset(this,offset,sizeof(Properties) - offset);
}

