#ifndef __FLUTE_PARSER_PARSEEXCEPTION_H__
#define __FLUTE_PARSER_PARSEEXCEPTION_H__

#include "CSSException.h"
#include "Token.h"
#include <string>
#include <vector>

namespace flute
{

class ParseException: public sac::CSSException
{
	public:
		 /**
		 * This constructor is used by the method "generateParseException"
		 * in the generated parser.  Calling this constructor generates
		 * a new object of this type with the fields "currentToken",
		 * "expectedTokenSequences", and "tokenImage" set.  The boolean
		 * flag "specialConstructor" is also set to true to indicate that
		 * this constructor was used to create this object.
		 * This constructor calls its super class with the empty string
		 * to force the "toString" method of parent class "Throwable" to
		 * print the error message in the form:
		 *     ParseException: <result of getMessage>
		 */
		ParseException(const Token& currentTokenVal, std::vector< std::vector<int> > expectedTokenSequencesVal, std::vector<std::string> tokenImageVal);
		
		ParseException();
		ParseException(const std::string& message);
		~ParseException(){};
	
	protected:
		bool specialConstructor;		
		/**
		 * Used to convert raw characters to their escaped version
		 * when these raw version cannot be used as part of an ASCII
		 * string literal.
		 */
		std::string add_excapes(const std::string& str);
	
	public:
		Token* currentToken;
		std::vector< std::vector<int> > expectedTokenSequences;
		std::vector<std::string> tokenImage;
		
		std::string getMessage();

		
};

}

#endif // __FLUTE_PARSER_PARSEEXCEPTION_H__