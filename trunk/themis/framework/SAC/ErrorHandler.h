#ifndef __ERRORHANDLER_H__
#define __ERRORHANDLER_H__

#include "CSSParseException.h"

namespace sac
{

class ErrorHandler
{
	public:
		virtual void warning(CSSParseException* exception) throw(CSSException) = 0;
		virtual void error(CSSParseException* exception) throw(CSSException) = 0;
		virtual void fatalError(CSSParseException* exception) throw(CSSException) = 0;
};

}

#endif // __ERRORHANDLER_H__