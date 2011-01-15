/*
	Copyright (c) 2003 Olivier Milla. All Rights Reserved.
	
	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or
	sell copies of the Software, and to permit persons to whom
	the Software is furnished to do so, subject to the following
	conditions:
	
	   The above copyright notice and this permission notice
	   shall be included in all copies or substantial portions
	   of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
	KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
	OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	Original Author: 	Olivier Milla (methedras@online.fr)
	Project Start Date: October 18, 2000
*/

// BeOS headers
#include <String.h>

// Themis headers
#include "framework/app.h"
#include "framework/SiteHandler.h"
#include "framework/SiteEntry.h"
#include "DOMEntry.hpp"

// Renderer headers
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
					{
						if (objet->PlugID() == 'cash')
						{
									cache = (CachePlug *)objet;
									cacheUserToken = cache->Register( Type(), "Themis Renderer" );
						}
						
					}
					
					}break;
					
				case PlugInUnLoaded:{
					if (((uint32)(message->FindInt32("type")) & TARGET_CACHE) != 0)
						cache = NULL;
					}break;
				case SH_PARSE_DOC_FINISHED: {
					BString typeOfDocument;
					message->FindString("type", &typeOfDocument);
					if (typeOfDocument == "dom") {
						int32 siteId = message->FindInt32("site_id");
						SiteEntry * site = ((App *)be_app)->GetSiteHandler()->GetEntry(siteId);
						if (site != NULL) {
							int32 domId = message->FindInt32("dom_id");
							DOMEntry * entry = (DOMEntry *) site->getEntry(domId);
							if (entry != NULL) {
								TDocumentPtr document = entry->getDocument();
								DOMTrees.push_back(document);								
								//Start Processing in a new thread
								// allocate the struct on the heap now ;) (thx emwe)
								preprocess_thread_param* param = new preprocess_thread_param;
								param->document = document;
								param->renderer = this;
								param->siteID = siteId;
								param->urlID = message->FindInt32("url_id");
								//feeding the random generator
								srand(time(NULL));
								thread_id id = spawn_thread(PreProcess,THREAD_NAME[rand()%THREAD_NAMES],30,(void *)param);								
								resume_thread(id);
							}
						}
					}
				} break;
				default:{
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
	return 0.05;	
}

int32 Renderer::Type()
{
	return 'trdr';
	//MS_TARGET_RENDERER;
}
