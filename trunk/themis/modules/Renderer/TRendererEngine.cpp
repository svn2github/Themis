/* See header TRenderer.h for more info */
#include <Alert.h>
#include <Screen.h>

//General
#include "CSS_HTM.h"
#include "CSS_Units.h"
#include "HtmlDefaults.h"
#include "TRenderer.h"
#include "TRenderWindow.h"
#include "UIBox.h"
#include "Utils.h"

//Elements
#include "Elements.h"


void Renderer::BroadcastPointer(TRenderView *view)
{
	//Do the Broadcasting
	BMessage message(RENDERVIEW_POINTER);
	message.AddInt32("command",COMMAND_INFO);
	message.AddString("type","TRenderView");
	message.AddPointer("data_pointer",view);
	Broadcast(MS_TARGET_ALL,&message);
}

int32 Renderer::PreProcess(void *data)
{
	preprocess_thread_param *cdata = (preprocess_thread_param *)data;

	//The new view
	TRenderView *view = new TRenderView(UIBox(800,450),cdata->document);
	view->viewID = cdata->viewID;

	BLocker *locker = new BLocker();
	locker->Lock();
	//Add the DOM & the view to the list of trees.
	cdata->renderer->UITrees.AddItem(view);
	locker->Unlock();
	delete locker;					
	//TODO: Get the DPI from User Option and give it to the view
		
	//Set the correct frame: the one specified or the one of Themis' window
/*	char *cWidth  = DefaultIfNull(document->getAttribute(CSS_WIDTH_P),CSS_WIDTH_P_IV);
	char *cHeight = DefaultIfNull(document->getAttribute(CSS_HEIGHT_P),CSS_HEIGHT_P_IV);
	
	float width = 0, height = 0;
	
	if (strcmp(cWidth,CSS_AUTO_V) == 0 || strcmp(cWidth,CSS_INHERIT_V) == 0)
		width = view_frame.Width();
	else
		width = GetFloatValue(cWidth,view->dpi);

	if (strcmp(cWidth,CSS_AUTO_V) == 0 || strcmp(cWidth,CSS_INHERIT_V) == 0)
		height = view_frame.Width();
	else
		height = GetFloatValue(cHeight,view->dpi);
				
	view->ResizeTo(width,height); */
		
	//Save the BMessenger to userIntterface
	//view->userInterface = userInterface;

	//Do the Broadcasting to give the view to the UI
	BMessage message(RENDERVIEW_POINTER);
	message.AddInt32("command",COMMAND_INFO);
	message.AddInt32("view_id",cdata->viewID);
	message.AddPointer("renderview_pointer",(void *)view);
	cdata->renderer->Broadcast(MS_TARGET_ALL,&message);
						
	//Start processing the DOM Tree
	//cdata->view->LockLooper();
	printf("RENDERER: START PROCESSING...\n");
	bigtime_t time = real_time_clock_usecs();
	cdata->renderer->Process(cdata->document,view); 
	printf("RENDERER: DONE PROCESSING in %g microseconds.\n",real_time_clock_usecs() - time);

	//Do the Broadcasting to say we are done rendering
	BMessage message2(UH_RENDER_FINISHED);
	message2.AddInt32("command",COMMAND_INFO);
	message2.AddInt32("view_id",cdata->viewID);
	cdata->renderer->Broadcast(MS_TARGET_URLHANDLER,&message2);
	
	//cdata->view->UnlockLooper();
	//Update the view
	//cdata->view->Invalidate();
		
	//Show the View, will be removed as soon as the REAL window uses the view
	//(new TRenderWindow((TRenderView *)UITrees.ItemAt(UITrees.CountItems()-1)))->Show();	
	return 0;
}

