/* prefswindow.cpp */

#define APPLY_BUTTON			'apbt'
#define COLOR_CHANGED			'clch'
#define DTD_SELECTED			'dtds'
#define HOMEPAGE_CHANGED		'hpch'
#define IM_CHECKBOX				'imcb'
#define LIST_SELECTION_CHANGED	'lsch'
#define NEW_WINDOW_START_PAGE	'nwsp'
#define OK_BUTTON				'okbt'
#define RESET_BUTTON			'resb'
#define SET_BLANK_PAGE			'stbp'
#define SHOWTABS_CHECKBOX		'stcb'
#define SHOWTYPEAHEAD_CHECKBOX  'stab'
#define TABSBACKGROUND_CHECKBOX	'tbcb'
#define TABHISTDEPTH_CHANGED	'thdc'

#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <ColorControl.h>
#include <Directory.h>
#include <ListItem.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <RadioButton.h>
#include <StringView.h>
#include <TextControl.h>

#include <iostream>
#include <ctype.h>
#include <stdlib.h>

#include "prefswin.h"
#include "app.h"
#include "win.h"

class ColorBox;

prefswin::prefswin(
	BRect frame,
	const char* title,
	window_look look,
	window_feel feel,
	uint32 flags,
	uint32 wspaces )
	: BWindow( frame, title, look, feel, flags, wspaces ) 
{
	AddShortcut( 'w', B_COMMAND_KEY, new BMessage( B_QUIT_REQUESTED ) );
	
	// init the temp vars with the ones from AppSettings
	
	// window vars
	AppSettings->FindString( "HomePage", &fHomePage );
	AppSettings->FindBool( "IntelligentMaximize", &fIntelligentMaximize );
	AppSettings->FindBool( "ShowTypeAhead", &fShowTypeAhead );
	AppSettings->FindInt8( "NewWindowStartPage", &fNewWindowStartPage );
	// tab prefs
	AppSettings->FindBool( "ShowTabsAtStartup", &fShowTabsAtStartup );
	AppSettings->FindBool( "OpenTabsInBackground", &fOpenTabsInBackground );
	AppSettings->FindInt8( "TabHistoryDepth", &fTabHistoryDepth );
	// color prefs
	union int32torgb convert;
	//AppSettings->FindInt32( "MenuColor", &convert.value );
	//fMenuColor = convert.rgb;
	AppSettings->FindInt32( "PanelColor", &convert.value );
	fPanelColor = convert.rgb;
	AppSettings->FindInt32( "ThemeColor", &convert.value );
	fThemeColor = convert.rgb;
	AppSettings->FindInt32( "ActiveTabColor", &convert.value );
	fActiveTabColor = convert.rgb;
	AppSettings->FindInt32( "InactiveTabColor", &convert.value );
	fInactiveTabColor = convert.rgb;
	AppSettings->FindInt32( "LightBorderColor", &convert.value );
	fLightBorderColor = convert.rgb;
	AppSettings->FindInt32( "DarkBorderColor", &convert.value );
	fDarkBorderColor = convert.rgb;
	AppSettings->FindInt32( "ShadowColor", &convert.value );
	fShadowColor = convert.rgb;
	// privacy prefs
	// HTML Parser prefs
	AppSettings->FindString( "DTDToUsePath", &fDTDToUsePath );
	
	
	BRect rect = Bounds();
	rect.right = 95;
	rect.bottom -= 35;
	lview = new leftview( rect, "LEFTVIEW", B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE_JUMP );
	AddChild( lview );
	
	rect.left = 96;
	rect.right = Bounds().right;
	rview = new BView( rect, "RIGHTVIEW", B_FOLLOW_ALL, B_WILL_DRAW );
	rview->SetViewColor( 219,219,219,255 );
	AddChild( rview );
	
	rect = Bounds();
	rect.top = rect.bottom - 35;
	bview = new BView( rect, "BOTTOMVIEW", B_FOLLOW_ALL, B_WILL_DRAW );
	bview->SetViewColor( 219,219,219,255 );
	AddChild( bview );
	
	// the buttons
	rect.left = bview->Bounds().right - 165;
	rect.top = bview->Bounds().bottom - 30;
	rect.right = rect.left + 45;
	rect.bottom = bview->Bounds().bottom - 10;
	BButton* OKbtn = new BButton( rect, "OKBUTTON", "OK", new BMessage( OK_BUTTON ), B_FOLLOW_ALL, B_WILL_DRAW );
	bview->AddChild( OKbtn );
	
	rect.left = rect.right + 10;
	rect.right = rect.left + 45;
	BButton* Cancelbtn = new BButton( rect, "CANCELBUTTON", "Cancel", new BMessage( B_QUIT_REQUESTED ), B_FOLLOW_ALL, B_WILL_DRAW );
	bview->AddChild( Cancelbtn );
	
	rect.left = rect.right + 10;
	rect.right = rect.left + 45;
	BButton* Applybtn = new BButton( rect, "APPLYBUTTON", "Apply", new BMessage( APPLY_BUTTON ), B_FOLLOW_ALL, B_WILL_DRAW );
	bview->AddChild( Applybtn );
		
	Show();
}

