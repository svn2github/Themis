#ifndef __FLUTE_PARSER_SELECTORS_CONDITIONFACTORIMPL_H__
#define __FLUTE_PARSER_SELECTORS_CONDITIONFACTORIMPL_H__

#include "ConditionFactory.h"

namespace flute
{

class ConditionFactoryImpl: public sac::ConditionFactory
{
	public:
		ConditionFactoryImpl();
		virtual ~ConditionFactoryImpl();
		
		virtual sac::CombinatorCondition* createAndCondition(sac::Condition* first, sac::Condition* second) throw(sac::CSSException);
		virtual sac::CombinatorCondition* createOrCondition(sac::Condition* first, sac::Condition* second) throw(sac::CSSException);
		virtual sac::NegativeCondition* createNegativeCondition(sac::Condition* first, sac::Condition* second) throw(sac::CSSException);
		virtual sac::PositionalCondition* createPositionalCondition(int position, bool typeNode, bool type) throw(sac::CSSException);
		virtual sac::AttributeCondition* createAttributeCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(sac::CSSException);
		virtual sac::AttributeCondition* createIdCondition(std::string& value) throw(sac::CSSException);
		virtual sac::LangCondition* createLangCondition(std::string& lang) throw(sac::CSSException);
		virtual sac::AttributeCondition* createOneOfAttributeCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(sac::CSSException);
		virtual sac::AttributeCondition* createBeginHyphenAttributeCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(sac::CSSException);
		virtual sac::AttributeCondition* createClassCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(sac::CSSException);
		virtual sac::AttributeCondition* createPseudoClassCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(sac::CSSException);
		virtual sac::Condition* createOnlyChildCondition() throw(sac::CSSException);
		virtual sac::Condition* createOnlyTypeCondition() throw(sac::CSSException);
		virtual sac::ContentCondition* createContentCondtion(std::string& data) throw(sac::CSSException);
};

}

#endif // __FLUTE_PARSER_SELECTORS_SELECTORFACTORIMPL_H__