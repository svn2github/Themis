#ifndef __FLUTE_PARSER_LOCATOR_H__
#define __FLUTE_PARSER_LOCATOR_H__

#include "Locator.h"
#include <string>

namespace sac
{
	class Parser;
}
namespace flute
{

class Token;

class LocatorImpl: public sac::Locator
{
	public:
		LocatorImpl(sac::Parser* p);
		LocatorImpl(sac::Parser* p, Token* tok);
		LocatorImpl(sac::Parser* p, int line, int column);
		virtual ~LocatorImpl();
		
		virtual std::string getURI() const { return uri; }
		virtual int getLineNumber() const { return line; }
		virtual int getColumnNumber() const{ return column; }
		
		LocatorImpl* reInit(sac::Parser* p);
		LocatorImpl* reInit(sac::Parser* p, int line, int column);		
		
	private:
		std::string uri;
		int line;
		int column;
};

}

#endif // __FLUTE_PARSER_LOCATOR_H__