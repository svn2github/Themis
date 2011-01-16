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
#include <vector>
using namespace std;

#include <List.h>

#include "cacheplug.h"
#include "commondefs.h"
#include "plugclass.h"
#include "plugman.h"

#include "TDocument.h"
#include "TElement.h"
#include "TNamedNodeMap.h"
#include "TNode.h"
#include "TNodeList.h"
#include "TText.h"

#include "Elements.h"
#include "TRenderView.h"
#include "UIElement.h"

#ifndef _TRENDERER_H_
#define _TRENDERER_H_

extern "C" __declspec( dllexport ) status_t Initialize( void *info = NULL );
extern "C" __declspec( dllexport ) status_t Shutdown( bool now = false );
extern "C" __declspec( dllexport ) PlugClass * GetObject();

struct preprocess_thread_param {
	TDocumentPtr 	document; 
	Renderer		*renderer;
	int32			siteID;
	int32			urlID;
	int32           domID;
};

struct processing_context {
	processing_context() { isLink = false; };
	//Link Stuffs
	bool		isLink;
	BString 	uri;
};

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
				int32		dpi;
				
				//A Lock
				BLocker		*locker;		

	private:
				BList		UITrees;
				vector 		<TDocumentPtr> DOMTrees;
				
		//Below spreads the dark land of Processing methods.
		
		//Because it's a thread_func it has to be static int32 with a single void param		
		static	int32		PreProcess(void *data);
		
				void		Process(TNodePtr node, UIElement *view, processing_context constext);

				TextElement	*FindText(UIElement *element);				

				int8		GetCSSDisplay(TElementPtr node);
				int8		GetCSSLevel(TElementPtr node, int8 css_display = -1);	
				int8 		GetCSSPosition(TElementPtr node); 
				int8 		GetCSSDirection(TElementPtr node);

				void		GetCSSMinMaxValues(TElementPtr node, UIElement *element);

				BRect		GetContainingBlock(TElementPtr node, UIElement *parentElement);
				void		GetBoxEdges(UIBox *box, TElementPtr element, UIBox parentBox, int32 dpi);
			
};

#endif