//That's the BIG one. Don't be scared and full it! Now !
void Renderer::Process( TNodePtr node, UIElement *element)
{
	TNodeListPtr children = node->getChildNodes();
	int 			length	 = 0;
	 
	if ((length = children->getLength()) != 0){
		UIBox	above_element_frame;
		if (element->nextLayer == NULL)
			element->nextLayer = new BList(length);
		printf("LENGTH = %d\n",length);
		printf("CREATING BList %d, attached to element %d\n",element->nextLayer,element); 
		for ( int i = length - 1; i >= 0; i--){
			TNodePtr child = children->item(i);
			printf("I = %d\n",i);
			//a switch of around a trillion line. That's all. ;-]
			switch(child->getNodeType()){
				case ELEMENT_NODE:{
					TElementPtr 	elementChild 	= shared_static_cast <TElement> (child);
					UIBox 			frame			= element->frame;
					UIElement		*uiChild 	 	= NULL;
					const char		*tagName 		= elementChild->getTagName().c_str();
					int8			css_display_v   = GetCSSDisplay(elementChild);
					
					
					/*We split in two: what's about tables and what is not
					if (css_display_v < 7){
						//Empty
					}
					else {
						//Empty
					}*/
					
					//Positionning system: switch (css_display_p):
						//case 1
							//That depends on the precedent brother element added
							//It should be parent's last item in his BList of children
						//case 2
							//frame.SetLeftTop(above_element_frame.MarginRect().LeftBottom());
							//Calculate frame, do alignments, etc
							//!!! See if margins should COLLAPSE !!! That should also
							//change the first line above
						//case 0:
							//Do Nothing
					
					//Give the element Min/Max values
					if (!(/*non-replaced-inline-element and */css_display_v > 6))
						GetCSSMinMaxValues(elementChild,uiChild);
					
					//Add the element to the UI Tree and go down the tree.
					if (uiChild){
						element->EAddChild(uiChild);
						printf("RENDERER: Element Added to tree\n");
						Process(child,uiChild);
					}
					else //THIS MIGHT ONLY BE TEMPORARY
						Process(child,element);
					}break;
				case TEXT_NODE:{
					TTextPtr		textChild 		= shared_static_cast <TText> (child);
					UIBox 			frame			= element->frame;
					UIElement		*uiChild 	 	= NULL ;
					uiChild = new TextElement(frame,child,textChild->getWholeText().c_str(), 
							    (BFont *)be_plain_font,SetColorSelf(RGB_BLACK),12.0);
					
					//Is it possible to have children here ?
					if (uiChild){
						element->EAddChild(uiChild);			
						printf("RENDERER: Text Element Added to tree %d\n",element->nextLayer);
						Process(child,uiChild);
					}
					else //THIS MIGHT ONLY BE TEMPORARY
						Process(child,element);
					}break;					
				default:{
					printf("RENDERER: NODE unsupported yet\n");
					}break;
			}
		}
	}
}

