/* See header for more info */

#include "TRenderer.h"

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
}

Renderer::~Renderer()
{
	if (cache)
		cache->Unregister(cacheUserToken);
		
	for (int32 i=0; i<UITrees.CountItems(); i++)
		delete (TRenderView *)(UITrees.RemoveItem((int32)0));
		
	for (int32 i=0; i<UITrees.CountItems(); i++)
		delete (TDocumentShared *)(DOMTrees.RemoveItem((int32)0));	

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
				case ReturnedData:{
					BString type = message->FindString("type");
					if (type == "dom"){
						void *buffer = NULL;
						TDocumentShared document;
						message->FindPointer("data_pointer", &buffer);
						if (!buffer)
							break;
						TDocumentShared *typer = (TDocumentShared *)buffer;
						document = *typer;
						PreProcess(document);
					}
					}break;
			}
		default:{
			printf("Renderer doesn't handle this broadcast\n");
			return PLUG_DOESNT_HANDLE;	
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
	return TARGET_RENDERER;
}
