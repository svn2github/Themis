#ifndef __FLUTE_PARSER_SELECTORS_CONDITIONSIMPL_H__
#define __FLUTE_PARSER_SELECTORS_CONDITIONSIMPL_H__

#include "Condition.h"
#include <string>

namespace flute
{

class AttributeConditionImpl: public sac::AttributeCondition 
{
	public:
		AttributeConditionImpl(std::string name, std::string value);
		virtual ~AttributeConditionImpl();
		
		int getConditionType() const { return sac::Condition::SAC_ATTRIBUTE_CONDITION; }
		std::string getNamespaceURI() { return string(""); }
		std::string getLocalName() { return localName; }
		bool getSpecified() { return false; }
		std::string getValue() { return value; }
	
	private:
		std::string localName;
		std::string value;
};

class BeginHyphenAttributeConditionImpl: public sac::AttributeCondition 
{
	public:
		BeginHyphenAttributeConditionImpl(std::string name, std::string value);
		virtual ~BeginHyphenAttributeConditionImpl();
		
		int getConditionType() const { return sac::Condition::SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION; }
		std::string getNamespaceURI() { return string(""); }
		std::string getLocalName() { return localName; }
		bool getSpecified() { return false; }
		std::string getValue() { return value; }
	
	private:
		std::string localName;
		std::string value;
};

class ClassConditionImpl: public sac::AttributeCondition 
{
	public:
		ClassConditionImpl(std::string value);
		virtual ~ClassConditionImpl();
		
		int getConditionType() const { return sac::Condition::SAC_CLASS_CONDITION; }
		std::string getNamespaceURI() { return string(""); }
		std::string getLocalName() { return string(""); }
		bool getSpecified() { return false; }
		std::string getValue() { return value; }
	
	private:
		std::string value;
};

class IdConditionImpl: public sac::AttributeCondition 
{
	public:
		IdConditionImpl(std::string value);
		virtual ~IdConditionImpl();
		
		int getConditionType() const { return sac::Condition::SAC_ID_CONDITION; }
		std::string getNamespaceURI() { return string(""); }
		std::string getLocalName() { return string(""); }
		bool getSpecified() { return false; }
		std::string getValue() { return value; }
	
	private:
		std::string value;
};

class OneOfAttributeConditionImpl: public sac::AttributeCondition 
{
	public:
		OneOfAttributeConditionImpl(std::string name, std::string value);
		virtual ~OneOfAttributeConditionImpl();
		
		int getConditionType() const { return sac::Condition::SAC_ONE_OF_ATTRIBUTE_CONDITION; }
		std::string getNamespaceURI() { return string(""); }
		std::string getLocalName() { return localName; }
		bool getSpecified() { return false; }
		std::string getValue() { return value; }
	
	private:
		std::string localName;
		std::string value;
};

class PseudoClassConditionImpl: public sac::AttributeCondition 
{
	public:
		PseudoClassConditionImpl(std::string value);
		virtual ~PseudoClassConditionImpl();
		
		int getConditionType() const { return sac::Condition::SAC_PSEUDO_CLASS_CONDITION; }
		std::string getNamespaceURI() { return string(""); }
		std::string getLocalName() { return string(""); }
		bool getSpecified() { return true; }
		std::string getValue() { return value; }
	
	private:
		std::string value;
};

class AndConditionImpl: public sac::CombinatorCondition
{
	public:
		AndConditionImpl(sac::Condition* first, sac::Condition* second);
		virtual ~AndConditionImpl();
		
		int getConditionType() const { return sac::Condition::SAC_AND_CONDITION; }
		sac::Condition* getFirstCondition() { return firstCondition; }
		sac::Condition* getSecondCondition() { return secondCondition; }
	
	private:
		sac::Condition* firstCondition;
		sac::Condition* secondCondition;
};

class LangConditionImpl: public sac::LangCondition
{
	public:
		LangConditionImpl(std::string language);
		virtual ~LangConditionImpl();
		
		int getConditionType() const { return sac::Condition::SAC_LANG_CONDITION; }
		std::string getLang() { return lang; }
	
	private:
		std::string lang;
};

}

#endif // __FLUTE_PARSER_SELECTORS_CONDITIONSIMPL_H__