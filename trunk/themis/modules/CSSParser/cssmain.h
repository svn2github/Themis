#ifndef _css_parser_main_
#define _css_parser_main_

#include "plugclass.h"
#include "msgsystem.h" 

extern "C" __declspec(dllexport)status_t Initialize(void *info=NULL);
extern "C" __declspec(dllexport)status_t Shutdown(bool now=false);
extern "C" __declspec(dllexport)PlugClass *GetObject(void);

class CSSParserObj:public PlugClass {
	private:
	public:
		CSSParserObj(BMessage *Info);
		virtual ~CSSParserObj();
		status_t ReceiveBroadcast(BMessage *msg);
		status_t BroadcastReply(BMessage *msg);
		uint32 BroadcastTarget();
		int32 Type();
};

#endif