prefswin::~prefswin() 
{
}

bool
prefswin::QuitRequested() 
{
	cout << "prefswin::QuitRequested()" << endl;
	
	BPoint point( Frame().left, Frame().top );
	AppSettings->ReplacePoint( "PrefsWindowPoint", point );
	
	be_app_messenger.SendMessage( PREFSWIN_CLOSE );
	cout << "end of prefswin::QuitRequested()" << endl;
	return true;
}

void prefswin::MessageReceived( BMessage* msg )
{
	switch( msg->what )
	{
		case APPLY_BUTTON :
		{
			cout << "APPLY_BUTTON" << endl;
			
			// write settings to AppSettings
			SaveSettings();
			// as Themis crashes very often on quit, i call the real SaveSettings() here too
			be_app_messenger.SendMessage( SAVE_APP_SETTINGS );
			
			// tell all windows to redraw its contents
			Win* win = ( ( App* )be_app )->FirstWindow();
			
			if( win == NULL )
			{
				cout << "First Window not valid anymore!" << endl;
				return;
			}	
			
			BMessenger* msgr = new BMessenger( NULL, win, NULL );
			msgr->SendMessage( RE_INIT_INTERFACE );
			
			while( win->NextWindow() != NULL )
			{
				win = win->NextWindow();
				delete msgr;
				msgr = new BMessenger( NULL, win, NULL );
				msgr->SendMessage( RE_INIT_INTERFACE );
			}
			
			delete msgr;
			break;
		}
		case COLOR_CHANGED :
		{
			cout << "COLOR_CHANGED" << endl;
			
			int8 colorindex;
			msg->FindInt8( "colorindex", &colorindex );
						
			BColorControl* ctrl;
			msg->FindPointer( "source", ( void** )&ctrl );
			
			switch( colorindex )
			{
				//case 0 : { fMenuColor = ctrl->ValueAsColor(); fMenuColorBox->SetColor( fMenuColor ); break; }
				case 1 : { fPanelColor = ctrl->ValueAsColor(); fPanelColorBox->SetColor( fPanelColor ); break; }
				case 2 : { fThemeColor = ctrl->ValueAsColor(); fThemeColorBox->SetColor( fThemeColor ); break; }
				case 3 : { fActiveTabColor = ctrl->ValueAsColor(); fActiveTabColorBox->SetColor( fActiveTabColor ); break; }
				case 4 : { fInactiveTabColor = ctrl->ValueAsColor(); fInactiveTabColorBox->SetColor( fInactiveTabColor ); break; }
				case 5 : { fLightBorderColor = ctrl->ValueAsColor(); fLightBorderColorBox->SetColor( fLightBorderColor ); break; }
				case 6 : { fDarkBorderColor = ctrl->ValueAsColor(); fDarkBorderColorBox->SetColor( fDarkBorderColor ); break; }
				case 7 : { fShadowColor = ctrl->ValueAsColor(); fShadowColorBox->SetColor( fShadowColor ); break; }
			}
				
			rview->Draw( rview->Bounds() );
								
			break;
		}
		case DTD_SELECTED :
		{
			cout << "DTD_SELECTED" << endl;
			BString string;
			msg->FindString( "DTDFileString", &string );
			cout << "DTDFileString: " << string.String() << endl;
			fDTDToUsePath.SetTo( string.String() );
			// save the string ( don't remove! ) the HTMLParser needs the setting at once
			AppSettings->ReplaceString( "DTDToUsePath", fDTDToUsePath );
			
			// send a message to the app, which then sends a Broadcast to the HTMLParser
			be_app_messenger.SendMessage( DTD_CHANGED );
			break;
		}
		case HOMEPAGE_CHANGED :
		{
			BTextControl* ctrl;
			msg->FindPointer( "source", ( void** )&ctrl );
			fHomePage.SetTo( ctrl->Text() );
			
			break;
		}
		case IM_CHECKBOX :
		{
			BCheckBox* cbox;
			msg->FindPointer( "source", ( void** )&cbox );
			if( cbox->Value() == 1 )
				fIntelligentMaximize = true;
			else
				fIntelligentMaximize = false;			
			
			break;
		}
		case LIST_SELECTION_CHANGED :
		{	
			// remove all possible childs of the right view
			if( rview->CountChildren() > 0 )
			{
				BView* child = NULL;
				int32 count = rview->CountChildren();
				while( rview->CountChildren() > 0 )
				{
					child = rview->ChildAt( 0 );
					rview->RemoveChild( child );
					delete child;
				}
			}			
			
			BRect rect;
			rect.left = rview->Bounds().left + 5;
			rect.top = 5;
			rect.right = rview->Bounds().right - 10;
			rect.bottom = rview->Bounds().bottom;
			
			switch( lview->listview->CurrentSelection() )
			{
				case 0 :
				{
					cout << "Window item selected" << endl;
										
					BBox* box = new BBox( rect, "BOX" );
					box->SetLabel( "Window" );
					rview->AddChild( box );
	
					rect.left += 5;
					rect.top += 10;
					rect.right = box->Bounds().right - 10;
					rect.bottom = rect.top + 15;
					
					BTextControl* homepage = new BTextControl( rect, "HOMEPAGE", "Home Page:", "", NULL );
					homepage->SetModificationMessage( new BMessage( HOMEPAGE_CHANGED ) );
					homepage->SetDivider( be_plain_font->StringWidth( "Home Page:" ) + 5.0 );
					homepage->SetText( fHomePage.String() );
					box->AddChild( homepage );
					
					rect.top = rect.bottom + 10;
					rect.bottom = rect.top + 20;
					rect.left = rect.right - 100;
					BMessage* blankmsg = new BMessage( SET_BLANK_PAGE );
					blankmsg->AddPointer( "homepagecontrol", homepage );
					BButton* blankbtn = new BButton( rect, "BLANKBUTTON", "Use Blank Page", blankmsg, B_FOLLOW_ALL, B_WILL_DRAW );
					box->AddChild( blankbtn );
					
					rect.left = Bounds().left + 10;
					rect.top = rect.bottom + 10;
					rect.bottom = rect.top + 15;
					rect.right = 200;
					BCheckBox* IMcbox = new BCheckBox( rect, "IMCBOX", "Intelligent Maximize", new BMessage( IM_CHECKBOX ) );
					if( fIntelligentMaximize == false )
						IMcbox->SetValue( 0 );
					else
						IMcbox->SetValue( 1 );
					box->AddChild( IMcbox );
					
					rect.top += 20;
					rect.bottom = rect.top + 15;
					BCheckBox* ShowTypeAheadbox = new BCheckBox(
						rect,
						"SHOWTYPEAHEADBOX",
						"Show Type Ahead Window",
						new BMessage( SHOWTYPEAHEAD_CHECKBOX ) );
					if( fShowTypeAhead == false )
						ShowTypeAheadbox->SetValue( 0 );
					else
						ShowTypeAheadbox->SetValue( 1 );
					box->AddChild( ShowTypeAheadbox );
					
					rect.left = box->Frame().left + 5;
					rect.top = rect.bottom + 20;
					rect.right = box->Frame().right - 15;
					rect.bottom = rect.top + 75;
	
					BBox* newwinpagebox = new BBox( rect, "NEWWINPAGEBOX" );
					newwinpagebox->SetLabel( "New Windows open with ..." );
					box->AddChild( newwinpagebox );
							
					rect.left = 5;
					rect.top = 10;
					rect.bottom = rect.top + 20;
					rect.right = 150;
					
					// blank page
					BMessage* nwspmsg1 = new BMessage( NEW_WINDOW_START_PAGE );
					nwspmsg1->AddInt8( "newpagemode", 0 );		
					BRadioButton* rbtn1 = new BRadioButton( rect, "RBTN1", "Blank Page", nwspmsg1 );
					newwinpagebox->AddChild( rbtn1 );
					// homepage
					rect.top += 20;
					rect.bottom = rect.top + 20;
					BMessage* nwspmsg2 = new BMessage( NEW_WINDOW_START_PAGE );
					nwspmsg2->AddInt8( "newpagemode", 1 );		
					BRadioButton* rbtn2 = new BRadioButton( rect, "RBTN2", "Home Page", nwspmsg2 );
					newwinpagebox->AddChild( rbtn2 );
					// current page
					rect.top += 20;
					rect.bottom = rect.top + 20;
					BMessage* nwspmsg3 = new BMessage( NEW_WINDOW_START_PAGE );
					nwspmsg3->AddInt8( "newpagemode", 2 );		
					BRadioButton* rbtn3 = new BRadioButton( rect, "RBTN3", "Current Page", nwspmsg3 );
					newwinpagebox->AddChild( rbtn3 );
					
					// select the correct radiobutton
					switch( fNewWindowStartPage )
					{
						case 0 : { rbtn1->SetValue( B_CONTROL_ON ); break; }
						case 1 : { rbtn2->SetValue( B_CONTROL_ON ); break; }
						case 2 : { rbtn3->SetValue( B_CONTROL_ON ); break; }
					}
														
					break;
				}
				case 1 :
				{
					cout << "Tabs item selected" << endl;
					
					BBox* box = new BBox( rect, "BOX" );
					box->SetLabel( "Tabs" );
					rview->AddChild( box );
					
					rect.left += 5;
					rect.top += 10;
					rect.bottom = rect.top + 15;
					rect.right = 200;
					
					BCheckBox* ShowTabscbox = new BCheckBox(
						rect,
						"SHOWTABSCBOX",
						"Show Tabs at Startup",
						new BMessage( SHOWTABS_CHECKBOX ) );
					if( fShowTabsAtStartup == false )
						ShowTabscbox->SetValue( 0 );
					else
						ShowTabscbox->SetValue( 1 );
					box->AddChild( ShowTabscbox );
					
					rect.top += 20;
					rect.bottom = rect.top + 15;
					BCheckBox* OpenTabsInBackgroundcbox = new BCheckBox(
						rect,
						"OPENTABSINBACKGROUNDCBOX",
						"Open new Tabs in Background",
						new BMessage( TABSBACKGROUND_CHECKBOX ) );
					if( fOpenTabsInBackground == false )
						OpenTabsInBackgroundcbox->SetValue( 0 );
					else
						OpenTabsInBackgroundcbox->SetValue( 1 );
					box->AddChild( OpenTabsInBackgroundcbox );
					
					rect.top += 25;
					rect.bottom = rect.top + 15;
					
					BTextControl* tabhistdepth = new BTextControl( rect, "TABHISTDEPTH", "Tab History Depth [1-127]:", "", NULL );
					tabhistdepth->SetModificationMessage( new BMessage( TABHISTDEPTH_CHANGED ) );
					tabhistdepth->SetDivider( be_plain_font->StringWidth( "Tab History Depth [1-127]:" ) + 5.0 );
					
					char* string;
					sprintf( string, "%d", fTabHistoryDepth );
					tabhistdepth->SetText( string );
					tabhistdepth->TextView()->AddFilter( new DigitOnlyMessageFilter() );
					box->AddChild( tabhistdepth );
					
					
					break;
				}
				case 2 :
				{
					cout << "Fonts item selected" << endl;
					
					BBox* box = new BBox( rect, "BOX" );
					box->SetLabel( "Fonts" );
					rview->AddChild( box );
					
					rect.left += 10;
					rect.top += 10;
					rect.bottom = rect.top + 20;
					rect.right -= 10;
										
					BStringView* strview = new BStringView( rect, "STRVIEW", "No sex until marriage! -- No fonts until renderer! :D" );
					box->AddChild( strview );
					
					break;
				}
				case 3 :
				{
					cout << "Colors item selected" << endl;
					
					BBox* box = new BBox( rect, "BOX" );
					box->SetLabel( "Colors" );
					rview->AddChild( box );
					
					rect.left += 5;
					rect.top += 10;
					rect.bottom = rect.top + 20;
					rect.right = rect.left + 200;
//					fMenuColorBox = new ColorBox( rect, "Menu Color", 0, fMenuColor );
//					box->AddChild( fMenuColorBox );
//					
//					rect.top = rect.bottom + 10;
//					rect.bottom = rect.top + 20;
					fPanelColorBox = new ColorBox( rect, "Panel Color", 1, fPanelColor );
					box->AddChild( fPanelColorBox );
					
					rect.top = rect.bottom + 10;
					rect.bottom = rect.top + 20;
					fThemeColorBox = new ColorBox( rect, "Theme Color", 2, fThemeColor );
					box->AddChild( fThemeColorBox );
					
					rect.top = rect.bottom + 10;
					rect.bottom = rect.top + 20;
					fActiveTabColorBox = new ColorBox( rect, "Active Tab Color", 3, fActiveTabColor );
					box->AddChild( fActiveTabColorBox );
					
					rect.top = rect.bottom + 10;
					rect.bottom = rect.top + 20;
					fInactiveTabColorBox = new ColorBox( rect, "Inactive Tab Color", 4, fInactiveTabColor );
					box->AddChild( fInactiveTabColorBox );
					
					rect.top = rect.bottom + 10;
					rect.bottom = rect.top + 20;
					fLightBorderColorBox = new ColorBox( rect, "Light Border Color", 5, fLightBorderColor );
					box->AddChild( fLightBorderColorBox );
					
					rect.top = rect.bottom + 10;
					rect.bottom = rect.top + 20;
					fDarkBorderColorBox = new ColorBox( rect, "Dark Border Color", 6, fDarkBorderColor );
					box->AddChild( fDarkBorderColorBox );
					
					rect.top = rect.bottom + 10;
					rect.bottom = rect.top + 20;
					fShadowColorBox = new ColorBox( rect, "Shadow Color", 7, fShadowColor );
					box->AddChild( fShadowColorBox );
					
					rect = box->Bounds();
					rect.right -= 10;
					rect.top = rect.bottom - 30;
					rect.bottom -= 10;
					rect.left = rect.right - 50;
					
					BButton* Resetbtn = new BButton( rect, "RESETBUTTON", "Reset", new BMessage( RESET_BUTTON ), B_FOLLOW_ALL, B_WILL_DRAW );
					box->AddChild( Resetbtn );
					
					
					break;
				}
				case 4 :
				{
					cout << "Privacy item selected" << endl;
					
					BBox* box = new BBox( rect, "BOX" );
					box->SetLabel( "Privacy" );
					rview->AddChild( box );
					
					rect.left += 10;
					rect.top += 10;
					rect.bottom = rect.top + 20;
					rect.right -= 10;
										
					BStringView* strview = new BStringView( rect, "STRVIEW", "Nothing here right now." );
					box->AddChild( strview );
										
					break;
				}
				case 5 :
				{
					cout << "HTML Parser item selected" << endl;
					
					BBox* box = new BBox( rect, "BOX" );
					box->SetLabel( "HTML Parser" );
					rview->AddChild( box );
					
					rect.left += 10;
					rect.top += 10;
					rect.bottom = rect.top + 20;
					rect.right -= 50;
					
					BPopUpMenu* popmenu = new BPopUpMenu( "No DTD selected or available!", true, true, B_ITEMS_IN_COLUMN );
					
					// set the DTDToUsePath to "none", as we may not find a DTD below
					//AppSettings->AddString( "DTDToUsePath", "none" );
	
					// find a DTD
					BString dtddir;
					AppSettings->FindString( "settings_directory", &dtddir );
					dtddir.Append( "/dtd/" );
					printf( "DTD dir: %s\n", dtddir.String() );
			
					BDirectory* dir = new BDirectory( dtddir.String() );
					if( dir->InitCheck() != B_OK )
					{
						printf( "DTD directory (%s) not found!\n", dtddir.String() );
						printf( "Setting DTDToUsePath to \"none\"\n" );
						AppSettings->AddString( "DTDToUsePath", "none" );
					}
					else
					{
						BEntry entry;
						while( dir->GetNextEntry( &entry, false ) != B_ENTRY_NOT_FOUND )
						{
							BPath path;
							entry.GetPath( &path );
							char name[B_FILE_NAME_LENGTH];
							entry.GetName( name );
					
							BString nstring( name );
							printf( "----------------\n" );
							printf( "found file: %s\n", nstring.String() );
							if( nstring.IFindFirst( "DTD", nstring.Length() - 3 ) != B_ERROR )
							{
								printf( "found DTD file: %s\n", nstring.String() );
								
								// add the file to the popupmenu
								BMessage* msg = new BMessage( DTD_SELECTED );
								msg->AddString( "DTDFileString", path.Path() );
								BMenuItem* item = new BMenuItem( name, msg, 0, 0 );
								popmenu->AddItem( item );
								
								// if the path of the current file equals the one of the settings,
								// mark the item
								if( strncmp( path.Path(), fDTDToUsePath.String(), strlen( path.Path() ) ) == 0 )
								{
									printf( "DTD from settings found -> SetMarked( true )\n" );
									( popmenu->ItemAt( popmenu->CountItems() - 1 ) )->SetMarked( true );
								}
							}
						} // while
						
						// if we found some DTDs, but still no DTD is saved in the prefs,
						// or no DTD is selected: 
						// set the last found DTD in the prefs. we save it to the prefs,
						// because the user might not reselect a DTD in the list, which
						// would save the DTD.
						BMessage* imsg = new BMessage( DTD_SELECTED );
						if( popmenu->CountItems() > 0 )
						{
							if( popmenu->FindMarked() == NULL )
							{
								printf( "no marked item found\n" );
								BMenuItem* item = popmenu->ItemAt( popmenu->CountItems() - 1 );
								item->SetMarked( true );
								// as we cannot invoke the item here, send the DTD_SELECTED message here
								BString dtdstring( dtddir.String() );
								dtdstring.Append( item->Label() );
								imsg->AddString( "DTDFileString", dtdstring.String() );
								PostMessage( imsg );
							}
						}
						else
						{
							imsg->AddString( "DTDFileString", "none" );
							PostMessage( imsg );
						}
						delete imsg;
					}
					delete dir;
					// end: find a DTD
					
					BMenuField* dtdmenufield = new BMenuField( rect, "DTDFIELD", "Document Type Definition:", popmenu, true, B_FOLLOW_TOP, B_WILL_DRAW);
					dtdmenufield->SetDivider( be_plain_font->StringWidth( "Document Type Definition:" ) + 5.0 );
					box->AddChild( dtdmenufield );					
					
					break;
				}
				default :
					break;
			}
			break;
		}
		case NEW_WINDOW_START_PAGE :
		{
			cout << "NEW_WINDOW_START_PAGE" << endl;
			int8 value;
			msg->FindInt8( "newpagemode", &value );
			fNewWindowStartPage = value;
			
			break;
		}
		case OK_BUTTON :
		{
			// apply all changes which have been made
			BMessage* applymsg = new BMessage( APPLY_BUTTON );
			MessageReceived( applymsg );			
			delete applymsg;
			
			BMessenger msgr( this );
			BMessage* qmsg = new BMessage( B_QUIT_REQUESTED );
			if( msgr.IsValid() == true )
			{	
				if( msgr.SendMessage( qmsg ) != B_OK )
					cout << "Error sending B_QUIT_REQUESTED message" << endl;
			}
			else
				cout << "Messenger not valid! B_QUIT_REQUESTED not sent." << endl;
			delete qmsg;

			break;
		}
		case RESET_BUTTON :
		{
			// reset all colors
			
			//fMenuColor = ui_color( B_MENU_BACKGROUND_COLOR );
			fPanelColor = ui_color( B_PANEL_BACKGROUND_COLOR );
			fThemeColor.red = 255;
			fThemeColor.green = 200;
			fThemeColor.blue = 0;
			fActiveTabColor = ui_color( B_PANEL_BACKGROUND_COLOR );
			fInactiveTabColor = fActiveTabColor;
			fInactiveTabColor.red -= 32;
			fInactiveTabColor.green -= 32;
			fInactiveTabColor.blue -= 32;
			fLightBorderColor.red = 187;
			fLightBorderColor.green = 187;
			fLightBorderColor.blue = 187;
			fDarkBorderColor.red = 51;
			fDarkBorderColor.green = 51;
			fDarkBorderColor.blue = 51;
			fShadowColor.red = 240;
			fShadowColor.green = 240;
			fShadowColor.blue = 240;
						
			//fMenuColorBox->SetColor( fMenuColor );
			fPanelColorBox->SetColor( fPanelColor );
			fThemeColorBox->SetColor( fThemeColor );
			fActiveTabColorBox->SetColor( fActiveTabColor );
			fInactiveTabColorBox->SetColor( fInactiveTabColor );
			fLightBorderColorBox->SetColor( fLightBorderColor );
			fDarkBorderColorBox->SetColor( fDarkBorderColor );
			fShadowColorBox->SetColor( fShadowColor );
			break;
		}
		case SET_BLANK_PAGE :
		{
			BTextControl* ctrl;
			msg->FindPointer( "homepagecontrol", ( void** )&ctrl );
			ctrl->SetText( "about:blank" );
			
			break;
		}
		case SHOWTABS_CHECKBOX :
		{
			BCheckBox* cbox;
			msg->FindPointer( "source", ( void** )&cbox );
			if( cbox->Value() == 1 )
				fShowTabsAtStartup = true;
			else
				fShowTabsAtStartup = false;		
			
			break;
		}
		case SHOWTYPEAHEAD_CHECKBOX :
		{
			BCheckBox* cbox;
			msg->FindPointer( "source", ( void** )&cbox );
			if( cbox->Value() == 1 )
				fShowTypeAhead = true;
			else
				fShowTypeAhead = false;
			
			break;		
		}
		case TABSBACKGROUND_CHECKBOX :
		{
			BCheckBox* cbox;
			msg->FindPointer( "source", ( void** )&cbox );
			if( cbox->Value() == 1 )
				fOpenTabsInBackground = true;
			else
				fOpenTabsInBackground = false;		
			
			break;
		}
		case TABHISTDEPTH_CHANGED :
		{
//			printf( "TABHISTDEPTH_CHANGED\n" );
			BTextControl* ctrl;
			msg->FindPointer( "source", ( void** )&ctrl );
			BString string( ctrl->Text() );
//			printf( "  %s [length: %ld]\n", string.String(), string.Length() );
			
			if( string.Length() == 0 )
				break;
			
			int32 newdepth = atoi( string.String() );
			if( newdepth > 127 || newdepth == 0 )
			{
//				printf( "  Correction.\n" );
				fTabHistoryDepth = 10;
				ctrl->SetText( "10" );
				ctrl->TextView()->Select( 2, 2 );
			}
			else
				fTabHistoryDepth = ( int8 )newdepth;
			
//			printf( "  new fTabHistoryDepth: %d\n", fTabHistoryDepth );
					
			break;
		}
		default:
			BWindow::MessageReceived( msg );
	}
}

