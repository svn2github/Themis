#ifndef __SELECTORFACTORY_H__
#define __SELECTORFACTORY_H__

#include "CSSException.h"
#include "Condition.h"
#include "Selector.h"

namespace sac
{

class SelectorFactory
{
	public:
		virtual ConditionalSelector* createConditionalSelector(SimpleSelector* selector, Condition* condition) throw(CSSException) = 0;
		virtual SimpleSelector* createAnyNodeSelector() throw(CSSException) = 0;
		virtual SimpleSelector* createRootNodeSelector() throw(CSSException) = 0;
		virtual NegativeSelector* createNegativeSelector(SimpleSelector* selector) throw(CSSException) = 0;
		virtual ElementSelector* createElementSelector(std::string& namespaceURI, std::string& tagName) throw(CSSException) = 0;
		virtual CharacterDataSelector* createTextNodeSelector(std::string& data) throw(CSSException) = 0;
		virtual CharacterDataSelector* createCDataNodeSelector(std::string& data) throw(CSSException) = 0;
		virtual ProcessingInstructionSelector* createProcessingInstructionSelector(std::string& target, std::string& data) throw(CSSException) = 0;
		virtual CharacterDataSelector* createCommentSelector(std::string& data) throw(CSSException) = 0;
		virtual ElementSelector* createPsuedoElementSelector(std::string& namespaceURI, std::string& tagName) throw(CSSException) = 0;
		virtual DescendantSelector* createDescendantSelector(Selector* parent, SimpleSelector* descendant) throw(CSSException) = 0;
		virtual DescendantSelector* createChildSelector(Selector* parent, SimpleSelector* child) throw(CSSException) = 0;
		virtual SiblingSelector* createDirectAdjecentSelector(int nodeType, Selector* child, SimpleSelector* directAdjacent) throw(CSSException) = 0;
};

}

#endif // __SELECTORFACTORY_H__