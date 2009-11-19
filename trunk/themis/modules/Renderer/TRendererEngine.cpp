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

#define GET_ATTRIBUTE_VALUE(attribute,default_value) (attribute.get() != NULL) ? attribute->getNodeValue().c_str() : default_value; 

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
	view->siteID   = cdata->siteID;
	view->urlID    = cdata->urlID;
	view->renderer = cdata->renderer;

	cdata->renderer->locker->Lock();
	//Add the DOM & the view to the list of trees.
	cdata->renderer->UITrees.AddItem(view);
	cdata->renderer->locker->Unlock();
					
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
	BMessage *message = new BMessage(RENDERVIEW_POINTER);
	message->AddInt32("command",COMMAND_INFO);
	message->AddInt32("site_id",view->siteID);
	message->AddInt32("url_id",view->urlID);
	message->AddPointer("renderview_pointer",(void *)view);
	cdata->renderer->Broadcast(MS_TARGET_ALL,message);
						
	//Start processing the DOM Tree
	printf("RENDERER: START PROCESSING...\n");
	processing_context context;
	bigtime_t time = real_time_clock_usecs();
	cdata->renderer->Process(cdata->document,view,context); 
	printf("RENDERER: DONE PROCESSING in %lld microseconds.\n",real_time_clock_usecs() - time);
	
	//Do the Broadcasting to say we are done rendering
	message->what = SH_RENDER_FINISHED;
	message->RemoveName("renderview_pointer");

	cdata->renderer->Broadcast(MS_TARGET_SITEHANDLER,message);	
	
	//Update the view
	//cdata->view->Invalidate();
	
	delete message;

	//Free what we created in MessageReceived in TRenderer.cpp
	delete cdata;
			
	//Show the View, will be removed as soon as the REAL window uses the view
	//(new TRenderWindow((TRenderView *)UITrees.ItemAt(UITrees.CountItems()-1)))->Show();	
	return 0;
}

