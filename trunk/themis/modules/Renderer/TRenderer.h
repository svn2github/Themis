/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <List.h>

#include "cacheplug.h"
#include "commondefs.h"
#include "plugclass.h"
#include "plugman.h"

#include "TDocument.h"
#include "TElement.h"
#include "TNode.h"
#include "TNodeList.h"
#include "TText.h"

#include "TRenderView.h"
#include "UIElement.h"

#ifndef _TRENDERER_H_
#define _TRENDERER_H_

extern "C" __declspec( dllexport ) status_t Initialize( void *info = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool now = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

class Renderer : public PlugClass
{
	public:
							Renderer(BMessage *info = NULL);
		virtual 			~Renderer();
		
		//Cache system
		CachePlug			*cache;
		uint32				cacheUserToken;
		
		//Needed by PlugClass
		virtual	status_t	ReceiveBroadcast(BMessage *message);
		virtual status_t	BroadcastReply(BMessage *message);
		virtual uint32		BroadcastTarget();
		virtual bool		IsHandler();
		virtual BHandler	*Handler();
		virtual bool		IsPersistent();
		virtual uint32		PlugID();
		virtual	char		*PlugName();
		virtual	float		PlugVersion();
		virtual	int32		Type();

	private:
				BList		UITrees;
				BList 		DOMTrees;
				
				void		BroadcastPointer(TDocumentPtr document);
				void		PreProcess(int32 document_number, int32 view_number, 
									   BRect view_frame, BMessenger userInterface);
				void		Process( TNodePtr node, UIElement *view);				
};

#endif