void
prefswin::SaveSettings()
{
	cout << "prefswin::SaveSettings()" << endl;
	
	// window
	AppSettings->ReplaceString( "HomePage", fHomePage.String() );
	AppSettings->ReplaceBool( "IntelligentMaximize", fIntelligentMaximize );
	AppSettings->ReplaceBool( "ShowTypeAhead", fShowTypeAhead );
	AppSettings->ReplaceInt8( "NewWindowStartPage", fNewWindowStartPage );
	// tabs
	AppSettings->ReplaceBool( "ShowTabsAtStartup", fShowTabsAtStartup );
	AppSettings->ReplaceBool( "OpenTabsInBackground", fOpenTabsInBackground );
	AppSettings->ReplaceInt8( "TabHistoryDepth", fTabHistoryDepth );
	// fonts
	// colors
	union int32torgb convert;
	//convert.rgb = fMenuColor;
	//AppSettings->ReplaceInt32( "MenuColor", convert.value );
	convert.rgb = fPanelColor;
	AppSettings->ReplaceInt32( "PanelColor", convert.value );
	convert.rgb = fThemeColor;
	AppSettings->ReplaceInt32( "ThemeColor", convert.value );
	convert.rgb = fActiveTabColor;
	AppSettings->ReplaceInt32( "ActiveTabColor", convert.value );
	convert.rgb = fInactiveTabColor;
	AppSettings->ReplaceInt32( "InactiveTabColor", convert.value );
	convert.rgb = fLightBorderColor;
	AppSettings->ReplaceInt32( "LightBorderColor", convert.value );
	convert.rgb = fDarkBorderColor;
	AppSettings->ReplaceInt32( "DarkBorderColor", convert.value );
	convert.rgb = fShadowColor;
	AppSettings->ReplaceInt32( "ShadowColor", convert.value );

	// privacy
	// HTML Parser
	AppSettings->ReplaceString( "DTDToUsePath", fDTDToUsePath );
}

