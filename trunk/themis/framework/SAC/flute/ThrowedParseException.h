#ifndef __FLUTE_PARSER_THROWEDPARSEEXCEPTION_H__
#define __FLUTE_PARSER_THROWEDPARSEEXCEPTION_H__

#include <string>

namespace flute
{

class JumpException
{
	public:
		JumpException(){};
		~JumpException(){};
};

class ThrowedParseException
{
	public:
		ThrowedParseException(const std::string& message):_message(message){}
		~ThrowedParseException(){};
		std::string getMessage() const { return _message; }
	
	private:
		std::string _message;
};

}

#endif // __FLUTE_PARSER_THROWEDPARSEEXCEPTION_H__