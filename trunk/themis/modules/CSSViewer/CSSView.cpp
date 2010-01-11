/*
	Copyright (c) 2001 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: November 22, 2009
*/
/*	CSSView implementation
	See CSSView.h for some more information
*/

// Standard C headers
#include <stdio.h>

// BeOS headers
#include <String.h>
#include <Rect.h>
#include <ListItem.h>
#include <Box.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <ScrollView.h>

// CSSView headers
#include "CSSView.hpp"

// DOM Style headers
#include "CSSRuleList.hpp"
#include "CSSRule.hpp"
#include "CSSMediaRule.hpp"
#include "CSSStyleRule.hpp"
#include "CSSStyleDeclaration.hpp"
#include "MediaList.hpp"

// Themis headers
#include "ColumnTypes.h"
#include "TTextView.hpp"

// Constants used
const int32 SELECTION = 'slct';
const char * cMediaSpecific = "Media-specific rules";
const char * cGeneral = "General rules";

CSSView :: CSSView(CSSStyleSheetPtr aStyleSheet)
		: BWindow(BRect(100, 100, 600, 400), "CSSWindow",
				  B_TITLED_WINDOW,
				  B_CURRENT_WORKSPACE) {

	// Save the stylesheet locally.
	mStyleSheet = aStyleSheet;

	// Set background and add it to the window.
	BRect backRect = Bounds();
	BView * backGround = new BView(backRect,
								   "Background",
								   B_FOLLOW_ALL_SIDES,
								   B_WILL_DRAW);
	backGround->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(backGround);

	// Create tree view. 
	BRect treeRect = backRect;
	treeRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	treeRect.right -= 350;
	treeRect.right -= B_V_SCROLL_BAR_WIDTH;
	mTree = new BOutlineListView(treeRect,
								 "CSSView",
								 B_SINGLE_SELECTION_LIST,
								 B_FOLLOW_ALL_SIDES);

	mTree->SetSelectionMessage(new BMessage(SELECTION));
	BScrollView * scrollTree
		= new BScrollView("Scroll Tree",
						  mTree,
						  B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT,
						  0,
						  true,
						  true);
	scrollTree->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	// Add default items to the tree.
	mMediaItem =
		new BStringItem(cMediaSpecific, 0, false);
	mTree->AddItem(mMediaItem);
	mGeneralItem =
		new BStringItem(cGeneral, 0, false);
	mTree->AddItem(mGeneralItem);

	// Add the tree to background view.
	backGround->AddChild(scrollTree);

	// Create a text box for the selector text.
	BRect textRect = backRect;
	textRect.left += 155;
	textRect.bottom -= 160;
	textRect.InsetBy(3.0, 3.0);
	BBox * textBox = new BBox(textRect,
							  "TextBox",
							  B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	textBox->SetLabel("Selector Text");
	textRect = textBox->Bounds();
	textRect.InsetBy(10, 10);
	textRect.top += 5;
	textRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	textRect.right -= B_V_SCROLL_BAR_WIDTH;

	// Create text view.
	BRect textDisplay(0, 0, textRect.Width(), textRect.Height());
	mSelectorText = new TTextView(textRect,
								  "TextView",
								  textDisplay,
								  B_FOLLOW_ALL_SIDES);
	BScrollView * scrollText =
		new BScrollView("Scroll Text",
						mSelectorText,
						B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT,
						B_FRAME_EVENTS,
						true,
						true);
	scrollText->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	textBox->AddChild(scrollText);

	// Add the text to the background view.
	backGround->AddChild(textBox);

	BRect propRect = backRect;
	propRect.left += 155;
	propRect.top += 160;
	propRect.InsetBy(3.0, 3.0);
	CreatePropertiesListView(propRect);

	backGround->AddChild(mPropertiesView);

	// Add the stylesheet to the tree.
	ShowStyleSheet();

	// Show the window.
	Show();

}

CSSView :: ~CSSView() {

	int32 items = mTree->CountItems();
	for (int32 i = 0; i < items; i++) {
		delete mTree->RemoveItem((int32) 0);
	}

}

void CSSView :: ShowStyleSheet() {
	
	// Set the title of the window.
	TDOMString title = mStyleSheet->getHref();
	if (title != "") {
		SetTitle(title.c_str());
	}
	else {
		SetTitle("Unknown CSS document");
	}
	
	// Loop through the stylesheet and add the rules to the tree.
	CSSRuleListPtr rules = mStyleSheet->getCSSRules();
	unsigned long length = rules->getLength();
	for (unsigned long i = 0; i < length; i++) {
		CSSRulePtr rule = rules->item(i);
		switch (rule->getType()) {
			case CSSRule::MEDIA_RULE: {
				CSSMediaRulePtr mediaRule = shared_static_cast<CSSMediaRule>(rule);
				MediaListPtr media = mediaRule->getMedia();
				CSSRuleListPtr mediaRules = mediaRule->getCSSRules();
				unsigned long mediaLength = media->getLength();
				for (unsigned int j = 0; j < mediaLength; j++) {
					TDOMString medium = media->item(j);
					// Find out if this medium is already in the list of media.
					int32 mediaCount = mTree->CountItemsUnder(mMediaItem, true);
					bool mediumFound = false;
					int32 k = 0;
					BStringItem * mediumItem = NULL;
					while (k < mediaCount &&  !mediumFound) {
						mediumItem =
							(BStringItem *) mTree->ItemUnderAt(mMediaItem,
															   false,
															   k);
						if (mediumItem->Text() == medium.c_str()) {
							// Already in the list. No need to add it again.
							mediumFound = true;
						}
						else {
							k++;
						}
					}
					if (!mediumFound) {
						// Still need to add it.
						mediumItem = new BStringItem(medium.c_str(), 0, false);
						mTree->AddUnder(mediumItem, mMediaItem);
					}
					// Add the rules in the medium.
					AddRules(mediaRules, mediumItem);
				}
				break;
			}
			case CSSRule::STYLE_RULE: {
				CSSStyleRulePtr styleRule = shared_static_cast<CSSStyleRule>(rule);
				TDOMString selectorText = styleRule->getSelectorText();
				BStringItem * text =
					new BStringItem(selectorText.c_str(), 0, false);
				mTree->AddUnder(text, mGeneralItem);
			}
		}
	}
	
}

void CSSView :: AddRules(CSSRuleListPtr aList, BStringItem * aParent) {

	unsigned long length = aList->getLength();
	for (unsigned long i = 0; i < length; i++) {
		CSSRulePtr rule = aList->item(i);
		if (rule->getType() == CSSRule::STYLE_RULE) {
			CSSStyleRulePtr styleRule = shared_static_cast<CSSStyleRule>(rule);
			TDOMString selectorText = styleRule->getSelectorText();
			BStringItem * text =
				new BStringItem(selectorText.c_str(), 0, false);
			mTree->AddUnder(text, aParent);
		}
		else {
			printf("Unsupported rule at this level\n");
		}
	}
}

void CSSView :: CreatePropertiesListView(BRect aRect) {

	mPropertiesView = new BColumnListView(aRect,
										  "PropertiesView",
										  B_FOLLOW_ALL_SIDES,
										  0);

	BStringColumn * propName =
		new BStringColumn("Property Name",
						  100,
						  10,
						  500,
						  0);
	mPropertiesView->AddColumn(propName, 0);
	
	BStringColumn * propValue =
		new BStringColumn("Property Value",
						  100,
						  10,
						  500,
						  0);
	mPropertiesView->AddColumn(propValue, 1);
}

void CSSView :: SetPropertiesOfSelection(TDOMString aSelectorText,
										 unsigned short aRuleType,
										 TDOMString aMedium) {

	// Variable to store the style.
	CSSStyleDeclarationPtr style;

	// Loop through the stylesheet to find the right rule.
	CSSRuleListPtr rules = mStyleSheet->getCSSRules();
	unsigned long length = rules->getLength();
	bool found = false;
	unsigned long i = 0;
	while (i < length && !found) {
		CSSRulePtr rule = rules->item(i);
		if (rule->getType() == aRuleType) {
			switch (aRuleType) {
				case CSSRule::STYLE_RULE: {
					CSSStyleRulePtr styleRule = shared_static_cast<CSSStyleRule>(rule);
					TDOMString selectorText = styleRule->getSelectorText();
					if (selectorText == aSelectorText) {
						// Found the right one.
						// Get the style and exit the loop.
						style = styleRule->getStyle();
						found = true;
					}
					else {
						i++;
					}
					break;
				}
				case CSSRule::MEDIA_RULE: {
					CSSMediaRulePtr mediaRule = shared_static_cast<CSSMediaRule>(rule);
					MediaListPtr media = mediaRule->getMedia();
					unsigned long mediaLength = media->getLength();
					bool mediumFound = false;
					unsigned long j = 0;
					while (j < mediaLength && !mediumFound) {
						TDOMString medium = media->item(j);
						if (medium == aMedium) {
							mediumFound = true;
						}
						else {
							j++;
						}
					}
					if (mediumFound) {
						CSSRuleListPtr mediaRules = mediaRule->getCSSRules();
						unsigned long mediaRulesLength = mediaRules->getLength();
						unsigned long k = 0;
						bool mediaRuleFound = false;
						while (k < mediaRulesLength && !mediaRuleFound) {
							CSSRulePtr mediaRule = mediaRules->item(k);
							CSSStyleRulePtr styleRule = shared_static_cast<CSSStyleRule>(mediaRule);
							TDOMString selectorText = styleRule->getSelectorText();
							if (selectorText == aSelectorText) {
								// Found the right one.
								// Get the style and exit the loop.
								style = styleRule->getStyle();
								mediaRuleFound = true;
							}
							else {
								k++;
							}
							
						}
						found = mediaRuleFound;
					}
					if (!found) {
						i++;
					}
					break;
				}
			}
		}
		else {
			i++;
		}
	}
	
	if (found) {
		mPropertiesView->Clear();
		unsigned long propLength = style->getLength();
		for (unsigned long j = 0; j < propLength; j++) {
			TDOMString propName = style->item(j);
			BStringField * nameField = new BStringField(propName.c_str());
			TDOMString propValue = style->getPropertyValue(propName);
			BStringField * valueField = new BStringField(propValue.c_str());
			BRow * row = new BRow();
			row->SetField(nameField, 0);
			row->SetField(valueField, 1);
			mPropertiesView->AddRow(row);
		}
	}

}

void CSSView :: MessageReceived(BMessage * aMessage) {

	switch (aMessage->what) {
		case SELECTION:	{
			int32 index = 0;
			aMessage->FindInt32( "index", &index );
			if (index > -1) {
				BStringItem * item = (BStringItem *) mTree->ItemAt(index);
				BStringItem * super = (BStringItem *) mTree->Superitem(item);
				if (super != NULL) {
					BString superText = super->Text();
					if (superText.Compare(cGeneral) == 0) {
						// An item under the general item.
						// This is a style rule.
						mSelectorText->SetText(item->Text());
						SetPropertiesOfSelection(item->Text(), CSSRule::STYLE_RULE);
					}
					else {
						BStringItem * superParent =
							(BStringItem *) mTree->Superitem(super);
						if (superParent != NULL) {
							BString superParentText = superParent->Text();
							if (superParentText.Compare(cMediaSpecific) == 0) {
								// We found an item under a medium
								// This is a style rule.
								mSelectorText->SetText(item->Text());
								SetPropertiesOfSelection(item->Text(),
														 CSSRule::MEDIA_RULE,
														 super->Text());
							}
							else {
								// Shouldn't happen as is now.
								mSelectorText->SetText("");
							}
						}
						else {
							// Probably the media types
							mSelectorText->SetText("");
						}
					}
				}
			}
			else {
				mSelectorText->SetText("");
			}
			break;
		}
		default: {
			BWindow::MessageReceived(aMessage);
		}
	}

}

bool CSSView :: QuitRequested() {

	return true;

}

void CSSView :: SetStyleSheet(CSSStyleSheetPtr aStyleSheet) {

	mStyleSheet = aStyleSheet;

	// Add the stylesheet to the tree.
	ShowStyleSheet();

}