/////////////////////////////////////
// leftview
/////////////////////////////////////

leftview::leftview(
	BRect frame,
	const char* name,
	uint32 resizingmode,
	uint32 flags )
	: BView( frame, name, resizingmode, flags )
{
	listview = NULL;
}

void
leftview::AttachedToWindow()
{
	SetViewColor( 219,219,219,255 );
	
	// add the list box
	BRect rect;
	rect.left = 10;
	rect.top = 10;
	rect.right = 90;
	rect.bottom = Bounds().bottom;
		
	BBox* listbox = new BBox( rect, "LISTBOX" );
	AddChild( listbox );
	
	// add the listview
	rect = listbox->Bounds();
	rect.InsetBy( 2, 2 );
	rect.right -=1;
	listview = new BListView( rect, "LISTVIEW", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE );
	listbox->AddChild( listview );
	
	// add the list items
	listview->AddItem( new BStringItem( "Window" ) );
	listview->AddItem( new BStringItem( "Tabs" ) );
	listview->AddItem( new BStringItem( "Fonts" ) );
	listview->AddItem( new BStringItem( "Colors" ) );
	listview->AddItem( new BStringItem( "Privacy" ) );
	listview->AddItem( new BStringItem( "HTML Parser" ) );
	/*listview->AddItem( new BStringItem( "Cache" ) );
	listview->AddItem( new BStringItem( "Cookies" ) );*/
	
	listview->SetSelectionMessage( new BMessage( LIST_SELECTION_CHANGED ) );
	listview->MakeFocus( true );
	listview->Select( 0 );
		
	BView::AttachedToWindow();
}

