/* See header TRenderer.h for more info */
#include <Screen.h>

//General
#include "TRenderer.h"
#include "TRenderWindow.h"
#include "Utils.h"

//Elements
#include "Elements.h"

/*This method should:
	_ Broadcast that we have a view but before retrieve the name of the 
	  page and set it to the view's name so the Window will know
	  what's the title even if tha page is still empty.
	_ Add the TRenderView and teh DOM to the lists of trees.
	_ Process the TDocumentShared fields.*/
void Renderer::PreProcess(TDocumentShared document)
{
	TRenderView *view = new TRenderView(BScreen().Frame());

	//Add the DOM & the view to the list of trees.
	UITrees.AddItem(view);
	DOMTrees.AddItem(&document);

	//Get the Page's name and set it as the view's title
	view->SetName(document->getNodeName().c_str());
	
	//Do the Broadcasting
	BMessage *message = new BMessage(ReturnedData);
	message->AddInt32("command",COMMAND_INFO);
	message->AddString("type","TRView");
	message->AddPointer("data_pointer",view);
	Broadcast(MS_TARGET_ALL,message);
	
	delete message;
	
	//Start processing the DOM Tree
	Process(document,view); 
}

//That's the BIG one. Don't be scared and full it! Now !
void Renderer::Process( TNodeShared node, UIElement *element)
{
	TNodeListShared children = node->getChildNodes();
	int 			length	 = 0;
	 
	if ((length = children->getLength()) != 0){
		for ( int i = length - 1; i >= 0; i--){
			TNodeShared child = make_shared(children->item(i));
			
			//a switch of around a trillion line. That's all. ;-]
			switch(child->getNodeType()){
				case ELEMENT_NODE:{
					TElementShared 	elementChild = shared_static_cast <TElement> (child);
					BRect 			frame;
					UIElement		*uiChild = NULL;
					//Build the correct UIElement.
/*---->TITLE*/		if (strcmp(elementChild->getTagName().c_str(),"TITLE") == 0){
						//TODO:We found the page's title and should set it as the View's name
						//which view should tell the window to show that title
					}
/*---->TABLE*/		else if (strcmp(elementChild->getTagName().c_str(),"TABLE") == 0){
						//TODO:Determine frame
						uiChild = new TableElement(frame,
										atoi(elementChild->getAttribute("CELLPADDING").c_str()),
										atoi(elementChild->getAttribute("CELLSPACING").c_str()),
										MakeRgbFromHexa(elementChild->getAttribute("BGCOLOR").c_str()),
										MakeRgbFromHexa(elementChild->getAttribute("BORDERCOLOR").c_str()));
					}
/*---->INPUT*/		else if (strcmp(elementChild->getTagName().c_str(),"INPUT") == 0){
						if(strcmp(elementChild->getAttribute("TYPE").c_str(),"SUBMIT") == 0){
							//TODO: Determine frame. How ? It's not always said !
							uiChild = new ButtonElement(
											BRect(10,10,110,40), 
											elementChild->getAttribute("NAME").c_str(),
											elementChild->getAttribute("VALUE").c_str(),
											new BMessage());
						}
						else
							printf("INPUT ELEMENT unsupported yet\n");	
					}	
/*---->default:*/ 	else
						printf("ELEMENT unsupported yet\n");	
				
					//Add the element to the UI Tree and go down the tree.
					if (uiChild){
						element->EAddChild(uiChild);
						Process(child,uiChild);
					}
					}break;
				default:{
					printf("NODE unsupported yet\n");
					}break;
			}
		}
	}
	
	//Show the View, will be removed as soon as the REAL window uses the view
	(new TRenderWindow((TRenderView *)UITrees.ItemAt(UITrees.CountItems()-1)))->Show();
}
