#include "SelectorFactoryImpl.h"
#include "SelectorsImpl.h"

using namespace flute;
using namespace sac;
using namespace std;


SelectorFactoryImpl::SelectorFactoryImpl()
:SelectorFactory()
{
}

SelectorFactoryImpl::~SelectorFactoryImpl()
{
}

ConditionalSelector* SelectorFactoryImpl::createConditionalSelector(SimpleSelector* selector, Condition* condition) throw(CSSException)
{
	return new ConditionalSelectorImpl(selector, condition);
}

SimpleSelector* SelectorFactoryImpl::createAnyNodeSelector() throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

SimpleSelector* SelectorFactoryImpl::createRootNodeSelector() throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

NegativeSelector* SelectorFactoryImpl::createNegativeSelector(SimpleSelector* selector) throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

ElementSelector* SelectorFactoryImpl::createElementSelector(string& namespaceURI, string& tagName) throw(CSSException)
{
	if (namespaceURI != "") 
	{
	    throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
	} 
	else
	{
	    return new ElementSelectorImpl(tagName);
	}
	
}

CharacterDataSelector* SelectorFactoryImpl::createTextNodeSelector(string& data) throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

CharacterDataSelector* SelectorFactoryImpl::createCDataNodeSelector(string& data) throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

ProcessingInstructionSelector* SelectorFactoryImpl::createProcessingInstructionSelector(string& target, string& data) throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

CharacterDataSelector* SelectorFactoryImpl::createCommentSelector(string& data) throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

ElementSelector* SelectorFactoryImpl::createPsuedoElementSelector(string& namespaceURI, string& tagName) throw(CSSException)
{
	if(namespaceURI != "")
		throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
	else
		return new PseudoElementSelectorImpl(tagName);
}

DescendantSelector* SelectorFactoryImpl::createDescendantSelector(Selector* parent, SimpleSelector* descendant) throw(CSSException)
{
	return new DescendantSelectorImpl(parent, descendant);
}

DescendantSelector* SelectorFactoryImpl::createChildSelector(Selector* parent, SimpleSelector* child) throw(CSSException)
{
	return new ChildSelectorImpl(parent, child);
}

SiblingSelector* SelectorFactoryImpl::createDirectAdjecentSelector(int nodeType, Selector* child, SimpleSelector* directAdjacent) throw(CSSException)
{
	if(nodeType != 1)
		throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
	else
		return new DirectAdjacentSelectorImpl(child, directAdjacent);
}