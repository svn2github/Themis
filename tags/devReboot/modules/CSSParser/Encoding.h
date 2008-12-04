#ifndef ENCODING_H
#define ENCODING_H
/* Encoding - this class takes care of different character sets
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/


#include "Types.h"

enum encoding_types
{
	UNKNOWN_ENCODING,
	UNSPECIFIED_ASCII_SUPERSET_ENCODING,

	ISO_8859_1_ENCODING,
	ISO_8859_2_ENCODING,
	ISO_8859_3_ENCODING,
	ISO_8859_4_ENCODING,
	ISO_8859_5_ENCODING,
	ISO_8859_6_ENCODING,
	ISO_8859_7_ENCODING,
	ISO_8859_8_ENCODING,
	ISO_8859_9_ENCODING,

	ISO_646_ENCODING, 
	UTF_8_ENCODING,
	UTF_16B_ENCODING, UTF_16L_ENCODING,
	ISO_10646_UCS_2B_ENCODING,
	ISO_10646_UCS_2L_ENCODING,
	
	NUM_ENCODINGS
};


class Encoding
{
	public:
		Encoding(uint16 type);
		virtual ~Encoding();

		uint16 Type() { return fType; }

		/** the abstract base class **/

		virtual int16 LookupLatin1(char *) abstract;
		virtual uint16 LookupUnicode16(char *) abstract;

		virtual char *NextChar(char *) abstract;

		// common translations
		virtual char *TranslateToUnicode16(char *) abstract;
		virtual char *TranslateToUTF8(char *) abstract;

		// may be very slow in the current implementation
		virtual char *Translate(Encoding *,char *) abstract;

		/** global class members **/

		static uint16 DetermineEncodingFromFirstBytes(char *bytes,int32 *skip);
		static Encoding *GetEncodingFromName(char *name);
		static Encoding *GetEncodingFromType(uint16 type);

	private:
		uint16	fType;
};

//----------------------------------------------------

class LatinEncoding : public Encoding
{
	public:
		LatinEncoding(uint16 type);
		~LatinEncoding();
	
		virtual int16 LookupLatin1(char *);
		virtual uint16 LookupUnicode16(char *);

		virtual char *NextChar(char *);

		// common translations
		virtual char *TranslateToUnicode16(char *);
		virtual char *TranslateToUTF8(char *);

		// may be very slow in the current implementation
		virtual char *Translate(Encoding *,char *);

	private:
		int16	*fLookupTable;	
};

#endif	/* ENCODING_H */
