#ifndef __DOCUMENTHANDLER_H__
#define __DOCUMENTHANDLER_H__

#include <string>
#include "CSSException.h"
#include "InputSource.h"
#include "LexicalUnit.h"
#include "SACMediaList.h"
#include "SelectorList.h"

namespace sac
{

class DocumentHandler
{
	public:
		virtual void startDocument(InputSource& source) throw(CSSException) = 0;
		virtual void endDocument(InputSource& source) throw(CSSException) = 0;
		virtual void comment(const std::string& text) throw(CSSException) = 0;
		virtual void ignorableAtRule(const std::string& atRule) throw(CSSException) = 0;
		virtual void namespaceDeclaration(const std::string& prefix, const std::string& uri) throw(CSSException) = 0;
		virtual void importStyle(const std::string& uri, SACMediaList& media, const std::string& defaultNamespaceURI) throw(CSSException) = 0;
		virtual void startMedia(SACMediaList& media) throw(CSSException) = 0;
		virtual void endMedia(SACMediaList& media) throw(CSSException) = 0;
		virtual void startPage(const std::string& name, const std::string& psuedo_page) throw(CSSException) = 0;
		virtual void endPage(const std::string& name, const std::string& psuedo_page) throw(CSSException) = 0;
		virtual void startFontFace() throw(CSSException) = 0;
		virtual void endFontFace() throw(CSSException) = 0;
		virtual void startSelector(SelectorList& selectors) throw(CSSException) = 0;
		virtual void endelector(SelectorList& selectors) throw(CSSException) = 0;
		virtual void property(const std::string& name, LexicalUnit& value, bool important) throw(CSSException) = 0;
};

}

#endif // __DOCUMENTHANDLER_H__