#ifndef __FLUTE_PARSER_PARSER_H__
#define __FLUTE_PARSER_PARSER_H__

#include "Parser.h"
#include <string>

namespace flute
{

class ParserConstants;

class ParserImpl: public sac::Parser, public ParserConstants
{
	public:
		static const std::string SPACE;
		
		ParserImpl();
		virtual ~ParserImpl();
		
//		virtual void setLocale(Locale locale) throw(CSSException);
		virtual void setDocumentHandler(DocumentHandler* handler);
		virtual void setSelectionFactory(SelectorFactory* selectorFactory);
	
	protected:
		sac::DocumentHandler* documentHandler;
		sac::ErrorHandler* errorHandler;
		sac::InputSource* source;
		sac::ConditionFactory* conditionFactory;
		sac::SelectorFactory* selectorFactory;
	
	private:
		// temporary place holder for pseudo-element ...
		std::string pseudoElt;
};

}

#endif // __FLUTE_PARSER_PARSER_H__