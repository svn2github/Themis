#ifndef __FLUTE_PARSER_PARSERCONSTANTS_H__
#define __FLUTE_PARSER_PARSERCONSTANTS_H__

#include <string>

namespace flute
{

class ParserConstants
{
	public:
		enum
		{
			END_OF_FILE = 0,
			S = 1,
			CDO = 5,
			CDC = 6,
			LBRACE = 7,
			RBRACE = 8,
			DASHMATCH = 9,
			INCLUDES = 10,
			EQ = 11,
			PLUS = 12,
			MINUS = 13,
			COMMA = 14,
			SEMICOLON = 15,
			PRECEDES = 16,
			DIV = 17,
			LBRACKET = 18,
			RBRACKET = 19,
			ANY = 20,
			DOT = 21,
			LPARAN = 22,
			RPARAN = 23,
			COLON = 24,
			NONASCII = 25,
			H = 26,
			UNICODE = 27,
			ESCAPE = 28,
			NMSTART = 29,
			NMCHAR = 30,
			STRINGCHAR = 31,
			D = 32,
			NAME = 33,
			STRING = 34,
			IDENT = 35,
			NUMBER = 36,
			_URL = 37,
			URL = 38,
			PERCENTAGE = 39,
			PT = 40,
			MM = 41,
			CM = 42,
			PC = 43,
			IN = 44,
			PX = 45,
			EMS = 46,
			EXS = 47,
			DEG = 48,
			RAD = 49,
			GRAD = 50,
			MS = 51,
			SECOND = 52,
			HZ = 53,
			KHZ = 54,
			DIMEN = 55,
			HASH = 56,
			IMPORT_SYM = 57,
			MEDIA_SYM = 58,
			CHARSET_SYM = 59,
			PAGE_SYM = 60,
			FONT_FACE_SYM = 61,
			ATKEYWORD = 62,
			IMPORTANT_SYM = 63,
			RANGE0 = 64,
			RANGE1 = 65,
			RANGE2 = 66,
			RANGE3 = 67,
			RANGE4 = 68,
			RANGE5 = 69,
			RANGE6 = 70,
			RANGE = 71,
			UNI = 72,
			UNICODERANGE = 73,
			FUNCTION = 74,
			UNKNOWN = 75
		};
		
		enum
		{
			DEFAULT = 0,
			IN_COMMENT = 1
		};
		
		static std::string tokenImage[];
};

}

#endif //__FLUTE_PARSER_PARSERCONSTANTS_H__