/////////////////////////////////////
// ColorBox
/////////////////////////////////////

ColorBox::ColorBox(
	BRect frame,
	const char* name,
	int colorindex,
	rgb_color startcolor )
	: BView( frame, name, B_FOLLOW_NONE, B_WILL_DRAW )
{
	fColorIndex = colorindex;
	
	fColor.red = startcolor.red;
	fColor.green = startcolor.green;
	fColor.blue = startcolor.blue;
}

ColorBox::~ColorBox()
{
}

void
ColorBox::Draw( BRect updaterect )
{
	updaterect = Bounds();
	
	rgb_color hi = HighColor();
	
	SetHighColor( Parent()->ViewColor() );
	FillRect( updaterect, B_SOLID_HIGH );	
	
	// draw the color rectangle
	updaterect.right = 30;
	
	SetHighColor( 96,96,96,255 );
	StrokeRect( updaterect, B_SOLID_HIGH );
	updaterect.InsetBy( 1, 1 );
	
	SetHighColor( 255,255,255,255 );
	StrokeRect( updaterect, B_SOLID_HIGH );
	
	SetHighColor( 232,232,232,255 );
	StrokeLine( BPoint( updaterect.left, updaterect.top ),
		BPoint( updaterect.right, updaterect.top ), B_SOLID_HIGH );
	StrokeLine( BPoint( updaterect.left, updaterect.top ),
		BPoint( updaterect.left, updaterect.bottom ), B_SOLID_HIGH );
	
	SetHighColor( 152,152,152,255 );
	StrokeLine( BPoint( updaterect.left, updaterect.bottom ),
		BPoint( updaterect.right, updaterect.bottom ), B_SOLID_HIGH );
	StrokeLine( BPoint( updaterect.right, updaterect.top ),
		BPoint( updaterect.right, updaterect.bottom ), B_SOLID_HIGH );
	
	SetHighColor( fColor );
	updaterect.InsetBy( 1, 1 );
	FillRect( updaterect, B_SOLID_HIGH );
	
	// draw the color string
	SetHighColor( 0,0,0,255 );
	SetFont( be_plain_font );
	font_height height_struct;
	be_plain_font->GetHeight( &height_struct );
	DrawString(
		Name(),
		BPoint(
			updaterect.right + 10,
			updaterect.top + ( ( updaterect.Height() / 2 ) + ( ( height_struct.ascent /*+ height_struct.descent*/ ) / 2 ) ) )
			);
	
	SetHighColor( hi );
}

