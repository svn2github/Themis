#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <string>

namespace sac
{

class Condition
{
	public:
		enum
		{
			SAC_AND_CONDITION,
			SAC_OR_CONDITION,
			SAC_NEGATIVE_CONDITION,
			SAC_POSITIONAL_CONDITION,
			SAC_ATTRIBUTE_CONDITION,
			SAC_ID_CONDITION,
			SAC_LANG_CONDITION,
			SAC_ONE_OF_ATTRIBUTE_CONDITION,
			SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION,
			SAC_CLASS_CONDITION,
			SAC_PSEUDO_CLASS_CONDITION,
			SAC_ONLY_CHILD_CONDITION,
			SAC_ONLY_TYPE_CONDITION,
			SAC_CONTENT_CONDITION
		};
		
		virtual int getConditionType() const = 0;
};

class AttributeCondition: public Condition
{
	public:
		virtual std::string getNamespaceURI() = 0;
		virtual std::string getLocalName() = 0;
		virtual bool getSpecified() = 0;
		virtual std::string getValue() = 0;
};

class CombinatorCondition: public Condition
{
	public:
		virtual Condition* getFirstCondition() = 0;
		virtual Condition* getSecondCondition() = 0;
};

class ContentCondition: public Condition
{
	public:
		virtual std::string getData() = 0;
};

class LangCondition: public Condition
{
	public:
		virtual std::string getLang() = 0;
};

class NegativeCondition: public Condition
{
	public:
		virtual Condition* getCondition() = 0;
};

class PositionalCondition: public Condition
{
	public:
		virtual int getPosition() const = 0;
		virtual bool getTypeNode() const = 0;
		virtual bool getType() const = 0;
};

}

#endif //__CONDITION_H__