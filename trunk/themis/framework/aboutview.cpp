/*
Copyright (c) 2002 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@yahoo.com)
Project Start Date: October 18, 2000
*/

// Standard C+ headers
#include <string.h>

// BeOS headers
#include <Application.h>
#include <Messenger.h>
#include <Window.h>
#include <String.h>
#include <TextView.h>

// Themis headers
#include "appaboutview.h"
#include "plugclass.h"
#include "aboutview.h"

aboutview *meAboutView;
#define SelectionChanged 'selc'
int sortaboutitems(const void *one,const void *two) {
	if ((one==NULL) && (two!=NULL))
		return 1;
	if ((one!=NULL) && (two==NULL))
		return -1;
	if ((one==NULL) && (two==NULL))
		return 0;
	BStringItem *a,*b;
	a=*((BStringItem**)one);
	b=*((BStringItem**)two);
	if (strcasecmp(a->Text(),"Themis Framework")==0)
		return -1;
	about_items_st *a2=NULL,*b2=NULL,*temp;
	for (int32 i=0; i<meAboutView->items->CountItems();i++) {
		temp=(about_items_st *)meAboutView->items->ItemAt(i);
		if ((a2!=NULL) && (b2!=NULL))
			break;
		if (strcasecmp(temp->listitem->Text(),a->Text())==0) {
			a2=temp;
			continue;
		}
		if (strcasecmp(temp->listitem->Text(),b->Text())==0) {
			b2=temp;
			continue;
		}
	}
	if (a2->type<b2->type)
		return -1;
	if (a2->type>b2->type)
		return 1;
	return strcasecmp(a2->listitem->Text(),b2->listitem->Text());
}
aboutview::aboutview(BRect frame, const char *name, uint32 resizem, uint32 flags)
	:BView(frame,name,resizem,flags) 
{
	meAboutView=this;
	SetViewColor(216,216,216);
	BRect r(Bounds());
	r.InsetBy(10,10);
	r.bottom-=25;
	outerbox=new BBox(r,"outer-about-box",B_FOLLOW_ALL,B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE_JUMP,B_FANCY_BORDER);
	AddChild(outerbox);
	r=Bounds();
	r.bottom-=15.0;
	r.top=r.bottom-15.0;
	r.left=(r.right/2.0)-20.0;
	r.right=(r.right/2.0)+20.0;
	OkB=new BButton(r,"Ok-button","Done",(new BMessage((uint32)B_OK)),B_WILL_DRAW|B_NAVIGABLE,B_FOLLOW_BOTTOM|B_FOLLOW_H_CENTER);
	OkB->MakeDefault(true);
	AddChild(OkB);
	items=NULL;
	r=outerbox->Bounds();
	r.right/=3.0;
	r.InsetBy(5.0,5.0);
	r.right-=B_V_SCROLL_BAR_WIDTH;
//	r.bottom-=B_H_SCROLL_BAR_HEIGHT;
	listv=new BListView(r,"about_items_listv",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL,B_WILL_DRAW|B_NAVIGABLE|B_FRAME_EVENTS);
	listv->SetSelectionMessage((new BMessage(SelectionChanged)));
	scroll=new BScrollView("about_list_scroller",listv,B_FOLLOW_H_CENTER|B_FOLLOW_TOP_BOTTOM|B_FOLLOW_LEFT,B_WILL_DRAW|B_NAVIGABLE_JUMP,false,true,B_FANCY_BORDER);
	outerbox->AddChild(scroll);
	r.left=r.right+20.0;
	r.right=outerbox->Bounds().right-5.0;
	innerbox=new BBox(r,"inner-about-box",B_FOLLOW_TOP_BOTTOM|B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE_JUMP,B_FANCY_BORDER);
	outerbox->AddChild(innerbox);
//add known items for about list.

	items=new BList();
	about_items_st *item=new about_items_st;
	item->pointer=be_app;
	item->type=AboutThemisApplication;
	item->listitem=new BStringItem("Themis Framework");
	printf("item 1: %p %s\n",item->listitem, item->listitem->Text());
	items->AddItem(item);
	item=new about_items_st;
	item->pointer=NULL;
	item->type=AboutThemisSSL;
	item->listitem=new BStringItem("SSL");
	printf("item 2: %p %s\n",item->listitem, item->listitem->Text());
	items->AddItem(item);
	firstrun=true;
}