void
ColorBox::MouseDown( BPoint point )
{
	uint32 buttons;
	GetMouse( &point, &buttons, true );
	
	// make sure, we only invoke the color-selector-window,
	// when we click on the color rectangle, not on the text
	// ok, this is a bit too static, but ok for now
	if( point.x > 30 )
		return;
	
	switch( buttons )
	{
		case B_PRIMARY_MOUSE_BUTTON :
		{
			ColorSelectWindow* colselwin = new ColorSelectWindow(
				BRect( 400, 250, 750, 335 ),
				Name(),
				B_TITLED_WINDOW_LOOK,
				B_MODAL_APP_WINDOW_FEEL,
				B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE,
				Window(),
				fColorIndex,
				fColor );
			colselwin->Show();
															
			break;
		}
		default :
			BView::MouseDown( point );
	}
}

void
ColorBox::SetColor( rgb_color color )
{
	fColor.red = color.red;
	fColor.green = color.green;
	fColor.blue = color.blue;
	
	Draw( Bounds() );
}

/////////////////////////////////////
// ColorSelectWindow
/////////////////////////////////////

ColorSelectWindow::ColorSelectWindow(
	BRect frame,
	const char* title,
	window_look look,
	window_feel feel,
	uint32 flags,
	BWindow* pwin,
	int colorindex,
	rgb_color startcolor,
	uint32 wspaces )
	: BWindow( frame, title, look, feel, flags, wspaces ) 
{
	fParentWin = pwin;
	
	AddShortcut( 'w', B_COMMAND_KEY, new BMessage( B_QUIT_REQUESTED ) );
	
	BView* view = new BView( Bounds(), "VIEW", B_FOLLOW_ALL, B_WILL_DRAW );
	union int32torgb panelcolor;
	AppSettings->FindInt32( "PanelColor", &panelcolor.value );
	view->SetViewColor( panelcolor.rgb );
	AddChild( view );
	
	BMessage* msg = new BMessage( COLOR_CHANGED );
	msg->AddInt8( "colorindex", colorindex );
		
	fColctrl = new BColorControl( BPoint( Bounds().left + 10, Bounds().top + 10 ), B_CELLS_32x8, 8.0,
		"COLCTRL", msg, true );
	fColctrl->SetTarget( NULL, fParentWin );
	fColctrl->SetValue( startcolor );
	view->AddChild( fColctrl );
}

ColorSelectWindow::~ColorSelectWindow()
{
}
