#include "ParserConstants.h"
#include "ParserImpl.h"

#include "ConditionFactoryImpl.h"
#include "SelectorFactoryImpl.h"

using namespace flute;
using namespace sac;
using namespace std;

const std::string ParserImpl::SPACE = " ";

ParserImpl::ParserImpl()
:Parser(),
 documentHandler(NULL),
 errorHandler(NULL),
 source(NULL),
 conditionFactory(new ConditionFactoryImpl()),
 selectorFactory(new SelectorFactoryImpl())
{
}

ParserImpl::~ParserImpl()
{
	delete conditionFactory;
	delete selectorFactory;
}

/*void ParserImpl::setLocale(Locale locale) throw(CSSException)
{
	throw CSSException(CSSException.SAC_NOT_SUPPORTED_ERR);
}*/

void ParserImpl::setDocumentHandler(DocumentHandler* handler)
{
	documentHandler = handler;
}

void ParserImpl::setSelectionFactory(SelectorFactory* selectorFactory)
{
}