//Gets the correct UIBox size, without calculating it's position
/*ùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùù
	Check if ContentRect() or PaddingRect() should be
	passed to GetFloatValue() as the last param
ùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùùù*/
void Renderer::GetBoxEdges(UIBox *box, TElementPtr element, UIBox parentBox, int32 dpi)
{
	//TODO: if it's not a table
	/******************************
		1 - Get the content box
	******************************/
	const char *cWidth  = DefaultIfNull(element->getAttribute(CSS_WIDTH_P).c_str(),CSS_WIDTH_P_IV);
	const char *cHeight = DefaultIfNull(element->getAttribute(CSS_HEIGHT_P).c_str(),CSS_HEIGHT_P_IV);
	
	float width = 0, height = 0;
	
	if (strcmp(cWidth,CSS_AUTO_V) == 0){
		//TODO	
	}
	else if (strcmp(cWidth,CSS_INHERIT_V) == 0)
		width = parentBox.ContentRect().Width();
	else
		width = GetFloatValue(cWidth,dpi,parentBox.ContentRect().Width());

	if (strcmp(cHeight,CSS_AUTO_V) == 0){
		//TODO
	}
	else if (strcmp(cHeight,CSS_INHERIT_V) == 0)
		height = parentBox.ContentRect().Height();
	else
		height = GetFloatValue(cHeight,dpi,parentBox.ContentRect().Height());

	//Give box the result
	box->SetContent(width,height);
	
	/******************************
		2 - Get the Margin values
	******************************/
	float margin[4] = { CSS_MARGIN_LEFT_P_IV,CSS_MARGIN_TOP_P_IV,CSS_MARGIN_RIGHT_P_IV,CSS_MARGIN_BOTTOM_P_IV };
	
	if (element->getAttribute(CSS_MARGIN_P).c_str() != NULL){
		const char *cMargin = element->getAttribute(CSS_MARGIN_P).c_str();
		
		if (strcmp(cMargin,CSS_INHERIT_V) == 0){
			//TODO !!!! BODY { 12em 12em } !!!!!!!
		}
		else {
			//TODO		
		}
	}
	else {
		const char *cMargin[4] = {
			element->getAttribute(CSS_MARGIN_LEFT_P).c_str(),
			element->getAttribute(CSS_MARGIN_TOP_P).c_str(),
			element->getAttribute(CSS_MARGIN_RIGHT_P).c_str(),
			element->getAttribute(CSS_MARGIN_BOTTOM_P).c_str()
		};	
		
		if (cMargin[0] == NULL)
			margin[0] = CSS_MARGIN_LEFT_P_IV;
		else if (strcmp(cMargin[0],CSS_INHERIT_V) == 0)
			margin[0] = parentBox.MarginRect().Width();	
		else
			margin[0] = GetFloatValue(cMargin[0],dpi/*,param*/);

		if (cMargin[1] == NULL)
			margin[1] = CSS_MARGIN_TOP_P_IV;
		else if (strcmp(cMargin[1],CSS_INHERIT_V) == 0)
			margin[1] = parentBox.MarginRect().Width();	
		else
			margin[1] = GetFloatValue(cMargin[1],dpi/*,param*/);

		if (cMargin[2] == NULL)
			margin[2] = CSS_MARGIN_RIGHT_P_IV;
		else if (strcmp(cMargin[2],CSS_INHERIT_V) == 0)
			margin[2] = parentBox.MarginRect().Width();	
		else
			margin[2] = GetFloatValue(cMargin[2],dpi/*,param*/);

		if (cMargin[3] == NULL)
			margin[3] = CSS_MARGIN_BOTTOM_P_IV;
		else if (strcmp(cMargin[3],CSS_INHERIT_V) == 0)
			margin[3] = parentBox.MarginRect().Width();	
		else
			margin[3] = GetFloatValue(cMargin[3],dpi/*,param*/);											
	}
	
	//Give box the result
	box->SetMargin(margin[0],margin[1],margin[2],margin[3]);
	
	/******************************
		3 - Get the Padding values
	******************************/
	float padding[4] = { CSS_PADDING_LEFT_P_IV,CSS_PADDING_TOP_P_IV,CSS_PADDING_RIGHT_P_IV,CSS_PADDING_BOTTOM_P_IV };
	
	if (element->getAttribute(CSS_PADDING_P).c_str() != NULL){
		const char *cPadding = element->getAttribute(CSS_PADDING_P).c_str();
		
		if (strcmp(cPadding,CSS_INHERIT_V) == 0){
			//TODO !!!! BODY { 12em 12em } !!!!!!!
		}
		else {
			//TODO		
		}
	}
	else {
		const char *cPadding[4] = {
			element->getAttribute(CSS_PADDING_LEFT_P_IV).c_str(),
			element->getAttribute(CSS_PADDING_TOP_P_IV).c_str(),
			element->getAttribute(CSS_PADDING_RIGHT_P_IV).c_str(),
			element->getAttribute(CSS_PADDING_BOTTOM_P_IV).c_str()
		};	
			
		if (cPadding[0] == NULL)
			padding[0] = CSS_PADDING_LEFT_P_IV;
		else if (strcmp(cPadding[0],CSS_INHERIT_V) == 0)
			padding[0] = parentBox.PaddingRect().Width();
		else
			padding[0] = GetFloatValue(cPadding[0],dpi/*,param*/);

		if (cPadding[1] == NULL)
			padding[1] = CSS_PADDING_TOP_P_IV;
		else if (strcmp(cPadding[1],CSS_INHERIT_V) == 0)
			padding[1] = parentBox.PaddingRect().Width();
		else
			padding[1] = GetFloatValue(cPadding[1],dpi/*,param*/);

		if (cPadding[2] == NULL)
			padding[2] = CSS_PADDING_RIGHT_P_IV;
		else if (strcmp(cPadding[2],CSS_INHERIT_V) == 0)
			padding[2] = parentBox.PaddingRect().Width();
		else
			padding[2] = GetFloatValue(cPadding[2],dpi/*,param*/);

		if (cPadding[3] == NULL)
			padding[3] = CSS_PADDING_BOTTOM_P_IV;
		else if (strcmp(cPadding[3],CSS_INHERIT_V) == 0)
			padding[3] = parentBox.PaddingRect().Width();
		else
			padding[3] = GetFloatValue(cPadding[3],dpi/*,param*/);												
	}

	//Give box the result
	box->SetPadding(padding[0],padding[1],padding[2],padding[3]);
						
	//TODO: if it's a table
}

