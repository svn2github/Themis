/* ThemisUrlView.cpp */

// BeOS headers
#include <Clipboard.h>
#include <String.h>
#include <Window.h>
#include <Polygon.h>

// C/C++ headers
#include <string.h>
#include <iostream>
#include <iostream.h>

// myheaders
#include "ThemisUrlView.h"
#include "win.h"
#include "app.h"

class ThemisUrlTextView;

ThemisUrlView::ThemisUrlView(
	BRect frame,
	const char* name,
	uint32 resizingmode,
	uint32 flags )
	: BView(
		frame,
		name,
		resizingmode,
		flags )
{
	BRect tvrect = Bounds();
	tvrect.left += 22;
	tvrect.top += 2;
	tvrect.right -= 15;
	tvrect.bottom -= 2;
	BRect txtrect = tvrect;
	txtrect.left = 0;
		
	textview = new ThemisUrlTextView( tvrect, "URLTEXTVIEW", txtrect, resizingmode,
		B_WILL_DRAW | B_NAVIGABLE|B_FULL_UPDATE_ON_RESIZE );
	// set some textview flags
	textview->MakeResizable( true );
	textview->SetStylable( false );
	textview->SetWordWrap( false );
	AddChild( textview );
	
	//the fav_icon
	fav_icon = new BBitmap( BRect( 0,0,15,15 ), B_RGB32 );
}

void
ThemisUrlView::AttachedToWindow()
{
	BView::AttachedToWindow();
	
	SetFavIcon( NULL );
}

void
ThemisUrlView::Draw( BRect updaterect )
{
	//cout << "ThemisUrlView::Draw()" << endl;
	
	// compromise for now
	// make the updaterect the Bounds rectangle
	updaterect = Bounds();
	
	rgb_color hi = HighColor();	
	
	// the outer margin
	union int32torgb convert;
	AppSettings->FindInt32( "LightBorderColor", &convert.value );
	SetHighColor( convert.rgb );
	StrokeRect( updaterect, B_SOLID_HIGH );
	/*SetHighColor( 100,100,100 );
	updaterect.InsetBy( 1, 1 );
	StrokeRect( updaterect, B_SOLID_HIGH );
	updaterect.InsetBy( -1, -1 );*/
	
	// draw the dropdown arrow ( i didnt like to implement another
	// BPictureButton and Bitmap for this tiny thing )
	BPoint list[3];
	list[0].Set( updaterect.right - 11, updaterect.bottom - 12 );
	list[1].Set( updaterect.right - 5, updaterect.bottom - 12 );
	list[2].Set( updaterect.right - 8, updaterect.bottom - 7 );
	AppSettings->FindInt32( "ThemeColor", &convert.value );
	SetHighColor( convert.rgb );
	FillPolygon( list, 3, B_SOLID_HIGH );
	
	SetHighColor( hi );
	
	// draw the fav_icon		
	SetDrawingMode( B_OP_ALPHA );
	DrawBitmap( fav_icon, BRect( 3,3,18,18 ) );
	SetDrawingMode( B_OP_COPY );
}

void
ThemisUrlView::MouseDown( BPoint point )
{
	Win* win = ( Win* )Window();
	BRect rect = Bounds();
	if( point.x > rect.right - 20 )
		if( win->urlpopupwindow == NULL )
		{
			BMessage* open = new BMessage( URL_TYPED );
			open->AddBool( "show_all", true );
			win->PostMessage( open );
		}
		else
			win->PostMessage( CLOSE_URLPOPUP );
}

const char*
ThemisUrlView::Text()
{
	return textview->Text();
}

BTextView*
ThemisUrlView::TextView()
{
	return textview;
}

void
ThemisUrlView::SetFavIcon( BBitmap *fav )
{
	if( fav != NULL )
		memcpy( fav_icon->Bits(), fav->Bits(), 1024 );
	else
		// we could also have copied from icon_document_hex here
		memcpy( fav_icon->Bits(), 
			( ( Win* )Window() )->bitmaps[9]->Bits(), 1024 );
	if( Parent() )
		Draw( Bounds() );
}

void
ThemisUrlView::SetText( const char *newtext )
{
	textview->SetText( newtext );
	// move the cursor to the right
	textview->Select( strlen( newtext ), strlen( newtext ) );
}


/////////////////////////////////////
// ThemisUrlTextView
/////////////////////////////////////

ThemisUrlTextView::ThemisUrlTextView(
	BRect frame,
	const char* name,
	BRect textrect,
	uint32 resizingmode,
	uint32 flags )
	: BTextView(
		frame,
		name,
		textrect,
		resizingmode,
		flags )
{
}

void
ThemisUrlTextView::Paste( BClipboard* clipboard )
{
	cout << "ThemisUrlTextView::Paste()" << endl;
	
	const char *text; 
	BString ptext;
	int32 textlen; 
	BMessage *clip = ( BMessage * )NULL; 

	if( be_clipboard->Lock() )
	{
		if( ( clip = be_clipboard->Data() ) != NULL )
			clip->FindData( "text/plain", B_MIME_TYPE, ( const void ** )&text, &textlen );
		
		be_clipboard->Unlock(); 
	}
	ptext.SetTo( text );
	ptext.RemoveSet( "\a" );	// bell. will likely never be in, but you can never be sure :)
	ptext.RemoveSet( "\b" );	// backspace
	ptext.RemoveSet( "\f" );	// formfeed
	ptext.RemoveSet( "\n" );	// newline
	ptext.RemoveSet( "\r" );	// carriage return
	ptext.RemoveSet( "\t" );	// horizontal tab
	ptext.RemoveSet( "\v" );	// vertical tab
	ptext.RemoveSet( "\'" );	// apostroph sign
	ptext.RemoveSet( "\"" );	// quote
	cout << "ptext: " << ptext.String() << endl;
	SetText( ptext.String() );
}


