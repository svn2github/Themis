#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include <string>
#include "Condition.h"

namespace sac
{

class Selector
{
	public:
		enum
		{
			SAC_CONDITIONAL_SELECTOR,
			SAC_ANY_NODE_SELECTOR,
			SAC_ROOT_NODE_SELECTOR,
			SAC_NEGATIVE_SELECTOR,
			SAC_ELEMENT_NODE_SELECTOR,
			SAC_TEXT_NODE_SELECTOR,
			SAC_CDATA_SECTION_NODE_SELECTOR,
			SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR,
			SAC_COMMENT_NODE_SELECTOR,
			SAC_PSEUDO_ELEMENT_SELECTOR,
			SAC_DESCENDANT_SELECTOR,
			SAC_CHILD_SELECTOR,
			SAC_DIRECT_ADJACENT_SELECTOR
		};
		
		virtual int getSelectorType() const = 0;
};

class SimpleSelector: public Selector
{
};

class SiblingSelector: public Selector
{
	public:
		enum
		{
			ANY_NODE = 201
		};
		
		virtual int getNodeType() const = 0;
		virtual const Selector* getSelector() = 0;
		virtual SimpleSelector* getSiblingSelector() = 0;
};

class CharacterDataSelector: public SimpleSelector
{
	public:
		virtual std::string getData() = 0;
};

class ConditionalSelector: public SimpleSelector
{
	public:
		virtual const SimpleSelector* getSimpleSelector() = 0;
		virtual const Condition* getCondition() = 0;
};

class DescendantSelector: public Selector
{
	public:
		virtual Selector* getAncestorSelector() const = 0;
		virtual SimpleSelector* getSimpleSelector() const = 0;
};

class ElementSelector: public SimpleSelector
{
	public:
		virtual std::string getNamespaceURI() = 0;
		virtual std::string getLocalName() = 0;
};

class NegativeSelector: public SimpleSelector
{
	public:
		virtual const SimpleSelector* getSimpleSelector() = 0;
};

class ProcessingInstructionSelector: public SimpleSelector
{
	public:
		virtual std::string getTarget() = 0;
		virtual std::string getData() = 0;
};

}

#endif // __SELECTOR_H__