aboutview::~aboutview() 
{
	if (items!=NULL) {
		about_items_st *cur;
		while (!items->IsEmpty()) {
			cur=(about_items_st *)items->RemoveItem((int32)0);
			delete cur;
			cur=NULL;
		}
		delete items;
		items=NULL;
	}
	
}
void aboutview::MessageReceived(BMessage *msg) 
{
	switch(msg->what)
	{
		case B_OK:
		{
			printf("done button pressed.\n");
			BMessenger msgr((BHandler *)Window());
			msgr.SendMessage(B_QUIT_REQUESTED);
		}break;
		case SelectionChanged:
		{
			int32 index=0;
			msg->FindInt32("index",&index);
			BStringItem *item=(BStringItem *)listv->ItemAt(index);
			if (item)
			{
				about_items_st *which = NULL;
				for (int32 i=0;i<items->CountItems();i++)
				{
					which=(about_items_st *)items->ItemAt(i);
					if (which->listitem==item)
						break;
					which=NULL;
				}
				
				printf("%s has been selected.\n",which->listitem->Text());
				if (innerbox->CountChildren()>0)
				{
					BView *child=NULL;
					for (int32 i=0; i<innerbox->CountChildren();i++)
					{
						child=innerbox->ChildAt(i);
						innerbox->RemoveChild(child);
						delete child;
					}
				}
				
				BRect r=innerbox->Bounds();
				r.InsetBy(5.0,5.0);
				if (which!=NULL)
				{
					switch (which->type)
					{
						case AboutThemisApplication:
						{
							innerbox->AddChild((new appaboutview(r,"About Themis View",B_FOLLOW_ALL,B_WILL_DRAW)));
						}break;
						case AboutThemisPlugIn:
						{
							BView *view= ((PlugClass*)which->pointer)->AboutView();
							bool icreated=false;
							if (view==NULL)
							{
								view=new BTextView(r,"About whatever plugin",r,B_FOLLOW_ALL,B_WILL_DRAW);
								((BTextView*)view)->MakeEditable(false);
								icreated=true;
							}
							innerbox->AddChild(view);
							if (icreated)
							{
								((BTextView*)view)->Insert("Add-on Name: ");
								PlugClass *plug=(PlugClass*)which;
								((BTextView*)view)->Insert(plug->PlugName());
								((BTextView*)view)->Insert("\nVersion: ");
								char temp[20];
								sprintf(temp,"%1.3f",plug->PlugVersion());
								((BTextView*)view)->Insert(temp);
								((BTextView*)view)->Insert("\nAdd-on ID: ");
								memset(temp,0,20);
								sprintf(temp,"%c%c%c%c",(char)plug->PlugID()>>24, (char)plug->PlugID()>>16,(char)plug->PlugID()>>8,(char)plug->PlugID());
								((BTextView*)view)->Insert(temp);
							}
							view=NULL;
						}break;
						case AboutThemisDOM:
						{
						}break;
						case AboutThemisSSL:
						{
							r.right-=B_V_SCROLL_BAR_WIDTH;
							BTextView *view;
							view=new BTextView(r,"About SSL",r,B_FOLLOW_ALL,B_WILL_DRAW);
							view->MakeEditable(false);
							view->Insert("SSL and other encryption support is provided by libcrypt:\nhttp://www.cs.auckland.ac.nz/~pgut001/cryptlib/\n");
							BScrollView *sv=new BScrollView("about_plug_scroller",view,B_FOLLOW_ALL,B_WILL_DRAW|B_NAVIGABLE_JUMP,false,true,B_FANCY_BORDER);
							innerbox->AddChild(sv);
							view=NULL;
						}break;
					}
				}
			}	// if (item)
		}break;  // case SelectionChanged:
		default:
		{
			BView::MessageReceived(msg);
		}
	}
}

void aboutview::AttachedToWindow() 
{
	OkB->SetTarget(this);
	LockLooper();
	if (firstrun)
	{
		for (int32 i=0; i<items->CountItems();i++)
			listv->AddItem(((about_items_st *)items->ItemAt(i))->listitem);
			
		firstrun=false;
		listv->SortItems(sortaboutitems);
		listv->Select(0);
		BMessenger here(this);
		BMessage selectFirstItem(SelectionChanged);
		selectFirstItem.AddInt32("index",0);
		here.SendMessage(&selectFirstItem);
	} else 
		listv->SortItems(sortaboutitems);

	listv->SetTarget(this);
	UnlockLooper();
}
