/*	Themis Renderer

	Olivier MILLA (Methedras) methedras@online.fr
	
	This code falls under the General Themis License.
*/
#include <vector>
using namespace std;

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

				//That's the resolution, by default 72dpi
				int32			dpi;		

	private:
				BList		UITrees;
				vector 		<TDocumentPtr> DOMTrees;
				
				void		BroadcastPointer(TRenderView *view);
				
				//Below spreads the dark land of Processing methods.
				void		PreProcess(TDocumentPtr document);
				void		Process(TNodePtr node, UIElement *view);
				
				int8		GetCSSDisplay(TElementPtr node);
				int8		GetCSSLevel(TElementPtr node, int8 css_display = -1);	
				int8 		GetCSSPosition(TElementPtr node); 
				int8 		GetCSSDirection(TElementPtr node);

				void		GetCSSMinMaxValues(TElementPtr node, UIElement *element);

				BRect		GetContainingBlock(TElementPtr node, UIElement *parentElement);
				void		GetBoxEdges(UIBox *box, TElementPtr element, UIBox parentBox, int32 dpi);
			
};

#endif