//See http://www.w3.org/TR/REC-CSS2/visudet.html#containing-block-details
BRect Renderer::GetContainingBlock(TElementPtr node, UIElement *parentElement)
{
	UIElement 	*non_static 	= parentElement;
	TElementPtr	non_static_n;
	
	//if { position : absolute } we return the UIBox of the first non-static parent
	//depending on the block/inline level and the direction proreties
	if (strcmp(node->getAttribute(CSS_POSITION_P).c_str(),CSS_ABSOLUTE_V) == 0){
		while (non_static){
			//Problem below, can't we have something else than a TElement ?
			if ((non_static_n = shared_static_cast <TElement> (non_static->node)) != NULL)	
				if (strcmp(non_static_n->getAttribute(CSS_POSITION_P).c_str(),CSS_STATIC_V) != 0){
					if (GetCSSLevel(non_static_n) == 1)
						return non_static->frame.PaddingRect();
					else {// css_level = 2
						//TODO: calculate frame of non_static->node->getFirstChild()
						//		calculate frame of non_static->node->getLastChild()	
						if (GetCSSPosition(non_static_n) == 1){ //ltr
							//TODO
						}	
						else { //css_position = 2 : rtl				
							//TODO
						}
					}	
				}
			non_static = non_static->parentElement;
		}
		//We have reached the root element
		return ((TRenderView *)(parentElement->parentView))->frame;
	}
		
	//If { position : fixed } we return the viewport frame
	if (strcmp(node->getAttribute(CSS_POSITION_P).c_str(),CSS_FIXED_V) == 0)
		return ((TRenderView *)(parentElement->parentView))->frame;
		
	TNodePtr 	n = parentElement->node;
	TElementPtr el_n = shared_static_cast <TElement> (n);
	
	while (el_n == NULL){ //might get infinite: first element added to a TDocumentPtr
		parentElement = parentElement->parentElement;
		n = parentElement->node;
		el_n = shared_static_cast <TElement> (n);
	}
	
	return parentElement->frame;
}

/*return 1 if position = static
		 2 				 relative
		 3 				 absolute
		 4				 fixed 		*/
int8 Renderer::GetCSSPosition(TElementPtr node) 
{
	const char *position = DefaultIfNull(node->getAttribute(CSS_POSITION_P).c_str(),CSS_POSITION_P_IV);
	
	if (strcmp(position,CSS_INHERIT_V) == 0)
		return GetCSSPosition(shared_static_cast <TElement> (node->getParentNode()));
	else if (strcmp(position,CSS_STATIC_V) == 0)
		return 1;
	else if (strcmp(position,CSS_RELATIVE_V) == 0)
		return 2;
	else if (strcmp(position,CSS_ABSOLUTE_V) == 0);
		return 3;
		
	return 4; //fixed
}

/*return 1 if direction = ltr
		 2				= rtl */
int8 Renderer::GetCSSDirection(TElementPtr node)
{
	const char *direction = DefaultIfNull(node->getAttribute(CSS_DIRECTION_P).c_str(),CSS_DIRECTION_P_IV);
	
	if (strcmp(direction,CSS_INHERIT_V) == 0)
		return GetCSSDirection(shared_static_cast <TElement> (node->getParentNode()));
	else if (strcmp(direction,CSS_LTR_V) == 0)
		return 1;
	
	return 2; //RTL
}

/*	return 1 if node is block-level and 2 if inline-level.
	The css_display param can be passed if already computed to avoid recomputing
	I assume that the following box of node is given by getNextSibling
	TODO: Not float condition */
