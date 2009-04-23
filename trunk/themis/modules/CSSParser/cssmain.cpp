// Standard C headers
#include <stdio.h>

// CSS Parser headers
#include "cssmain.h"
#include "commondefs.h"

#include "StyleSheet.h"
#include "InputStream.h"

// Themis headers
#include "PrefsDefs.h"

#define MULTIPLE_DOCUMENTS 0


static CSSParserObj *CSSP;
static BMessage *InitInfo;

BMessage * appSettings;

status_t Initialize( void * aInfo )	{
	
	BMessage ** appSettings_p;
	if (aInfo != NULL) {
		BMessage * message = (BMessage *) aInfo;
		if (message->HasPointer("settings_message_ptr"))	{
			message->FindPointer( "settings_message_ptr", (void **) & appSettings_p );
			appSettings = *appSettings_p;
		}
		InitInfo=(BMessage *) aInfo;
		CSSP = new CSSParserObj(message);
	}
	else {
		CSSP = new CSSParserObj();
	}
	
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
	cssdoc_head=NULL;
}

CSSParserObj::~CSSParserObj()
{
	printf("CSS Parser shutting down.\n");
	cssdoc_list *last;
	while (cssdoc_head!=NULL) {
		last=cssdoc_head->next;
		delete cssdoc_head;
		cssdoc_head=last;
	}
	
}
void CSSParserObj::showDOM(const TNodePtr aNode)
{
	TNodeListPtr children=aNode->getChildNodes();
	int length=children->getLength();
	for (int i=length -1; i>=0; i--) {
		TNodePtr child=children->item(i);
		if (child->getNodeType()==ELEMENT_NODE) {
			
			printf("node: %s\n",child->getNodeName().c_str());
			showDOM(child);
			
		}
		
	}
}

bool CSSParserObj::AddDocument(TDocumentPtr Document) 
{
	bool success=false;
	
#if MULTIPLE_DOCUMENTS == 1
	printf("CSS Parser: Multiple Document Mode\n");
	cssdoc_list *nu=new cssdoc_list;
	nu->document=Document;
	cssdoc_list *last=cssdoc_head;
	if (last==NULL) {
		cssdoc_head=nu;
		success=true;
	} else {
		while (last->next!=NULL)
			last=last->next;
		last->next=nu;
		success=true;
	}
	
#else
	printf("CSS Parser: Single Document Mode\n");
	
	if (cssdoc_head==NULL) {
		cssdoc_head=new cssdoc_list;
	}
	cssdoc_head->document=Document;
	success=true;
	
#endif
	printf("DOM Children:\n");
	showDOM(Document);
		
	return success;
	
}

status_t CSSParserObj::ReceiveBroadcast(BMessage *msg)
{
	int32 command=0;
	status_t status=B_ERROR;
	
	msg->FindInt32("command",&command);
	switch (command) {
		case COMMAND_INFO: {
			printf("CSS Parser has received information.\n");
			msg->PrintToStream();
			status=B_OK;
			switch (msg->what) {
				case PlugInLoaded: {
					printf("Plug-in loaded...\n");
					
				}break;
				case PlugInUnLoaded: {
					printf("Plug-in unloaded...\n");
					
				}break;
				case CSS_CHANGED_PARSER:	{
					Debug( "Request to change base css file", PlugID() );
					if ( appSettings != NULL )	{
						const char * path;
						appSettings->FindString( kPrefsActiveCSSPath, &path );
						string cssLoad = "Loading new CSS file: ";
						cssLoad += path;
						Debug( cssLoad.c_str(), PlugID() );
						FileInputStream input(path);
						
						if (input.InitCheck() >= RC_OK)
						{
							StyleSheet css(CSS_USER_TYPE,&input);
						}

						Debug( "New CSS file loaded", PlugID() );
					}
					break;
				}
				case ReturnedData: {
					PlugClass *broadcaster=NULL;
					msg->FindPointer("_broadcast_origin_pointer_",(void**)&broadcaster);
					uint32 target=0;
					if (broadcaster!=NULL)
						target=broadcaster->PlugID();
					if (target=='html') {
						BString type;
						msg->FindString("type",&type);
						if (type!="dom") {
							printf("CSS Parser: Not a DOM object.\n");
							break;
						}
						void *document=NULL;
						msg->FindPointer("data_pointer",&document);
						if (document) {
							TDocumentPtr *dr_ptr=(TDocumentPtr*)document;
							AddDocument(*dr_ptr);
							
						}
						document=NULL;
						
					}
					broadcaster=NULL;
					target=0;
				}break;
				
				default:
					printf("Unknown information packet.\n");
					status=B_ERROR;
			}
			
		}break;
		default:
			printf("Unsupported message.\n");
	}
	
	return status;
	
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

bool CSSParserObj::IsPersistent() 
{
	return true;
}

uint32 CSSParserObj::PlugID()
{
	return 'cssp';
}

char *CSSParserObj::PlugName()
{
	return "CSS Parser";
}

float CSSParserObj::PlugVersion()
{
	return CSS_PARSER_VERSION_NUMBER;
	
}


