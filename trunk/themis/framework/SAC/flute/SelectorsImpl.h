#ifndef __FLUTE_PARSER_SELECTORS_SELECTORSIMPL_H__
#define __FLUTE_PARSER_SELECTORS_SELECTORSIMPL_H__

#include "Selector.h"
#include <string>

namespace flute
{

class DirectAdjacentSelectorImpl: public sac::SiblingSelector
{
	public:
		DirectAdjacentSelectorImpl(sac::Selector* child, sac::SimpleSelector* directAdjecent);
		virtual ~DirectAdjacentSelectorImpl();
	
		int getSelectorType() const { return sac::Selector::SAC_DIRECT_ADJACENT_SELECTOR; }
		int getNodeType() const { return 1; }
		sac::Selector* getSelector() { return child; }
		sac::SimpleSelector* getSiblingSelector() { return directAdjacent; }
		
	private:
		sac::Selector* child;
		sac::SimpleSelector* directAdjacent;
};

class ConditionalSelectorImpl: public sac::ConditionalSelector
{
	public:
		ConditionalSelectorImpl(sac::SimpleSelector* simpleSelector, sac::Condition* condition);
		virtual ~ConditionalSelectorImpl();
		
		int getSelectorType() const { return sac::Selector::SAC_CONDITIONAL_SELECTOR; }
		sac::SimpleSelector* getSimpleSelector() { return simpleSelector; }
		sac::Condition* getCondition() { return condition; }

	private:
		sac::SimpleSelector* simpleSelector;
		sac::Condition* condition;
};

class DescendantSelectorImpl: public sac::DescendantSelector
{
	public:
		DescendantSelectorImpl(sac::Selector* parent, sac::SimpleSelector* selector);
		virtual ~DescendantSelectorImpl();
		
		int getSelectorType() const { return sac::Selector::SAC_DESCENDANT_SELECTOR; }
		sac::Selector* getAncestorSelector() const { return parent; }
		sac::SimpleSelector* getSimpleSelector() const { return simpleSelector; }

	private:
		sac::Selector* parent;
		sac::SimpleSelector* simpleSelector;
};

class ChildSelectorImpl: public sac::DescendantSelector
{
	public:
		ChildSelectorImpl(sac::Selector* parent, sac::SimpleSelector* child);
		virtual ~ChildSelectorImpl();
		
		int getSelectorType() const { return sac::Selector::SAC_CHILD_SELECTOR; }
		sac::Selector* getAncestorSelector() const { return parent; }
		sac::SimpleSelector* getSimpleSelector() const { return child; }

	private:
		sac::Selector* parent;
		sac::SimpleSelector* child;	
};

class ElementSelectorImpl: public sac::ElementSelector
{
	public:
		ElementSelectorImpl(const std::string& name);
		virtual ~ElementSelectorImpl();
		
		int getSelectorType() const { return sac::Selector::SAC_ELEMENT_NODE_SELECTOR; }
		/**
		 * Returns the
		 * <a href="http://www.w3.org/TR/REC-xml-names/#dt-NSName">namespace
		 * URI</a> of this element selector.
		 * <p><code>""</code> if this element selector can match any namespace.</p>
		 */
		std::string getNamespaceURI() { return std::string(""); }
		/**
		 * Returns the
		 * <a href="http://www.w3.org/TR/REC-xml-names/#NT-LocalPart">local part</a>
		 * of the
		 * <a href="http://www.w3.org/TR/REC-xml-names/#ns-qualnames">qualified
		 * name</a> of this element.
		 * <p><code>NULL</code> if this element selector can match any element.</p>
		 * </ul>
		 */
		std::string getLocalName() { return localName; }
	
	private:
		std::string localName;
};


class PseudoElementSelectorImpl: public sac::ElementSelector
{
	public:
		PseudoElementSelectorImpl(const std::string& name);
		virtual ~PseudoElementSelectorImpl();
		
		int getSelectorType() const { return sac::Selector::SAC_PSEUDO_ELEMENT_SELECTOR; }
		/**
		 * Returns the
		 * <a href="http://www.w3.org/TR/REC-xml-names/#dt-NSName">namespace
		 * URI</a> of this element selector.
		 * <p><code>""</code> if this element selector can match any namespace.</p>
		 */
		std::string getNamespaceURI() { return std::string(""); }
		/**
		 * Returns the
		 * <a href="http://www.w3.org/TR/REC-xml-names/#NT-LocalPart">local part</a>
		 * of the
		 * <a href="http://www.w3.org/TR/REC-xml-names/#ns-qualnames">qualified
		 * name</a> of this element.
		 * <p><code>NULL</code> if this element selector can match any element.</p>
		 * </ul>
		 */
		std::string getLocalName() { return localName; }
	
	private:
		std::string localName;
};

}

#endif // __FLUTE_PARSER_SELECTORS_SELECTORSIMPL_H__