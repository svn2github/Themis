/* StyleSheet.cpp - CSS related stuff
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "StyleSheet.h"
#include "InputStream.h"
#include "StyleSheetParser.h"
#include "MediaTypes.h"


StyleSheet::StyleSheet(css_types type,InputStream *input) : fType(type)
{
	StyleSheetParser parse(this,input);
	
	if (parse.InitCheck() < RC_OK)
		return;
}


StyleSheet::~StyleSheet()
{
	// empty lists
	for(int i = fSelectors.CountItems();i-- > 0;)
	{
		Selector *selector = (Selector *)fSelectors.ItemAt(i);

		delete selector;
	}
	fSelectors.MakeEmpty();
	fProperties.MakeEmpty();
}


void
StyleSheet::AddSelector(Selector *selector)
{
	fSelectors.AddItem(selector);
}


void
StyleSheet::AddProperties(Properties *properties)
{
	fProperties.AddItem(properties);
}



/***************************** SimpleSelector *****************************/
// #pragma mark -


SimpleSelector::SimpleSelector(const SimpleSelector &sd)
{
	puts("SELECTOR_DESCRIPTION_COPY_CONSTRUCTOR");
	
	name = strdup(sd.name);
	flags = sd.flags;
}


SimpleSelector::SimpleSelector()
{
	name = language = NULL;
	flags = SELECTOR_STANDARD;
	op = OP_NONE;
}


SimpleSelector::~SimpleSelector()
{
	free(name);
	free(language);
	
	for(int index = attributes.CountItems();index-- > 0;)
	{
		Attribute *attr = (Attribute *)attributes.ItemAt(index);
		delete attr;
	}
	attributes.MakeEmpty();
}


void
SimpleSelector::AddAttribute(Attribute *attr)
{
	attributes.AddItem(attr);
}


int32
SimpleSelector::GetSpecificity()
{
	int32 specificity = name ? 1 : 0;

	// count attributes
	for(int32 index = attributes.CountItems();index-- > 0;)
	{
		Attribute *attr = (Attribute *)attributes.ItemAt(index);

		if (attr->type == ATTRIBUTE_ID)
			specificity += 10000;
		else
			specificity += 100;
	}

	// count pseudo classes/elements
	uint32 classes = flags & (SELECTOR_PSEUDO_CLASS | SELECTOR_PSEUDO_ELEMENT);
	for(;classes;classes >>= 1)
	{
		if (classes & 1)
			specificity += 100;
	}
	return specificity;
}


/***************************** Selector *****************************/
// #pragma mark -


Selector::Selector()
{
	fSubject = NULL;
	fProperties = NULL;
	fSpecificity = 0;
}


Selector::~Selector()
{
	for(int i = fList.CountItems();i-- > 0;)
	{
		SimpleSelector *sd = (SimpleSelector *)fList.ItemAt(i);
		
		fList.RemoveItem(i);
		delete sd;
	}
	delete fSubject;
	
	if (fProperties != NULL)
	{
		if (--fProperties->ownerCount == 0)	// delete unused properties
			delete fProperties;
	}
}


void
Selector::AddSimpleSelector(SimpleSelector *sd)
{
	if (fSubject != NULL)
		fList.AddItem(fSubject);

	fSubject = sd;
	fSpecificity += sd->GetSpecificity();
}


void
Selector::SetProperties(Properties *properties)
{
	fProperties = properties;
	properties->ownerCount++;
}


void
Selector::AddMedia(char */*mediaType*/)
{
	fMediaType = MEDIA_ALL_TYPES;
}