/////////////////////////////////////
// ThemisUrlViewMessageFilter
/////////////////////////////////////

ThemisUrlViewMessageFilter::ThemisUrlViewMessageFilter( BWindow* win )
	: BMessageFilter( B_ANY_DELIVERY, B_ANY_SOURCE )
{
	window = win;
}

filter_result
ThemisUrlViewMessageFilter::Filter( BMessage *msg, BHandler **target )
{
	filter_result result( B_DISPATCH_MESSAGE );
	uint32 mod = modifiers();
	switch( msg->what )
	{
		case B_KEY_DOWN :
		{
			//cout << "B_KEY_DOWN received" << endl;			
									
			const char* key = msg->FindString( "bytes" );
			
			BMessenger messagetarget( (*target)->Looper() );
			
			switch( key[0] )
			{
				case B_DOWN_ARROW :
				{
					//cout << "B_DOWN_ARROW received" << endl;
					
					// if we have the urlpopupwindow
					if( ((Win*)window)->urlpopupwindow != NULL )
					{
						BMessenger msgr( ((Win*)window)->urlpopupwindow->Looper() );
						msgr.SendMessage( URL_SELECT_NEXT );
					}
					else // create it if ( urltext fits older urls )
					{
						BMessage* msg = new BMessage( URL_TYPED );
						msg->AddBool( "show_all", true );
						messagetarget.SendMessage( msg );
					}
					result = B_SKIP_MESSAGE;
					break;
				}
				case B_UP_ARROW :
				{
					//cout << "B_DOWN_ARROW received" << endl;
					
					// if we have the urlpopupwindow
					if( ((Win*)window)->urlpopupwindow != NULL )
					{
						BMessenger msgr( ((Win*)window)->urlpopupwindow->Looper() );
						msgr.SendMessage( URL_SELECT_PREV );
					}
					
					result = B_SKIP_MESSAGE;
					break;
				}
				case B_ESCAPE :
				{
					messagetarget.SendMessage( CLOSE_URLPOPUP );
					result = B_SKIP_MESSAGE;
					break;
				}
				// the next 2 cases need to be in here...
				// dont ask me why, but when i am really intensively using
				// _all_ arrow keys to navigate themis sometimes hangs ( no reall crash )
				// if i put those 2 in, i can't reprocude the 'hanging'
				// this is really weird
				case B_LEFT_ARROW :
					break;
				case B_RIGHT_ARROW :
					break;
				case B_ENTER :
				{
					messagetarget.SendMessage( URL_OPEN );
					result = B_SKIP_MESSAGE;
					break;
				}
				case B_TAB :
				{
					// if CONTROL+TAB are pressed, forward the message, to switch windows
					if( mod & B_CONTROL_KEY )
					{
						result = B_DISPATCH_MESSAGE;
						break;
					}
					
					// if we have the urlpopupwindow
					if( ((Win*)window)->urlpopupwindow != NULL )
					{
						BMessenger msgr( window->Looper() );
						msgr.SendMessage( CLOSE_URLPOPUP );
						
						result = B_SKIP_MESSAGE;
						break;
					}
					if( ((Win*)window)->tabview->TabAt( ((Win*)window)->tabview->Selection() )->View() != NULL )
					{
						if( window->CurrentFocus() )
							window->CurrentFocus()->MakeFocus( false );
						((Win*)window)->tabview->TabAt( ((Win*)window)->tabview->Selection() )->View()->MakeFocus( true );
					}	
								
					result = B_SKIP_MESSAGE;
					break;
				}
				case 'n' :
				{
					if( mod == 0x402 )	// B_OPTION_KEY
					{
						result = B_DISPATCH_MESSAGE;
						break;
					}

					messagetarget.SendMessage( URL_TYPED );
					result = B_DISPATCH_MESSAGE;
					break;
				}
				case 't' :
				{
					if( mod == 0x402 )	// B_OPTION_KEY
					{
						result = B_DISPATCH_MESSAGE;
						break;
					}

					// please don't ask me why, the below doesn't work!?
					// i don't know. and this drives me crazy!
					// same applies for case 'n' and case 'T'/'N'/'E'
//					if( mod & B_OPTION_KEY )
//					{
//						cout << "option + t" << endl;
//						result = B_DISPATCH_MESSAGE;
//						break;
//					}
					
					messagetarget.SendMessage( URL_TYPED );
					result = B_DISPATCH_MESSAGE;
					break;
				}
				// SHIFT+OPTION+x browser ident change detection
				case 'T' :
				{
					if( mod == 0x503 || mod == 0x603 )	// B_OPTION_KEY + SHIFT
					{
						result = B_DISPATCH_MESSAGE;
						break;
					}
										
					messagetarget.SendMessage( URL_TYPED );
					result = B_DISPATCH_MESSAGE;
					break;
				}
				case 'N' :
				{
					if( mod == 0x503 || mod == 0x603 )	// B_OPTION_KEY + SHIFT
					{
						result = B_DISPATCH_MESSAGE;
						break;
					}
										
					messagetarget.SendMessage( URL_TYPED );
					result = B_DISPATCH_MESSAGE;
					break;
				}
				case 'E' :
				{
					if( mod == 0x503 || mod == 0x603 )	// B_OPTION_KEY + SHIFT
					{
						result = B_DISPATCH_MESSAGE;
						break;
					}
										
					messagetarget.SendMessage( URL_TYPED );
					result = B_DISPATCH_MESSAGE;
					break;
				}
				default :
				{
					messagetarget.SendMessage( URL_TYPED );
					break;
				}
			}
		}
		default :
			break;
	}
	return result;
}