//That's the BIG one. Don't be scared and full it! Now !
void Renderer::Process(TNodePtr node, UIElement *element, processing_context context)
{
	//Validity Check
	if (node.get() == NULL)
		return;
		
	TNodeListPtr children = node->getChildNodes();
	int 			length	 = 0;
	 
	if ((length = children->getLength()) != 0){
		for ( int i = 0; i < length; i++){
			TNodePtr child = children->item(i);

			//a switch of around a trillion line. That's all. ;-]
			switch(child->getNodeType()){
				case ELEMENT_NODE:{
					TElementPtr 	elementChild 	= shared_static_cast <TElement> (child);
					UIBox 			frame			= element->frame;
					UIElement		*uiChild 	 	= NULL;
					const char		*tagName 		= elementChild->getTagName().c_str();
//					int8			css_display_v   = GetCSSDisplay(elementChild);
					
					
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
//					if (!(/*non-replaced-inline-element and */css_display_v > 6))
//						GetCSSMinMaxValues(elementChild,uiChild);
					
/*******P*******/	if (strcmp(tagName,"P") == 0){
						uiChild = new PElement(child);
						element->EAddChild(uiChild);
						Process(child,uiChild,context);
						break;
					}
/*******A*******/	else if (strcmp(tagName,"A") == 0){
						if (child->hasAttributes()){
							TNamedNodeMapPtr attributes = child->getAttributes();
							TNodePtr attribute;			
							Process(child,element,context);											
						}
						else {
							printf("RENDERER ERROR: A element has no attributes\n");
							Process(child,element,context);
						}
						break;
					}
/******IMG******/	else if (strcmp(tagName,"IMG") == 0){
						uiChild = new BitmapElement(frame,child,"paste_url_here");
						element->EAddChild(uiChild);
						Process(child,uiChild,context);
						break;
					}
/*****INPUT*****/	else if (strcmp(tagName,"INPUT") == 0){
						if (child->hasAttributes()) {
							TNamedNodeMapPtr attributes = child->getAttributes();
							TNodePtr attribute;
							attribute = attributes->getNamedItem("TYPE"); 
							const char *type = GET_ATTRIBUTE_VALUE(attribute,"text");
							attribute = attributes->getNamedItem("NAME");
							const char *name = GET_ATTRIBUTE_VALUE(attribute,NULL);
							attribute = attributes->getNamedItem("VALUE");
							const char *value = GET_ATTRIBUTE_VALUE(attribute,NULL);								
							
							//switch the type: text, button, etc and build the elements as expected
							//TODO: Check w3c compliance one by one and handle all the possible options
							if (type[0] == 't'){ //equivalent to strcmp(type,"text") but faster
								uiChild = new TextControlElement(frame,child,name,value,false);
							} 							
							else if (type[0] == 'b'){ //equivalent to strcmp(type,"button") but faster
								uiChild = new ButtonElement(frame,child,name,value,PUSH_BUTTON_MODE);							
							}
							else if (type[0] == 'p'){ //equivalent to strcmp(type,"password") but faster
								uiChild = new TextControlElement(frame,child,name,value,true);
							}
							else if (type[0] == 's'){ //equivalent to strcmp(type,"submit") but faster
								uiChild = new ButtonElement(frame,child,name,value,SUBMIT_BUTTON_MODE);							
							}
							else if (type[0] == 'c'){ //equivalent to strcmp(type,"checkbox") but faster
								uiChild = new CheckBoxElement(frame,child,name,value);
							}
							else if (type[0] == 'i'){ //equivalent to strcmp(type,"image") but faster
								//uiChild = ??? TODO ???  (Use IMAGE_BUTTON_MODE as buttonMode !!)
							}
							else if (type[1] == 'a'){ //equivalent to strcmp(type,"radio") but faster
								uiChild = new RadioButtonElement(frame,child,name,value);
							}
							else if (type[1] == 'e'){ //equivalent to strcmp(type,"reset") but faster
								uiChild = new ButtonElement(frame,child,name,value,RESET_BUTTON_MODE);							
							}
							else if (type[0] == 'h'){ //equivalent to strcmp(type,"hidden") but faster
								//uiChild = ??? TODO ???
							}
							else if (type[0] == 'f'){ //equivalent to strcmp(type,"file") but faster
								uiChild = new FileElement(frame,node,name,value,FILE_BUTTON_MODE);
							}																																																
							else
								printf("RENDERER ERROR: type strcmp failed: type = %s\n",type);				
						}					
						else
							printf("RENDERER ERROR: INPUT has no attributes.\n");
						
						//probably temporary
						if (uiChild){
							element->EAddChild(uiChild);						
							Process(child,uiChild,context);
						}
						else
							Process(child,element,context);
						break;
					}
/****TEXTAREA****/	else if (strcmp(tagName,"TEXTAREA") == 0){
						if (child->hasAttributes()) {	
							TNamedNodeMapPtr attributes = child->getAttributes();
							TNodePtr attribute = attributes->getNamedItem("NAME");
							const char *name = GET_ATTRIBUTE_VALUE(attribute,NULL);
							uiChild = new TextAreaElement(frame,node,name);
						}
						//probably temporary
						if (uiChild){
							element->EAddChild(uiChild);						
							Process(child,uiChild,context);
						}
						else
							Process(child,element,context);						
						break;
					}					
/*****BUTTON*****/	else if (strcmp(tagName,"BUTTON") == 0){ //IMPORTANT NOTE: By now a BUTTON tag is strictly equivalent visualy to a INPUT tag that constructs a button. Should be changed later.
						if (child->hasAttributes()) {	
							TNamedNodeMapPtr attributes = child->getAttributes();
							TNodePtr attribute;
							attribute = attributes->getNamedItem("NAME");
							const char *name = GET_ATTRIBUTE_VALUE(attribute,NULL);
							attribute = attributes->getNamedItem("VALUE");
							const char *value = GET_ATTRIBUTE_VALUE(attribute,NULL);									
							attribute = attributes->getNamedItem("TYPE"); 
							const char *type = GET_ATTRIBUTE_VALUE(attribute,"submit"); //submit is the default value
							
							t_button_mode mode;
							if 		(type[0] == 's') mode = SUBMIT_BUTTON_MODE;
							else if (type[0] == 'p') mode = PUSH_BUTTON_MODE;
							else					 mode = RESET_BUTTON_MODE;
							
							uiChild = new ButtonElement(frame,child,name,value,mode);	
						}
						else
							printf("RENDERER ERROR: BUTTON has no attributes.\n");
													
						if (uiChild){
							element->EAddChild(uiChild);
							Process(child,uiChild,context);
						}
						else
							Process(child,element,context);
						break;
					}
/******FORM*****/	else if (strcmp(tagName,"FORM") == 0){
						uiChild = new FormElement(child);
						if (uiChild){
							element->EAddChild(uiChild);
							Process(child,uiChild,context);
						}
						else
							Process(child,element,context);
						break;
					} 
/*****TITLE*****/	else if (strcmp(tagName,"TITLE") == 0){
						TNodeListPtr    loc_children = child->getChildNodes();
						int 			loc_length	 = loc_children->getLength();
						
						if (loc_length > 1)	 //Just for debug in case
							printf("RENDERER WARNING: WOOPS HEADER HAS MORE THAN 1 CHILD\n");
							
						TNodePtr loc_child = loc_children->item(0);

						if (loc_child->getNodeType() == TEXT_NODE){
							TTextPtr loc_textChild = shared_static_cast <TText> (loc_child);	
							element->parentView->SetName(loc_textChild->getWholeText().c_str());
							printf("RENDERER INFO: Name of the view found: %s !!!!!!\n",element->parentView->Name());
						}
						else { // just for debug in case
							printf("RENDERER WARNING: WOOPS TITLE CHILD IS NOT A TEXT_NODE\n");
							printf("RENDERER WARNING: TYPE IS %s\n",loc_child->getNodeName().c_str());
						}
						
						//We should jump that whole part I think by now
						Process(node->getNextSibling(),element,context);	
						break;				
					}
/*****BODY******/	else if(strcmp(tagName,"BODY") == 0){
						if (child->hasAttributes()) {	
							TNamedNodeMapPtr attributes = child->getAttributes();
							// Code is not used. Disabled to avoid the warmings.
							/*
							TNodePtr attribute = attributes->getNamedItem("TEXT"); // !! DEPRECATED !! Here for compatibility
							const char *text = GET_ATTRIBUTE_VALUE(attribute,NULL);
							attribute = attributes->getNamedItem("LINK"); 		   // !! DEPRECATED !! Here for compatibility
							const char *link = GET_ATTRIBUTE_VALUE(attribute,NULL);
							//Use this given color as text Link
							attribute = attributes->getNamedItem("ALINK"); 		   // !! DEPRECATED !! Here for compatibility
							const char *alink = GET_ATTRIBUTE_VALUE(attribute,NULL);
							//Use this given color as as unvisited text Link
							attribute = attributes->getNamedItem("VLINK"); 		   // !! DEPRECATED !! Here for compatibility
							const char *vlink = GET_ATTRIBUTE_VALUE(attribute,NULL);
							//Use this given color as active text Link														
							*/
						}					
						break;						
					}
					else {
						Process(child,element,context); //if nobody understands the tag, let's go down the tree anyway
					}
					}break;
				case TEXT_NODE:{
					TTextPtr		textChild 		= shared_static_cast <TText> (child);
					UIElement		*uiChild 	 	= NULL ;
					BFont			*font			= new BFont(be_plain_font);
					rgb_color		*high           = new rgb_color;
					SetColor(high,RGB_BLACK);
					TextElement 	*textElement    = NULL;
					if (element->nextLayer != NULL)
						textElement = FindText((UIElement *)element->nextLayer->LastItem());
					
					//TODO: Do all the cooking of font: size, direction, features, etc.
					if (textElement){
						textElement->AppendText(textChild->getWholeText().c_str(),font,high);
						printf("RENDERER DEBUG: Text Element %s appened to test %p\n",textChild->getWholeText().c_str(),textElement);
						Process(child,textElement,context);	//uiChild = textElement;					
					}
					else {
						uiChild = new TextElement(child,textChild->getWholeText().c_str(),font,high);
						element->EAddChild(uiChild);			
						printf("RENDERER DEBUG: Text Element %s Added to tree %p\n",textChild->getWholeText().c_str(),element->nextLayer);
						Process(child,uiChild,context);								    
					}
					}break;				
				default:{
					printf("RENDERER WARNING: NODE unsupported yet\n");
					}break;
			}
		}
	}
}

//TODO: To be enhanced !!
TextElement *Renderer::FindText(UIElement *element)
{
	return dynamic_cast <TextElement *> (element);
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
