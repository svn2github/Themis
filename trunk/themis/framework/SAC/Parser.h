#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "ConditionFactory.h"
#include "DocumentHandler.h"
#include "ErrorHandler.h"
#include "LexicalUnit.h"
#include "SelectorFactory.h"
#include "SelectorList.h"

namespace sac
{

class Parser
{
	public:
//		virtual void setLocale(Locale locale) throw(CSSException) = 0;
		virtual void setDocumentHandler(DocumentHandler* handler) = 0;
		virtual void setSelectionFactory(SelectorFactory* selectorFactory) = 0;
		virtual void setConditionFactory(ConditionFactory* conditionFactory) = 0;
		virtual void setErrorHandler(ErrorHandler* handler) = 0;
		virtual void parseStyleSheet(InputSource* source) throw(CSSException) = 0;
		virtual void parseStyleSheet(std::string uri) throw(CSSException) = 0;
		virtual void parseStyleDeclaration(InputSource* source) throw(CSSException) = 0;
		virtual void parseRule(InputSource* source) throw(CSSException) = 0;
		virtual std::string getParserVersion() const = 0;
		virtual SelectorList* parserSelectors(InputSource* source) throw(CSSException) = 0;
		virtual LexicalUnit* parsePropertyValue(InputSource* source) throw(CSSException) = 0;
		virtual bool parsePriority(InputSource* source) throw(CSSException) = 0;
};

}

#endif // __PARSER_H__