/* See header for more info */


#include "Globals.h"
#include "TRenderer.h"

#define 	THREAD_NAMES	7

const char *THREAD_NAME[] = {
	"Boing! BOING! Boing! BOING!"		,
	"Rendering ? You mean...Again ??"   ,
	"I'm trying to get out of here"	 	,
	"You're talking to me ?"			,
	"I am the blind Renderer"			,	
	"BeOS Powaaaaaaaaaaaaaaaa !!!!!"	,
	"Je suis un GniarkGniarkTaTete"
};

Renderer *renderer;

//Exported funcs
status_t Initialize(void *info)
{
	renderer = new Renderer((BMessage *)info);
		
	printf("Done\n");
	
	return B_NO_ERROR;
}

status_t ShutDown(bool now)
{
	delete renderer;
	
	return B_NO_ERROR;
}

PlugClass *GetObject()
{
	return renderer;
}

Renderer::Renderer(BMessage *info) : PlugClass(info,"TRenderer") 
{
	cache = (CachePlug *) PlugMan->FindPlugin( CachePlugin );
	
	if (cache)
		cacheUserToken = cache->Register( Type(), "Themis Renderer" );
	else
		cacheUserToken = 0;
	
	//Default DPI
	dpi = 72;
	
	locker = new BLocker();
}

Renderer::~Renderer()
{
	if (cache)
		cache->Unregister(cacheUserToken);
		
	for (int32 i=0; i<UITrees.CountItems(); i++)
		delete (TRenderView *)(UITrees.RemoveItem((int32)0));
		
	DOMTrees.clear();

	delete locker;
}

status_t Renderer::ReceiveBroadcast(BMessage *message)
{
	switch (message->FindInt32("command")){
		case COMMAND_INFO:{
			switch (message->what){
				case PlugInLoaded:{
					PlugClass *objet = NULL;
					message->FindPointer("plugin",(void **)&objet);
					if (objet != NULL)
						if ((objet->Type() & TARGET_CACHE) != 0)
							cache = (CachePlug *)objet;
					}break;
				case PlugInUnLoaded:{
					if (((uint32)(message->FindInt32("type")) & TARGET_CACHE) != 0)
						cache = NULL;
					}break;
				case UH_RENDER_START:{ //New way to do
					void *buffer = NULL;
					TDocumentPtr document;
					message->FindPointer("dom_tree_pointer",&buffer);
					if (!buffer)
						break;
					TDocumentPtr *typer = (TDocumentPtr *)buffer;
					document = *typer;		
					DOMTrees.push_back(document);								
					//Start Processing in a new thread so people think it'll work faster ;-))
					preprocess_thread_param param = {document,this,message->FindInt32("view_id")};
					//feeding the random generator
					srand(time(NULL));
					thread_id id = spawn_thread(PreProcess,THREAD_NAME[rand()%THREAD_NAMES],30,(void *)&param);								
					resume_thread(id);
				} break;
/*				case ReturnedData:{ //OLD WAY TO DO
					BString type = message->FindString("type");
					if (type == "dom"){
						void *buffer = NULL;
						TDocumentPtr document;
						message->FindPointer("data_pointer", &buffer);
						if (!buffer)
							break;
						TDocumentPtr *typer = (TDocumentPtr *)buffer;
						document = *typer;
						
					//Start Processing
					preprocess_thread_param param = {document,new TRenderView(UIBox(800,450),document),this};
					thread_id id = spawn_thread(PreProcess,"\"boing boing\" says the renderer",30,(void *)&param);								
					resume_thread(id);					
					}
					}break; */
/*				case R_WELCOME:{
					printf("RENDERER: R_WELCOME received\n");
					BRect rect;
					int32 doc_number, view_number;
					BMessenger userInterface;
				
					//Retrieve data sent by UI
					message->FindRect("rect",&rect);
					message->FindInt32("document_number",&doc_number);
					message->FindInt32("view_number",&view_number);
					message->FindMessenger("messenger",&userInterface);
				
					//Start Processing
					PreProcess(doc_number,view_number,rect,userInterface);
					}break; */
				default:{
					printf("Renderer doesn't handle this broadcast\n");
					return PLUG_DOESNT_HANDLE;	
					}
			}	
		}
	}
	
	return PLUG_HANDLE_GOOD;
}

status_t Renderer::BroadcastReply(BMessage *message)
{
	return B_OK;
}

uint32 Renderer::BroadcastTarget()
{
	return MS_TARGET_RENDERER;
}

bool Renderer::IsHandler()
{
	return false;
}

BHandler *Renderer::Handler()
{
	return NULL;
}

bool Renderer::IsPersistent()
{
	return true;
}

uint32 Renderer::PlugID()
{
	return 'trdr';
}

char *Renderer::PlugName()
{
	return "Themis Renderer";
}

float Renderer::PlugVersion()
{
	return 0.01;	
}

int32 Renderer::Type()
{
	return MS_TARGET_RENDERER;
}