int8 Renderer::GetCSSLevel(TElementPtr node, int8 css_display = -1)
{
	TNodePtr 	nextSibling;
	TElementPtr	nextSibling_n;	
	int8		nextSiblingPosition = 0;
	
	if (css_display == -1)
		css_display = GetCSSDisplay(node);
	
	if (css_display > 1 && css_display < 8){
		if (css_display > 3 && css_display < 6){ 	//run-in boxes || compact boxes
			if ((nextSibling = node->getNextSibling()) != NULL)
				if ((nextSibling_n = shared_static_cast <TElement> (nextSibling)) != NULL){
					nextSiblingPosition = GetCSSPosition(nextSibling_n);			
					if (nextSiblingPosition < 3 /* && NOT FLOAT*/) // not float && not absolutely-positioned
						return 2;
				}				
		}
		
		return 1; // css_display = 2 || 3 || 7 || 4-block-box || 5-block-box
	}
		
	return 2;
}

void Renderer::GetCSSMinMaxValues(TElementPtr node, UIElement *element)
{
/*
	//First the height
	const char *min = node->getAttribute(CSS_MIN_HEIGHT_P).c_str();
	const char *max = node->getAttribute(CSS_MAX_HEIGHT_P).c_str();
	
	element->minHeight = (min) ? GetFloatValue(min,dpi,param) : CSS_MIN_HEIGHT_P_IV;
	element->maxHeight = (max) ? GetFloatValue(max,dpi,param) : CSS_MAX_HEIGHT_P_IV;	 	
	
	//then the width
	min = node->getAttribute(CSS_MIN_WIDTH_P).c_str();
	max = node->getAttribute(CSS_MAX_WIDTH_P).c_str();
	
	element->minWidth = (min) ? GetFloatValue(min,dpi,param) : CSS_MIN_WIDTH_P_IV;
	element->maxWidth = (max) ? GetFloatValue(max,dpi,param) : CSS_MAX_WIDTH_P_IV;
*/
}

/*TODO: DISCUSS WITH MARK THE DYNAMIC_CAST AT THE END*/
int8 Renderer::GetCSSDisplay(TElementPtr node)
{
	const char *css_display_p = DefaultIfNull(node->getAttribute("display").c_str(),CSS_DISPLAY_P_IV);

	if (strcmp(css_display_p,CSS_INLINE_V) == 0)
		return 1;
	else if (strcmp(css_display_p,CSS_BLOCK_V) == 0)
		return 2;
	else if (strcmp(css_display_p,CSS_LIST_ITEM_V) == 0)
		return 3;
	else if (strcmp(css_display_p,CSS_RUN_IN_V) == 0)
		return 4;
	else if (strcmp(css_display_p,CSS_COMPACT_V) == 0)
		return 5;
	else if (strcmp(css_display_p,CSS_MARKER_V) == 0)
		return 6;
	else if (strcmp(css_display_p,CSS_TABLE_V) == 0)
		return 7;
	else if (strcmp(css_display_p,CSS_INLINE_TABLE_V) == 0)
		return 8;
	else if (strcmp(css_display_p,CSS_TABLE_ROW_GROUP_V) == 0)
		return 9;
	else if (strcmp(css_display_p,CSS_TABLE_HEADER_GROUP_V) == 0)
		return 10;
	else if (strcmp(css_display_p,CSS_TABLE_FOOTER_GROUP_V) == 0)
		return 11;
	else if (strcmp(css_display_p,CSS_TABLE_ROW_V) == 0)
		return 12;
	else if (strcmp(css_display_p,CSS_TABLE_COLUMN_GROUP_V) == 0)
		return 13;
	else if (strcmp(css_display_p,CSS_TABLE_COLUMN_V) == 0)
		return 14;
	else if (strcmp(css_display_p,CSS_TABLE_CELL_V) == 0)
		return 15;
	else if (strcmp(css_display_p,CSS_TABLE_CAPTION_V) == 0)
		return 16;
	else if (strcmp(css_display_p,CSS_NONE_V) == 0)
		return 0;
	else if (strcmp(css_display_p,CSS_INHERIT_V) == 0)
		return GetCSSDisplay(shared_static_cast <TElement> (node->getParentNode()));
	
	printf("GetCSSDisplay: WHAT'S THAT MAN ?!?\n");
	
	//To avoid gcc warning
	return 0;
}
