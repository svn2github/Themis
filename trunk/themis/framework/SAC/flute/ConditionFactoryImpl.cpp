#include "ConditionFactoryImpl.h"
#include "ConditionImpl.h"

using namespace flute;
using namespace sac;
using namespace std;

ConditionFactoryImpl::ConditionFactoryImpl()
:ConditionFactory()
{
}

ConditionFactoryImpl::~ConditionFactoryImpl()
{
}

CombinatorCondition* ConditionFactoryImpl::createAndCondition(Condition* first, Condition* second) throw(CSSException)
{
	return new AndConditionImpl(first, second);
}

CombinatorCondition* ConditionFactoryImpl::createOrCondition(Condition* first, Condition* second) throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

NegativeCondition* ConditionFactoryImpl::createNegativeCondition(Condition* first, Condition* second) throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

PositionalCondition* ConditionFactoryImpl::createPositionalCondition(int position, bool typeNode, bool type) throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

AttributeCondition* ConditionFactoryImpl::createAttributeCondition(string& localName, string& namespaceURI, bool specified, string& value) throw(CSSException)
{
	if(namespaceURI != "" || specified)
		throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
	else
		return new AttributeConditionImpl(localName, value);
	
	// can't reach next line
	return 0;
}

AttributeCondition* ConditionFactoryImpl::createIdCondition(string& value) throw(CSSException)
{
	return new IdConditionImpl(value);
}

LangCondition* ConditionFactoryImpl::createLangCondition(string& lang) throw(CSSException)
{
	return new LangConditionImpl(lang);
}

AttributeCondition* ConditionFactoryImpl::createOneOfAttributeCondition(string& localName, string& namespaceURI, bool specified, string& value) throw(CSSException)
{
	if(namespaceURI != "" || specified)
		throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
	else
		return new OneOfAttributeConditionImpl(localName, value);
	
	// can't reach next line
	return 0;	
}

AttributeCondition* ConditionFactoryImpl::createBeginHyphenAttributeCondition(string& localName, string& namespaceURI, bool specified, string& value) throw(CSSException)
{
	if(namespaceURI != "" || specified)
		throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
	else
	    return new BeginHyphenAttributeConditionImpl(localName, value);
	
	// can't reach next line
	return 0;	
}

AttributeCondition* ConditionFactoryImpl::createClassCondition(string& localName, string& namespaceURI, bool specified, string& value) throw(CSSException)
{
	return new ClassConditionImpl(value);
}

AttributeCondition* ConditionFactoryImpl::createPseudoClassCondition(string& localName, string& namespaceURI, bool specified, string& value) throw(CSSException)
{
	return new PseudoClassConditionImpl(value);
}

Condition* ConditionFactoryImpl::createOnlyChildCondition() throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

Condition* ConditionFactoryImpl::createOnlyTypeCondition() throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}

ContentCondition* ConditionFactoryImpl::createContentCondtion(string& data) throw(CSSException)
{
	throw CSSException(CSSException::SAC_NOT_SUPPORTED_ERR);
}
		