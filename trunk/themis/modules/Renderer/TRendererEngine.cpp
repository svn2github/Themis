/* See header TRenderer.h for more info */
#include <Screen.h>

//General
#include "HtmlDefaults.h"
#include "TRenderer.h"
#include "TRenderWindow.h"
#include "Utils.h"

//Elements
#include "Elements.h"

void Renderer::BroadcastPointer(TDocumentPtr document)
{
	TRenderView *view = new TRenderView(BRect(0,0,800,450));
	
	//Add the DOM & the view to the list of trees.
	UITrees.AddItem(view);
	DOMTrees.AddItem(&document);
	
	//Do the Broadcasting
	BMessage message(RENDERVIEW_POINTER);
	message.AddInt32("command",COMMAND_INFO);
	message.AddString("type","TRView");
	message.AddPointer("data_pointer",view);
	message.AddInt32("view_number",UITrees.CountItems()-1);
	message.AddInt32("document_number",DOMTrees.CountItems()-1);
	Broadcast(MS_TARGET_ALL,&message);
}

void Renderer::PreProcess(int32 document_number, int32 view_number, BRect view_frame, BMessenger userInterface)
{
	void *buffer = NULL;
	buffer = DOMTrees.ItemAt(document_number);
	TDocumentPtr *typer = (TDocumentPtr *)buffer;
	TDocumentPtr document = *typer;
	
	TRenderView *view = (TRenderView *)UITrees.ItemAt(view_number);
	
	//Set the correct frame
	view->frame = view_frame;
	view->userInterface = userInterface;
	
	//Start processing the DOM Tree
	printf("RENDERER: START PROCESSING...\n");
	bigtime_t time = real_time_clock_usecs();
	Process(document,view); 
	printf("RENDERER: DONE PROCESSING in %g microseconds.\n",real_time_clock_usecs() - time);
	
	view->Invalidate();
	
	//Show the View, will be removed as soon as the REAL window uses the view
	//(new TRenderWindow((TRenderView *)UITrees.ItemAt(UITrees.CountItems()-1)))->Show();	
}

//That's the BIG one. Don't be scared and full it! Now !
void Renderer::Process( TNodePtr node, UIElement *element)
{
	TNodeListPtr children = node->getChildNodes();
	int 			length	 = 0;
	 
	if ((length = children->getLength()) != 0){
		for ( int i = length - 1; i >= 0; i--){
			TNodePtr child = children->item(i);
			
			//a switch of around a trillion line. That's all. ;-]
			switch(child->getNodeType()){
				case ELEMENT_NODE:{
					TElementPtr 	elementChild = shared_static_cast <TElement> (child);
					//Assume that the new element is inside it's parent of at least 2 pixels
					BRect 			frame = element->frame.InsetBySelf(2,2);
					UIElement		*uiChild = NULL;
					const char		*tagName = elementChild->getTagName().c_str();
printf("RENDERER: Element %s is ",tagName);
					//Build the correct UIElement.
/*---->TITLE*/		if (strcmp(tagName,"TITLE") == 0){
printf("supported partially\n");
						//TODO:We found the page's title and should set it as the View's name
						//which view should tell the window to show that title
						//element->parentView->SetName();
						//element->parentView->userInterface.SendMessage(SITE_TITLE);
					}
/*---->TABLE*/		else if (strcmp(tagName,"TABLE") == 0){
printf("supported partially\n");
						//align the frame
						float width  = atoi(elementChild->getAttribute("WIDTH").c_str());
						float height = atoi(elementChild->getAttribute("HEIGHT").c_str());
	
						const char *align  = elementChild->getAttribute("ALIGN").c_str();
						const char *valign = elementChild->getAttribute("VALIGN").c_str();
						
						if (!align)
							align = DEFAULT_TABLE_ALIGN;
						if (!valign)
							valign = DEFAULT_TABLE_VALIGN;
						
						if (strcmp(align,"RIGHT") == 0)
							frame.left    = frame.right - width;
						else if (strcmp(align,"LEFT") == 0)
							frame.right   = frame.left + width;
						else { //CENTER
							BPoint center = GetRectCenter(frame);
							frame.left    = center.x - width/2;
							frame.right   = center.x + width/2;
						}
						if (strcmp(valign,"TOP") == 0)
							frame.bottom  = frame.top + height; 
						else if (strcmp(valign,"BOTTOM") == 0) 
							frame.top	  = frame.bottom - height; 	
						else { //CENTER
							BPoint center = GetRectCenter(frame);
							frame.top     = center.y - height/2;
							frame.bottom  = center.y + height/2;						
						}
						//Create the UIElement
						uiChild = new TableElement(frame,
										atoi(elementChild->getAttribute("CELLPADDING").c_str()),
										atoi(elementChild->getAttribute("CELLSPACING").c_str()),
										MakeRgbFromHexa(elementChild->getAttribute("BGCOLOR").c_str()),
										MakeRgbFromHexa(elementChild->getAttribute("BORDERCOLOR").c_str()));
					}
/*---->TH*/			else if (strcmp(tagName,"TH") == 0){
printf("supported partially\n");
						TableElement *table = dynamic_cast <TableElement *> (element);
						if (table)
							table->AddColumn();
						else
							printf("RENDERER: ERROR. Parent element isn't a Table\n");	
					}
/*---->TD*/			else if (strcmp(tagName,"TD") == 0){
printf("supported partially\n");
						TableElement *table = dynamic_cast <TableElement *> (element);
						if (table)
							table->AddRow();
						else
							printf("RENDERER: ERROR. Parent element isn't a Table\n");
					}
/*---->INPUT*/		else if (strcmp(tagName,"INPUT") == 0){
printf("suppported partially\n");
						if(strcmp(elementChild->getAttribute("TYPE").c_str(),"SUBMIT") == 0){
							//TODO: Determine frame. How ? It's not always said !
							uiChild = new ButtonElement(
											BRect(10,10,110,40), 
											elementChild->getAttribute("NAME").c_str(),
											elementChild->getAttribute("VALUE").c_str(),
											new BMessage());
						}
						else
							printf("RENDERER: INPUT ELEMENT unsupported yet\n");	
					}
/*---->HEAD*/		else if (strcmp(tagName,"HEAD") == 0)
printf("ignored\n");
/*---->BODY*/		else if (strcmp(tagName,"BODY") == 0)
printf("ignored\n");	
/*---->default:*/ 	else
printf("unsupported yet\n");	
				
					//Add the element to the UI Tree and go down the tree.
					if (uiChild){
						element->EAddChild(uiChild);
						printf("TRENDERER: Element Added to tree\n");
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
