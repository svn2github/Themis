#ifndef __CONDITIONFACTORY_H__
#define __CONDITIONFACTORY_H__

#include "CSSException.h"
#include "Condition.h"

namespace sac
{

class ConditionFactory
{
	public:
		virtual CombinatorCondition* createAndCondition(Condition* first, Condition* second) throw(CSSException) = 0;
		virtual CombinatorCondition* createOrCondition(Condition* first, Condition* second) throw(CSSException) = 0;
		virtual NegativeCondition* createNegativeCondition(Condition* first, Condition* second) throw(CSSException) = 0;
		virtual PositionalCondition* createPositionalCondition(int position, bool typeNode, bool type) throw(CSSException) = 0;
		virtual AttributeCondition* createAttributeCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(CSSException) = 0;
		virtual AttributeCondition* createIdCondition(std::string& value) throw(CSSException) = 0;
		virtual LangCondition* createLangCondition(std::string& lang) throw(CSSException) = 0;
		virtual AttributeCondition* createOneOfAttributeCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(CSSException) = 0;
		virtual AttributeCondition* createBeginHyphenAttributeCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(CSSException) = 0;
		virtual AttributeCondition* createClassCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(CSSException) = 0;
		virtual AttributeCondition* createPseudoClassCondition(std::string& localName, std::string& namespaceURI, bool specified, std::string& value) throw(CSSException) = 0;
		virtual Condition* createOnlyChildCondition() throw(CSSException) = 0;
		virtual Condition* createOnlyTypeCondition() throw(CSSException) = 0;
		virtual ContentCondition* createContentCondtion(std::string& data) throw(CSSException) = 0;
};

}

#endif // __CONDITIONFACTORY_H__