#ifndef __FLUTE_PARSER_SELECTORS_SELECTORFACTORIMPL_H__
#define __FLUTE_PARSER_SELECTORS_SELECTORFACTORIMPL_H__

#include "SelectorFactory.h"

namespace flute
{

class SelectorFactoryImpl: public sac::SelectorFactory
{
	public:
		SelectorFactoryImpl();
		virtual ~SelectorFactoryImpl();		
		
		virtual sac::ConditionalSelector* createConditionalSelector(sac::SimpleSelector* selector, sac::Condition* condition) throw(sac::CSSException);
		virtual sac::SimpleSelector* createAnyNodeSelector() throw(sac::CSSException);
		virtual sac::SimpleSelector* createRootNodeSelector() throw(sac::CSSException);
		virtual sac::NegativeSelector* createNegativeSelector(sac::SimpleSelector* selector) throw(sac::CSSException);
		virtual sac::ElementSelector* createElementSelector(std::string& namespaceURI, std::string& tagName) throw(sac::CSSException);
		virtual sac::CharacterDataSelector* createTextNodeSelector(std::string& data) throw(sac::CSSException);
		virtual sac::CharacterDataSelector* createCDataNodeSelector(std::string& data) throw(sac::CSSException);
		virtual sac::ProcessingInstructionSelector* createProcessingInstructionSelector(std::string& target, std::string& data) throw(sac::CSSException);
		virtual sac::CharacterDataSelector* createCommentSelector(std::string& data) throw(sac::CSSException);
		virtual sac::ElementSelector* createPsuedoElementSelector(std::string& namespaceURI, std::string& tagName) throw(sac::CSSException);
		virtual sac::DescendantSelector* createDescendantSelector(sac::Selector* parent, sac::SimpleSelector* descendant) throw(sac::CSSException);
		virtual sac::DescendantSelector* createChildSelector(sac::Selector* parent, sac::SimpleSelector* child) throw(sac::CSSException);
		virtual sac::SiblingSelector* createDirectAdjecentSelector(int nodeType, sac::Selector* child, sac::SimpleSelector* directAdjacent) throw(sac::CSSException);
};

}

#endif // __FLUTE_PARSER_SELECTORS_SELECTORFACTORIMPL_H__