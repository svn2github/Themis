#ifndef STYLE_SHEET_H
#define STYLE_SHEET_H
/* StyleSheet.h - CSS related stuff
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/

#include "Types.h"
#ifdef __BEOS__
#	include <List.h>
#	define List BList	// to be replaced by portable list
#else
#	error "no BeOS, no list"
#endif
#include "Properties.h"

enum css_types
{
	CSS_USER_TYPE = 0,
	CSS_AUTHOR_TYPE
};

class InputStream;
class Selector;

class StyleSheet
{
	public:
		StyleSheet(css_types type,InputStream *input);
		~StyleSheet();

		void AddSelector(Selector *selector);
		void AddProperties(Properties *properties);

	private:
		int32	fType;
		List	fSelectors,fProperties;
};


/***************************** Selectors *****************************/

enum attribute_types
{
	ATTRIBUTE_NONE = 0,
	ATTRIBUTE_CLASS,
	ATTRIBUTE_ID,
	ATTRIBUTE_SET,		// [attr]
	ATTRIBUTE_EQUAL,	// [attr="xy"]
	ATTRIBUTE_IN,		// [attr~="xy"]
	ATTRIBUTE_BEGIN		// [attr|="xy"]
};

class Attribute
{
	public:
		char *name;
		char *value;
		int8 type;
};

enum	// simple selector flags
{
	SELECTOR_STANDARD		= 0x0000,
	SELECTOR_UNIVERSAL		= 0x0001,

	SELECTOR_FIRST_CHILD	= 0x0002,	// pseudo classes

	SELECTOR_LINK			= 0x0004,
	SELECTOR_VISITED		= 0x0008,

	SELECTOR_ACTIVE			= 0x0010,
	SELECTOR_HOVER			= 0x0020,
	SELECTOR_FOCUS			= 0x0040,

	SELECTOR_LANG			= 0x0080,

	SELECTOR_FIRST_LINE		= 0x0100,	// pseudo elements
	SELECTOR_FIRST_LETTER	= 0x0200,

	SELECTOR_BEFORE			= 0x0400,
	SELECTOR_AFTER			= 0x0800,
};

#define SELECTOR_PSEUDO_CLASS	(SELECTOR_FIRST_CHILD | SELECTOR_LINK | SELECTOR_VISITED | \
								 SELECTOR_ACTIVE | SELECTOR_HOVER | SELECTOR_FOCUS | \
								 SELECTOR_LANG)
#define SELECTOR_PSEUDO_ELEMENT (SELECTOR_FIRST_LINE | SELECTOR_FIRST_LETTER | \
								 SELECTOR_BEFORE | SELECTOR_AFTER)

enum operators
{
	OP_NONE = 0,
	OP_DESCENDANT,	// white-space
	OP_ADJACENT,	// +
	OP_CHILD		// >
};

class SimpleSelector
{
	public:
		SimpleSelector();
		SimpleSelector(const SimpleSelector &);
		~SimpleSelector();

		void AddAttribute(Attribute *attr);
		
		char *name;
		char *language;
		List attributes;
		uint32 flags;
		int8 op;
		
		int32 GetSpecificity();
};

class Selector
{
	public:
		Selector();
		~Selector();
		
		void AddSimpleSelector(SimpleSelector *sd);
		inline int32 GetSpecificity() { return fSpecificity; };
		void SetProperties(Properties *properties);
		
		void AddMedia(char *mediaType);

	private:
		SimpleSelector *fSubject;	// a pointer to the "subject"
		List fList;
		int32 fSpecificity;
		int32 fMediaType;
		Properties *fProperties;
};

#endif	/* STYLE_SHEET_H */
