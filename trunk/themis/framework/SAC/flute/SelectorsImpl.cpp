#include "SelectorsImpl.h"

using namespace flute;
using namespace sac;
using namespace std;


DirectAdjacentSelectorImpl::DirectAdjacentSelectorImpl(Selector* child, SimpleSelector* directAdjecent)
:SiblingSelector(),
 child(child),
 directAdjacent(directAdjecent)
{
}

DirectAdjacentSelectorImpl::~DirectAdjacentSelectorImpl()
{
	// deletes are questionable still
	delete child;
	delete directAdjacent;
}

ConditionalSelectorImpl::ConditionalSelectorImpl(SimpleSelector* simpleSelector, Condition* condition)
:ConditionalSelector(),
 simpleSelector(simpleSelector),
 condition(condition)
{
}

ConditionalSelectorImpl::~ConditionalSelectorImpl()
{
	delete simpleSelector;
	delete condition;
}

DescendantSelectorImpl::DescendantSelectorImpl(Selector* parent, SimpleSelector* selector)
:DescendantSelector(),
 parent(parent),
 simpleSelector(selector)
{
}

DescendantSelectorImpl::~DescendantSelectorImpl()
{
	// deletes are questionable still
	delete parent;
	delete simpleSelector;
}

ChildSelectorImpl::ChildSelectorImpl(Selector* parent, SimpleSelector* child)
:DescendantSelector(),
 parent(parent),
 child(child)
{
}

ChildSelectorImpl::~ChildSelectorImpl()
{
	// deletes are questionable still
	delete parent;
	delete child;
}

ElementSelectorImpl::ElementSelectorImpl(const std::string& name)
:ElementSelector(),
 localName(name)
{
}

ElementSelectorImpl::~ElementSelectorImpl()
{
}

PseudoElementSelectorImpl::PseudoElementSelectorImpl(const std::string& name)
:ElementSelector(),
 localName(name)
{
}

PseudoElementSelectorImpl::~PseudoElementSelectorImpl()
{
}