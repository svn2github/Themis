#ifndef _css_parser_main_
#define _css_parser_main_
#define CSS_PARSER_VERSION_NUMBER 0.01
#include "plugclass.h"
#include "msgsystem.h" 
// DOM headers
#include "TDocument.h"
#include "TNode.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"

extern "C" __declspec(dllexport)status_t Initialize(void *info=NULL);
extern "C" __declspec(dllexport)status_t Shutdown(bool now=false);
extern "C" __declspec(dllexport)PlugClass *GetObject(void);

class CSSParserObj:public PlugClass {
	private:
		bool AddDocument(TDocumentPtr Document);
		void showDOM(const TNodePtr aNode);
	
		struct cssdoc_list {
			TDocumentPtr document;
			cssdoc_list *next;
			cssdoc_list(){
//				document=NULL;
				next=NULL;
			}
			~cssdoc_list(){
//				document=NULL;
				next=NULL;
			}
		} *cssdoc_head;
	public:
		CSSParserObj(BMessage *Info);
		virtual ~CSSParserObj();
		status_t ReceiveBroadcast(BMessage *msg);
		status_t BroadcastReply(BMessage *msg);
		uint32 BroadcastTarget();
		int32 Type();
		bool IsPersistent();
		uint32 PlugID();
		char *PlugName();
		float PlugVersion();
};

#endif
