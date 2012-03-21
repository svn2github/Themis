/*
	Copyright (c) 2010 Mark Hellegers. All Rights Reserved.
	
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
	Class Start Date: Februari 28, 2010
*/

/*	CSSView
	Renders part of a document with css
	
	Mark Hellegers (mark@firedisk.net)
	28-02-2010
	
*/

#ifndef CSSVIEW_HPP
#define CSSVIEW_HPP

// Standard C++ headers
#include <vector>
#include <string>

// BeOS headers
#include <Handler.h>
#include <Rect.h>
#include <Point.h>
#include <be/interface/GraphicsDefs.h>

// DOM headers
#include "DOMSupport.h"

// DOM Style headers
#include "CSSStyleSheet.hpp"
#include "CSSStyleDeclaration.hpp"

// CSSRenderer headers
#include "TextBox.hpp"

// Declarations used
class BFont;
class CSSRendererView;
class CSSStyleContainer;

// Namespaces used
using std::vector;
using std::string;

class CSSView : public BHandler {
	
	private:
		TNodePtr mNode;
		CSSStyleContainer * mStyleSheets;
		CSSRendererView * mBaseView;
		vector<TextBox> mTextBoxes;
		float mLineHeight;
		float mBottomMargin;
		float mSpaceWidth;
		BFont * mFont;
		bool mInheritedFont;
		bool mClickable;
		string mHref;
		string mName;
		rgb_color mColor;
		string mBorderStyle;
		int32 mSiteId;
		int32 mUrlId;
		
		void RetrieveLink(bool aVisible = true);

	protected:
		vector<CSSView *> mChildren;
		vector<BRect> mRects;
		BRect mRect;
		BPoint mEndPoint;
		float mBorderWidth;
		float mMarginBottom;
		float mMarginRight;
		float mMarginLeft;
		BRect mListStyleRect;
		string mListStyleType;
		bool mDisplay;
		bool mBlock;
		float mRequestedWidth;
		float mRequestedHeight;

		void ApplyStyle(const TElementPtr aElement,
						const CSSStyleDeclarationPtr aStyle);
	
	public:
		CSSView(CSSRendererView * aBaseView,
				TNodePtr aNode,
				CSSStyleContainer * aStyleSheets,
				CSSStyleDeclarationPtr aStyle,
				BRect aRect,
				int32 aSiteId,
				int32 aUrlId,
				rgb_color aColor,
				BFont * aFont = NULL);
		~CSSView();
		virtual void MouseDown(BPoint aPoint);
		void Draw();
		bool Contains(BPoint aPoint);
		bool IsDisplayed();
		bool IsBlock();
		bool IsClickable();
		BRect Bounds();
		BPoint GetEndPoint();
		virtual void Layout(BRect aRect,
							BPoint aStartingPoint);
		void SplitText();
		void RetrieveResources();
		void SetWidth(float aWidth);
		void SetHeight(float aHeight);
		float GetChildWidth(unsigned int aIndex);
		void SetChildWidth(float aWidth, unsigned int aIndex);
		unsigned int GetLength() const;
};

#endif
