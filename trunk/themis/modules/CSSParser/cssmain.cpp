#include "cssmain.h"
#include <stdio.h>
static CSSParserObj *CSSP;
static BMessage *InitInfo;


status_t Initialize(void *info) {
	CSSP=NULL;
	InitInfo=(BMessage *)info;
	return B_OK;
}

status_t Shutdown(bool now) {
	delete CSSP;
	CSSP=NULL;
	
	InitInfo=NULL;
	
	return B_OK;
}
PlugClass *GetObject(void) {
	if (CSSP==NULL)
		CSSP=new CSSParserObj(InitInfo);
	
	return CSSP;
}

CSSParserObj::CSSParserObj(BMessage *Info):PlugClass(Info)
{
	printf("CSS Parser loaded\n");
	
}

CSSParserObj::~CSSParserObj()
{
	printf("CSS Parser shutting down.\n");
	
}
status_t CSSParserObj::ReceiveBroadcast(BMessage *msg)
{
	return 0;
	
}
status_t CSSParserObj::BroadcastReply(BMessage *msg)
{
	return 0;
}
uint32 CSSParserObj::BroadcastTarget()
{
	return MS_TARGET_CSS_PARSER;
}
int32 CSSParserObj::Type()
{
	return 'cssp';
}


