#include "ConditionImpl.h"

using namespace flute;
using namespace sac;
using namespace std;

AttributeConditionImpl::AttributeConditionImpl(string name, string value)
:AttributeCondition(),
localName(name),
value(value)
{
}

AttributeConditionImpl::~AttributeConditionImpl()
{
}

BeginHyphenAttributeConditionImpl::BeginHyphenAttributeConditionImpl(string name, string value)
:AttributeCondition(),
localName(name),
value(value)
{
}

BeginHyphenAttributeConditionImpl::~BeginHyphenAttributeConditionImpl()
{
}

ClassConditionImpl::ClassConditionImpl(string value)
:AttributeCondition(),
value(value)
{
}

ClassConditionImpl::~ClassConditionImpl()
{
}

IdConditionImpl::IdConditionImpl(string value)
:AttributeCondition(),
value(value)
{
}

IdConditionImpl::~IdConditionImpl()
{
}

OneOfAttributeConditionImpl::OneOfAttributeConditionImpl(string name, string value)
:AttributeCondition(),
localName(name),
value(value)
{
}

OneOfAttributeConditionImpl::~OneOfAttributeConditionImpl()
{
}

PseudoClassConditionImpl::PseudoClassConditionImpl(string value)
:AttributeCondition(),
value(value)
{
}

PseudoClassConditionImpl::~PseudoClassConditionImpl()
{
}

AndConditionImpl::AndConditionImpl(Condition* first, Condition* second)
:CombinatorCondition(),
firstCondition(first),
secondCondition(second)
{
}

AndConditionImpl::~AndConditionImpl()
{
	delete firstCondition;
	delete secondCondition;
}

LangConditionImpl::LangConditionImpl(string language)
:LangCondition(),
lang(language)
{
}

LangConditionImpl::~LangConditionImpl()
{
}