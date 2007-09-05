/*
Copyright (c) 2003 Z3R0 One. All Rights Reserved.

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

Original Author & Project Manager: Z3R0 One (z3r0_one@bbnk.dhs.org)
Project Start Date: October 18, 2000
*/

#include <Messenger.h>

#include <stdio.h>

#include "TPictureButton.h"

TPictureButton::TPictureButton(
	BRect rect,
	const char* name,
	BPicture* onpic,
	BPicture* overpic,
	BPicture* activepic,
	BPicture* offpic,
	BMessage* message,
	uint32 resizingmode,
	uint32 flags )
	: BView( rect, name, resizingmode, flags )
{
//	printf( "TPictureButton::TPictureButton()\n" );
	
	fMode = 0;
	fMouseOver = false;
	fInvMessage = new BMessage( *message );
	
	fOnPic = onpic;
	fOverPic = overpic;
	fActivePic = activepic;
	if( offpic == NULL )	// no disabling of button allowed/wanted
		fOffPic = NULL;
	else
		fOffPic = offpic;
}

TPictureButton::~TPictureButton()
{
//	printf( "TPictureButton::~TPictureButton()\n" );
	
	delete fInvMessage;
	
	delete fOnPic;
	delete fOverPic;
	delete fActivePic;
	delete fOffPic;
}

void
TPictureButton::Draw(
	BRect updaterect )
{
//	printf( "TPictureButton::Draw()\n" );
	
	if( Parent() == NULL )
		return;
	
	rgb_color high = HighColor();
	rgb_color parentcolor = Parent()->ViewColor();
	SetHighColor( parentcolor );
	
	FillRect( Bounds(), B_SOLID_HIGH );
	
	BPoint point( 0.0, 0.0 );
	
	switch( fMode )
	{
		case 0 : DrawPicture( fOnPic, point ); break;
		case 1 : DrawPicture( fOverPic, point ); break;
		case 2 : DrawPicture( fActivePic, point ); break;
		case 3 : DrawPicture( fOffPic, point ); break;
	}
	
	SetHighColor( high );
}

void
TPictureButton::MouseDown(
	BPoint point )
{
//	printf( "TPictureButton::MouseDown()\n" );
	
	if( fMode < 3 && fMode != 2 )
	{
		SetMode( 2 );
	}
}

void
TPictureButton::MouseMoved(
	BPoint point,
	uint32 transit,
	const BMessage* message )
{
//	printf( "TPictureButton::MouseMoved()\n" );
	if( transit == B_ENTERED_VIEW )
	{
//		printf( "  B_ENTERED_VIEW\n" );
		
		if( fMode < 3 && fMode != 1 )
		{
			fMouseOver = true;
			SetMode( 1 );
		}
	}
	if( transit == B_EXITED_VIEW )
	{
//		printf( "  B_EXITED_VIEW\n" );
		
		if( fMode < 3 && fMode != 0 )
		{
			fMouseOver = false;
			SetMode( 0 );
		}
	}
	
}

void
TPictureButton::MouseUp(
	BPoint point )
{
//	printf( "TPictureButton::MouseUp()\n" );
	
	if( fMode < 3 && fMode != 1 )
	{
		SetMode( 1 );
		BMessenger* msgr = new BMessenger( ( BHandler* )Window() );
		if( msgr->IsValid() )
			msgr->SendMessage( fInvMessage );
		delete msgr;
	}
}

void
TPictureButton::SetMode(
	int8 mode,
	bool forcemode )
{
//	printf( "TPictureButton::SetMode() [%s]\n", Name() );
//	printf( "  new mode: %d [ 0=on, 1=over, 2=active, 3=off ]\n", mode );
	
	if( fMode == mode )
		return;
	
	if( mode == 3 && fOffPic == NULL )
	{
		printf( "  WARNING! trying to set mode 3 with fOffPic = NULL! Aborting!\n" );
		return;
	}
	
	// disable the MouseOver flag
	if( mode == 3 )
		fMouseOver = false;
	
	// only needed for close tabview button so far.
	// ( reason: no offpic, so i cannot call mode 3 which would automatically set
	//   fMouseOver to false )
	if( forcemode == true )
	{
		fMode = mode;
		fMouseOver = false;
		return;
	}
	
	// "if" can happen, when button was disabled, mouse is over, and is enabled again
	if( fMouseOver == true && mode == 0 )
		fMode = 1;
	else
		fMode = mode;
	
	Draw( Bounds() );
}
