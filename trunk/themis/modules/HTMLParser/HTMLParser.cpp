/* HTMLParser implementation
	See HTMLParser.h for some more information
*/

#include <fstream>
#include <string>
#include <algorithm>
#include <ctype.h>
#include <stdio.h>

#include <Message.h>
#include <DataIO.h>

#include "HTMLParser.h"
#include "commondefs.h"
#include "plugman.h"


#include "TDocument.h"
#include "TElement.h"
#include "TNode.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"
#include "TText.h"
#include "TComment.h"


HTMLParser * parser;
BMessage ** appSettings_p;
BMessage * appSettings;

status_t Initialize( void * info )	{
	
	printf( "Calling Initialize...\n" );
	
	parser = NULL;
	if ( info != NULL )	{
		BMessage * message = (BMessage *) info;
		if ( message->HasPointer( "settings_message_ptr" ) )	{
			message->FindPointer( "settings_message_ptr", (void **) & appSettings_p );
			appSettings = *appSettings_p;
		}
		parser = new HTMLParser( message );
	}
	else	{
		parser = new HTMLParser();
	}
	
	return B_OK;
	
}

status_t Shutdown( bool now )	{
	
	delete parser;
	
	return B_OK;
	
}

PlugClass * GetObject()	{
	
	return parser;
	
}

HTMLParser	::	HTMLParser( BMessage * info )	:	BHandler( "HTMLParser" ), PlugClass( info )	{
	
	cache = (CachePlug *) PlugMan->FindPlugin( CachePlugin );
	userToken = 0;
	
	if ( cache != NULL )	{
		userToken = cache->Register( Type(), "HTML Parser" );
	}
	
	reset();
	
}

HTMLParser	::	~HTMLParser()	{
	
}

void HTMLParser	::	reset()	{

	mPos = 0;
	mOldPos = 0;
	mLineNr = 1;
	mCharNr = 1;
	mOldTagLineNr = mLineNr;
	mOldTagCharNr = mCharNr;
	mTagLineNr = mLineNr;
	mTagCharNr = mCharNr;
	mTag = "";
	mOldTag = "";
	mContent = "";
	mCloseTag = false;
	
}

void HTMLParser	::	showTree( const TNodeShared aNode, int aSpacing )	{
	
	TNodeListShared children = aNode->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = 0; i < length; i++ )	{
			TNodeShared child = make_shared( children->item( i ) );
			for ( int j = 0; j < aSpacing; j++ )	{
				cout << "  ";
			}
			cout << "Child name: " << child->getNodeName().c_str() << endl;
			if ( child->getNodeType() == ELEMENT_NODE )	{
				// Check for attributes
				TNamedNodeMapShared attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodeWeak attr = attributes->item( j );
					TNodeShared tempAttr = make_shared( attr );
					for ( int j = 0; j < aSpacing + 1; j++ )	{
						cout << "  ";
					}
					cout << "Attribute " << tempAttr->getNodeName();
					cout << " with value " << tempAttr->getNodeValue() << endl;
				}
			}
			showTree( child, aSpacing + 1 );
		}
	}	
}

void HTMLParser	::	showDocument()	{
	
	cout << "Document layout...\n";
	showTree( mDocument, 1 );

}

void HTMLParser	::	setContent( string aContent )	{
	
	mContent = aContent;
	
}

bool HTMLParser	::	isStartTag()	{
	
	return !mCloseTag;

}

bool HTMLParser	::	isDocTypeTag()	{
	
	if ( mTag == "!doctype" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isHtmlTag()	{
	
	if ( mTag == "html" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isCommentTag()	{
	
	if ( mTag == "!--" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isHeadTag()	{
	
	if ( mTag == "head" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isBodyTag()	{
	
	if ( mTag == "body" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isTitleTag()	{
	
	if ( mTag == "title" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isStyleTag()	{
	
	if ( mTag == "style" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isScriptTag()	{
	
	if ( mTag == "script" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isIsIndexTag()	{
	
	if ( mTag == "isindex" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isBaseTag()	{
	
	if ( mTag == "base" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isMetaTag()	{
	
	if ( mTag == "meta" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isLinkTag()	{
	
	if ( mTag == "link" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isBlockLevelTag()	{

	if ( isPTag() ||
		 isListTag() ||
		 isPreTag() ||
		 isBodyStyleTag() ||
		 isFormTag() ||
		 isEmptyBlockTag() ||
		 isTableTag() )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isTextLevelTag()	{

	if ( isFontStyleTag() ||
		 isPhraseTag() ||
		 isFormFieldTag() ||
		 isAnchorTag() ||
		 isEmptyTextTag() ||
		 isAppletTag() ||
		 isFontTag() ||
		 isMapTag() )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isHeadingTag()	{
	
	if ( mTag == "h1" ||
		 mTag == "h2" ||
		 mTag == "h3" ||
		 mTag == "h4" ||
		 mTag == "h5" ||
		 mTag == "h6" )	{
		return true;
	}
	
	return false;
	
}	 

bool HTMLParser	::	isAdressTag()	{
	
	if ( mTag == "adress" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isPTag()	{
	
	if ( mTag == "p" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isListTag()	{
	
	if ( isULTag() ||
		 isOLTag() ||
		 isDLTag() )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isULTag()	{
	
	if ( mTag == "ul" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isOLTag()	{
	
	if ( mTag == "ol" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isDLTag()	{
	
	if ( !mTag.compare( "dl" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isLITag()	{
	
	if ( mTag == "li" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isDDTag()	{
	
	if ( mTag == "dd" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isDTTag()	{
	
	if ( mTag == "dt" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isPreTag()	{
	
	if ( mTag == "pre" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isBodyStyleTag()	{

	if ( mTag == "div" ||
		 mTag == "center" ||
		 mTag == "blockquote" )	{
		return true;
	}
	
	return false;

}	

bool HTMLParser	::	isFormTag()	{
	
	if ( mTag == "form" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isEmptyBlockTag()	{

	if ( mTag == "isindex" ||
		 mTag == "hr" )	{
		return true;
	}
	
	return false;

}

bool HTMLParser	::	isTableTag()	{
	
	if ( mTag == "table" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isTrTag()	{
	
	if ( mTag == "tr" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isCaptionTag()	{
	
	if ( mTag == "caption" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isTdTag()	{
	
	if ( mTag == "td" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isThTag()	{
	
	if ( mTag == "th" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isFontStyleTag()	{
	
	if ( isFontStylePreTag() ||
		 mTag == "i" ||
		 mTag == "big" ||
		 mTag == "small" ||
		 mTag == "sub" ||
		 mTag == "sup" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isFontStylePreTag()	{
	
	if ( mTag == "tt" ||
		 mTag == "b" ||
		 mTag == "u" ||
		 mTag == "strike" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isPhraseTag()	{

	if ( mTag == "em" ||
		 mTag == "strong" ||
		 mTag == "dfn" ||
		 mTag == "code" ||
		 mTag == "samp" ||
		 mTag == "kbd" ||
		 mTag == "var" ||
		 mTag == "cite" )	{
		return true;
	}
	
	return false;

}	

bool HTMLParser	::	isFormFieldTag()	{
	
	if ( mTag == "input" ||
		 mTag == "select" ||
		 mTag == "textarea" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isInputTag()	{
	
	if ( mTag == "input" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isSelectTag()	{
	
	if ( mTag == "select" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isTextAreaTag()	{
	
	if ( mTag == "textarea" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isOptionTag()	{
	
	if ( mTag == "option" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isAnchorTag()	{
	
	if ( mTag == "a" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isEmptyTextTag()	{

	if ( mTag == "img" ||
		 isEmptyTextPreTag() )	{
		return true;
	}
	
	return false;

}	

bool HTMLParser	::	isEmptyTextPreTag()	{

	if ( mTag == "basefont" ||
		 mTag == "br" )	{
		return true;
	}
	
	return false;

}	

bool HTMLParser	::	isAppletTag()	{
	
	if ( mTag == "applet" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isParamTag()	{
	
	if ( mTag == "param" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isFontTag()	{
	
	if ( mTag == "font" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isMapTag()	{
	
	if ( mTag == "map" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isAreaTag()	{
	
	if ( mTag == "area" )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isWhiteSpace()	{
	
	if ( isspace( mContent[mPos] ) || iscntrl( mContent[mPos] ) )	{
		return true;
	}
	
	return false;
	
}

void HTMLParser	::	nextChar()	{

	if ( mPos != mContent.size() )	{
		if ( mContent[mPos] != '\n' )	{
			if ( mContent[mPos] != '\t' )	{
				mCharNr++;
			}
			else	{
				// I'm counting 4 characters for a tab
				mCharNr += 4;
			}
		}
		else	{
			mLineNr++;
			mCharNr = 1;
		}
		mPos++;
	}
	else	{
		throw ReadException();
	}
			
}

string HTMLParser	::	getTag()	{
	
	mOldPos = mPos;
	
	// Reset the tag and tag information
	mOldTag = mTag;
	mTag = "";
	mCloseTag = false;

	while ( mContent[mPos] != '<' )	{
		// Not the start of a tag yet
		nextChar();
	}
	
	// Store position of tag
	mOldTagLineNr = mLineNr;
	mOldTagCharNr = mCharNr;
	mTagLineNr = mLineNr;
	mTagCharNr = mCharNr;
	
	nextChar();

	if ( mContent[mPos] == '/' )	{
		// Is an end tag
		mCloseTag = true;
		nextChar();
	}
	
	while ( !isWhiteSpace() && mContent[mPos] != '>' )	{
		mTag += tolower( mContent[mPos] );
		nextChar();
	}
	
	return mTag;
	
}

string HTMLParser	::	getText( bool aConserveSpaces = false )	{
	
	mOldPos = mPos;
	
	while ( mContent[mPos] != '>' )	{
		nextChar();
	}
	
	nextChar();
	
	string result;
	char last = 's';
	
	while ( mContent[mPos] != '<' )	{
		if ( !aConserveSpaces )	{
			if ( mContent[mPos] != '\n' && mContent[mPos] != '\t' )	{
				if ( !( mContent[mPos] == ' ' && last == ' ' ) )	{
					result += mContent[mPos];
					last = mContent[mPos];
				}
			}
			else	{
				if ( mContent[mPos] == '\t' && last != ' ' )	{
					result += ' ';
					last = ' ';
				}
			}
		}
		else	{
			result += mContent[mPos];
		}
		nextChar();
	}
	
	return result;
	
}

string HTMLParser	::	getAttrValue()	{
	
	mOldPos = mPos;
	
	string result;
	
	nextChar();
	
	while ( isWhiteSpace() )	{
		nextChar();
	}
	
	if ( mContent[mPos] == '"' )	{
		nextChar();
		while ( mContent[mPos] != '"' )	{
			result += mContent[mPos];
			nextChar();
		}
		nextChar();
		
		return result;
		
	}

	if ( mContent[mPos] == '\'' )	{
		nextChar();
		while ( mContent[mPos] != '\'' )	{
			result += mContent[mPos];
			nextChar();
		}
		nextChar();
		
		return result;
		
	}		

	while ( mContent[mPos] != '>' && !isWhiteSpace() )	{
		result += mContent[mPos];
		nextChar();
	}
	
	return result;
	
}

string HTMLParser	::	getAttribute()	{

	mOldPos = mPos;
	mAttrNoValue = false;

	string result;

	while ( isWhiteSpace() )	{
		nextChar();
	}
	
	if ( mContent[mPos] == '>' )	{
		return result;
	}
	
	if ( mContent[mPos] == '/' )	{
		nextChar();
		if ( mContent[mPos] == '>' )	{
			return result;
		}
	}
	
	while ( mContent[mPos] != '=' && mContent[mPos] != '>' && !isWhiteSpace() )	{
		// I'm assuming case does not matter with attribute names
		result+= tolower( mContent[mPos] );
		nextChar();
	}
	
	while ( isWhiteSpace() )	{
		nextChar();
	}
	
	if ( mContent[mPos] != '=' )	{
		// Attribute doesn't have a value
		mAttrNoValue = true;
	}
	
	return result;
	
}

string HTMLParser	::	getComment()	{
	
	string result;
	
	while ( mContent[mPos] != '>' )	{
		result += mContent[mPos];
		nextChar();
	}
	
	char last = result[ result.size() - 1 ];
	
	if ( last == '-' )	{
		result.erase( result.size() - 1 );
		last = result[ result.size() - 1 ];
		if ( last == '-' )	{
			result.erase( result.size() - 1 );
			last = result[ result.size() - 1 ];
			while ( last == ' ' || last == '\t' || last == '\n' )	{
				result.erase( result.size() - 1 );
				last = result[ result.size() - 1 ];
			}
		}
	}
	
	return result;
	
}

string HTMLParser	::	getString( bool aConserveSpaces )	{

	string result;

	switch ( mContent[mPos] )	{
		case '>' :	{
			result = getText( aConserveSpaces );
			mStringType = TEXT;
			break;
		}
		case '<' :	{
			result = getTag();
			mStringType = TAG;
			break;
		}
		case '=' :	{
			result = getAttrValue();
			mStringType = ATTRVALUE;
			break;
		}
		default :	{
			result = getAttribute();
			mStringType = ATTR;
		}
	}

	if ( mPos == mContent.size() && result == "" )	{
		throw ReadException();
	}

	return result;
	
}

void HTMLParser	::	skipTag()	{
	
	while ( mContent[mPos] != '>' )	{
		// Not the end of this tag yet
		if ( mContent[mPos] == '"' )	{
			// Might be a > in the quote, not sure if it is legal
			nextChar();
			while ( mContent[mPos] != '"' )	{
				nextChar();
			}
		}
		nextChar();
	}
	
}

void HTMLParser	::	backPedal()	{
	
	mPos = mOldPos;
	
}

void HTMLParser	::	startParsing( TDocumentShared aDocument )	{
	
	mDocument = aDocument;
	
	bool insideDoc = true;

	try	{
	
		if ( mContent.size() == 0 )	{
			throw ReadException();
		}

		while ( insideDoc )	{
			getTag();
	
			if ( isDocTypeTag() )	{
				doctypeTag();
				continue;
			}
	
			if ( isHtmlTag() )	{
				htmlTag();
				// Last tag, quit the loop
				insideDoc = false;
				continue;
			}
			
			cout << "startParsing: Unexpected tag found: " << mTag << ". Skipping...\n";
			skipTag();
			
		}
	}
	catch ( ReadException r )	{
		cout << "Unexpected end of file..\n";
		cout << "Returning partial tree\n";
	}
	
}

void HTMLParser	::	doctypeTag()	{
	
	// I don't really care about this tag for now

}

void HTMLParser	::	htmlTag()	{

	// Add to DOM tree
	TElementShared element = mDocument->createElement( "html" );
	mDocument->appendChild( element );
	
	bool insideHtml = true;
	
	while ( insideHtml )	{
		getTag();

		if ( isStartTag() )	{
			if ( isHeadTag() )	{
				headTag( element );
				continue;
			}

			if ( isBodyTag() )	{
				bodyStyleTag( element );
				continue;
			}
			if ( isCommentTag() )	{
				commentTag( element );
				continue;
			}

			cout << mTagLineNr << " " << mTagCharNr << ": ";
			cout << "html: Unexpected tag found: " << mTag << ". Skipping...\n";
			skipTag();

		}
		else	{
			if ( !mTag.compare( "html" ) )	{

				insideHtml = false;
									
			}
			else	{
				cout << mTagLineNr << " " << mTagCharNr << ": ";
				cout << "html: Unexpected closing tag found: " << mTag << ". Skipping...\n";
			}
		}
	}
	
}

void HTMLParser	::	commentTag( TElementShared aParent )	{

	string data = getComment();

	// Add to parent
	TCommentShared comment = mDocument->createComment( data );
	aParent->appendChild( comment );

}

void HTMLParser	::	headTag( TElementShared aParent )	{

	// Add to parent
	TElementShared element = mDocument->createElement( "head" );
	aParent->appendChild( element );
	
	bool insideHead = true;
	
	while ( insideHead )	{
		getTag();

		if ( isStartTag() )	{
			if ( isTitleTag() ||
				 isStyleTag() ||
				 isScriptTag() )	{
				normalHeadTag( element );
				continue;
			}
			if ( isIsIndexTag() ||
				 isBaseTag() ||
				 isMetaTag() ||
				 isLinkTag() )	{
				emptyElementTag( element );
				continue;
			}
			if ( isCommentTag() )	{
				commentTag( element );
				continue;
			}
			
			// Not a known tag
			cout << mTagLineNr << " " << mTagCharNr << ": ";
			cout << "head: Unexpected tag found: " << mTag << ". Skipping...\n";
			skipTag();

		}
		else	{			
			if ( isHeadTag() )	{

				// End the while loop
				insideHead = false;
			}
			else	{
				cout << mTagLineNr << " " << mTagCharNr << ": ";
				cout << "head: Unexpected closing tag found: " << mTag << ". Skipping...\n";
			}
		}
	}
	
}

void HTMLParser	::	normalHeadTag( TElementShared aParent )	{

	// Save the tag name
	string tag = mTag;

	// Add to parent
	TElementShared element = mDocument->createElement( tag );
	aParent->appendChild( element );

	bool insideHeadLevel = true;
	
	string data;
	
	while ( insideHeadLevel )	{
		
		data += getText();
		
		getTag();

		if ( !isStartTag() )	{
			if ( !mTag.compare( tag ) )	{
				insideHeadLevel = false;
			}
			else	{
				cout << mTagLineNr << " " << mTagCharNr << ": ";
				cout << tag << ": Unexpected closing tag found: " << mTag << ". Skipping...\n";
			}
		}
		else	{
			if ( isCommentTag() )	{
				commentTag( element );
				continue;
			}
			cout << mTagLineNr << " " << mTagCharNr << ": ";
			cout << tag << ": Unexpected tag found: " << mTag << ". Skipping...\n";
			skipTag();
		}
	}

	TTextShared text = mDocument->createText( data );
	element->appendChild( text );

}

void HTMLParser	::	bodyStyleTag( TElementShared aParent, bool aInsideForm )	{
	
	// Save the tag name
	string tag = mTag;

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );

	bool insideBodyStyle = true;
	string attribute;
	
	while ( insideBodyStyle )	{
		// Warning: more possible than a tag only
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isHeadingTag() )	{
						normalTextTag( element );
						continue;
					}
					if ( isAdressTag() )	{
						adressTag( element );
						continue;
					}
					if ( isBlockLevelTag() )	{
						blockLevelTag( element, aInsideForm );
						continue;
					}
					if ( isTextLevelTag() )	{
						textLevelTag( element );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}
		
					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << tag << ": Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
		
						// End the while loop
						insideBodyStyle = false;
					}
					else	{
						cout << mTagLineNr << " " << mTagCharNr << ": ";
						cout << tag << ": Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	adressTag( TElementShared aParent )	{

	// Add to parent
	TElementShared element = mDocument->createElement( "adress" );
	aParent->appendChild( element );

	bool insideAdress = true;
	string attribute;
	
	while ( insideAdress )	{

		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isPTag() )	{
						pTag( element );
						continue;
					}
					if ( isTextLevelTag() )	{
						textLevelTag( element );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}
		
					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "adress: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( "adress" ) )	{
		
						// End the while loop
						insideAdress = false;
					}
					else	{
						cout << mTagLineNr << " " << mTagCharNr << ": ";
						cout << "adress: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}

}

void HTMLParser	::	blockLevelTag( TElementShared aParent, bool aInsideForm )	{

	if ( isPTag() )	{
		pTag( aParent );
		return;
	}
	if ( isListTag() )	{
		listTag( aParent );
		skipTag();
		return;
	}
	if ( isPreTag() )	{
		preTag( aParent );
		return;
	}
	if ( isBodyStyleTag() )	{
		bodyStyleTag( aParent );
		return;
	}
	if ( isFormTag() )	{
		if ( aInsideForm )	{
			// Not allowed here
			cout << mTagLineNr << " " << mTagCharNr << ": ";
			cout << mOldTag << ": Illegal tag found: " << mTag << ". Skipping...\n";
			skipTag();
			return;
		}
		else	{
			bodyStyleTag( aParent, true );
			return;
		}
	}
	if ( isEmptyBlockTag() )	{
		emptyElementTag( aParent );
		return;
	}
	if ( isTableTag() )	{
		tableTag( aParent );
		return;
	}

}

void HTMLParser	::	textLevelTag( TElementShared aParent, bool aConserveSpaces, bool aInsideAnchor )	{
	
	if ( isFontStyleTag() ||
		 isPhraseTag() ||
		 isFontTag() )	{
		normalTextTag( aParent, aConserveSpaces, aInsideAnchor );
		return;
	}
	if ( isFormFieldTag() )	{
		formFieldTag( aParent, aConserveSpaces );
		return;
	}
	if ( isAnchorTag() )	{
		if ( aInsideAnchor )	{
			// Not allowed here
			cout << mTagLineNr << " " << mTagCharNr << ": ";
			cout << mOldTag << ": Illegal tag found: " << mTag << ". Skipping...\n";
			skipTag();
			return;
		}
		else	{
			normalTextTag( aParent, aConserveSpaces, true );
			return;
		}
	}
	if ( isEmptyTextTag() )	{
		emptyElementTag( aParent );
		return;
	}
	if ( isAppletTag() )	{
		appletTag( aParent, aConserveSpaces, aInsideAnchor );
		return;
	}
	if ( isMapTag() )	{
		mapTag( aParent );
		return;
	}
	
}

void HTMLParser	::	flowLevelTag( TElementShared aParent )	{
	
	// Save the tag name
	string tag = mTag;

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );

	bool insideFlow = true;
	string attribute;
	
	while ( insideFlow )	{
		// Warning: more possible than a tag only
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isBlockLevelTag() )	{
						blockLevelTag( element );
						continue;
					}
					if ( isTextLevelTag() )	{
						textLevelTag( element );
						continue;
					}
					if ( !mTag.compare( tag ) )	{
		
						// End the while loop
						insideFlow = false;
						backPedal();
						continue;
					}
		
					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << tag << ": Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
		
						// End the while loop
						insideFlow = false;
						continue;
					}
					if ( isListTag() )	{
		
						// End the while loop
						insideFlow = false;
						backPedal();
						continue;
					}

					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << tag << ": Unexpected tag found: " << mTag << ". Skipping...\n";

				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}

}

void HTMLParser	::	formFieldTag( TElementShared aParent, bool aConserveSpaces )	{
	
	if ( isInputTag() )	{
		emptyElementTag( aParent );
		return;
	}
	if ( isSelectTag() )	{
		selectTag( aParent, aConserveSpaces );
		return;
	}
	if ( isTextAreaTag() )	{
		pcDataTag( aParent, aConserveSpaces );
		return;
	}

}

void HTMLParser	::	pTag( TElementShared aParent )	{

	// Add to parent
	TElementShared element = mDocument->createElement( "p" );
	aParent->appendChild( element );
	
	bool insideP = true;
	string attribute;
	
	while ( insideP )	{
		
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( !isStartTag() )	{
					if ( isPTag() )	{
						insideP = false;
						continue;
					}
					if ( isAdressTag() ||
						 isBodyStyleTag() ||
						 isFormTag() ||
						 isListTag() )	{
						insideP = false;
						backPedal();
						continue;
					}
		
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "p: Unexpected closing tag found: " << mTag << ". Skipping...\n";
		
				}
				else	{
					if ( isBlockLevelTag() ||
						 isHeadingTag() ||
						 isLITag() )	{
						insideP = false;
						backPedal();
						continue;
					}
					if ( isTextLevelTag() )	{
						textLevelTag( element );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}

					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "p: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();

				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}

}

void HTMLParser	::	listTag( TElementShared aParent )	{
	
	// Save the tag name
	string tag = mTag;

	// To check if this list can have list items or not
	bool listItem = false;

	if ( isULTag() || isOLTag() )	{
		listItem = true;
	}

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );

	bool insideList = true;
	string attribute;
	
	while ( insideList )	{
		// Warning: more possible than a tag only
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( ( isLITag() && listItem ) ||
						 ( isDDTag() && !listItem ) )	{
						flowLevelTag( element );
						continue;
					}
					if ( isDTTag() && !listItem )	{
						textLevelTag( element );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}
		
					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "list: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
		
						// End the while loop
						insideList = false;
					}
					else	{
						cout << mTagLineNr << " " << mTagCharNr << ": ";
						cout << "list: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	tableTag( TElementShared aParent )	{
	
	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );

	bool insideTable = true;
	string attribute;
	
	while ( insideTable )	{
		// Warning: more possible than a tag only
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isTrTag() )	{
						trTag( element );
						continue;
					}
					if ( isCaptionTag() )	{
						normalTextTag( element );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}
		
					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "table: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isTableTag() )	{
		
						// End the while loop
						insideTable = false;
					}
					else	{
						cout << mTagLineNr << " " << mTagCharNr << ": ";
						cout << "table: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	trTag( TElementShared aParent )	{
	
	// Add to parent
	TElementShared element = mDocument->createElement( "tr" );
	aParent->appendChild( element );

	bool insideTr = true;
	string attribute;
	
	while ( insideTr )	{
		// Warning: more possible than a tag only
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isThTag() || isTdTag() )	{
						bodyStyleTag( element );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}
		
					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "tr: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isTrTag() )	{
		
						// End the while loop
						insideTr = false;
					}
					else	{
						cout << mTagLineNr << " " << mTagCharNr << ": ";
						cout << "tr: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	preTag( TElementShared aParent )	{

	// Add to parent
	TElementShared element = mDocument->createElement( "pre" );
	aParent->appendChild( element );
	
	bool insidePre = true;
	string attribute;
	
	while ( insidePre )	{
		
		string data = getString( true );

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( !isStartTag() )	{
					if ( isPreTag() )	{
						insidePre = false;
						continue;
					}
		
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "pre: Unexpected closing tag found: " << mTag << ". Skipping...\n";
		
				}
				else	{
					if ( isFontStylePreTag() ||
						 isPhraseTag() )	{
						normalTextTag( element, true );
						continue;
					}
					if ( isFormFieldTag() )	{
						formFieldTag( element, true );
						continue;
					}
					if ( isAnchorTag() )	{
						normalTextTag( element, true, true );
						continue;
					}
					if ( isAppletTag() )	{
						appletTag( element, true );
						continue;
					}
					if ( isEmptyTextPreTag() )	{
						emptyElementTag( element );
						continue;
					}
					if ( isMapTag() )	{
						mapTag( element );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}

					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "pre: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();

				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	emptyElementTag( TElementShared aParent )	{

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );
	
	bool insideEmptyElement = true;
	
	string attribute;
	
	while ( insideEmptyElement )	{
		
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :
			case TEXT :	{
				// At the end of the empty element tag
				insideEmptyElement = false;
				backPedal();
				break;
			}
		}
	}
	
}

void HTMLParser	::	selectTag( TElementShared aParent, bool aConserveSpaces )	{
	
	// Add to parent
	TElementShared element = mDocument->createElement( "select" );
	aParent->appendChild( element );

	bool insideSelect = true;
	string attribute;
	
	while ( insideSelect )	{
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isOptionTag() )	{
						pcDataTag( element, aConserveSpaces );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}
		
					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "select: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isSelectTag() )	{
		
						// End the while loop
						insideSelect = false;
					}
					else	{
						cout << mTagLineNr << " " << mTagCharNr << ": ";
						cout << "select: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	pcDataTag( TElementShared aParent, bool aConserveSpaces )	{
	
	// Save the tag name
	string tag = mTag;

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );

	bool insidePcData = true;
	string attribute;
	
	while ( insidePcData )	{
		string data = getString( aConserveSpaces );

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}
		
					// End the while loop
					insidePcData = false;
					backPedal();
					continue;
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
		
						// End the while loop
						insidePcData = false;
					}
					else	{
		
						// End the while loop
						insidePcData = false;
						backPedal();
						continue;
					}
				}
				break;
			}
			case TEXT :	{
				if ( ( data.compare( " " ) && data.compare( "" ) ) || ( aConserveSpaces && data.compare( "" ) ) )	{
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	appletTag( TElementShared aParent, bool aConserveSpaces, bool aInsideAnchor )	{
	
	// Add to parent
	TElementShared element = mDocument->createElement( "applet" );
	aParent->appendChild( element );

	bool insideApplet = true;
	string attribute;
	
	while ( insideApplet )	{
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isParamTag() )	{
						emptyElementTag( element );
						continue;
					}
					if ( isTextLevelTag() )	{
						textLevelTag( element, aConserveSpaces, aInsideAnchor );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}
		
					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "applet: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isAppletTag() )	{
		
						// End the while loop
						insideApplet = false;
					}
					else	{
						cout << mTagLineNr << " " << mTagCharNr << ": ";
						cout << "applet: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( ( data.compare( " " ) && data.compare( "" ) ) || ( aConserveSpaces && data.compare( "" ) ) )	{
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	mapTag( TElementShared aParent )	{
	
	// Add to parent
	TElementShared element = mDocument->createElement( "map" );
	aParent->appendChild( element );

	bool insideMap = true;
	string attribute;
	
	while ( insideMap )	{
		string data = getString();

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isAreaTag() )	{
						emptyElementTag( element );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}
		
					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "map: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isMapTag() )	{
		
						// End the while loop
						insideMap = false;
					}
					else	{
						cout << mTagLineNr << " " << mTagCharNr << ": ";
						cout << "map: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( ( data.compare( " " ) && data.compare( "" ) ) )	{
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	normalTextTag( TElementShared aParent, bool aConserveSpaces, bool aInsideAnchor )	{

	// Save the tag name
	string tag = mTag;

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );
	
	bool insideNormalText = true;
	string text;
	string attribute;
	
	while ( insideNormalText )	{
		
		string data = getString( aConserveSpaces );

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
				if ( mAttrNoValue )	{
					element->setAttribute( attribute, "" );
					attribute = "";
				}
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, data );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( !isStartTag() )	{
					if ( !mTag.compare( tag ) )	{
						insideNormalText = false;
						continue;
					}
		
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << tag << ": Unexpected closing tag found: " << mTag << ". Skipping...\n";
		
				}
				else	{
					if ( isTextLevelTag() )	{
						textLevelTag( element, aConserveSpaces, aInsideAnchor );
						continue;
					}
					if ( isCommentTag() )	{
						commentTag( element );
						continue;
					}

					// Not a known tag
					cout << mTagLineNr << " " << mTagCharNr << ": ";
					cout << tag << ": Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();

				}
				break;
			}
			case TEXT :	{
				if ( ( data.compare( " " ) && data.compare( "" ) ) || ( aConserveSpaces && data.compare( "" ) ) )	{
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}

}

void HTMLParser	::	MessageReceived( BMessage * message )	{
	
	message->PrintToStream();
	
}

bool HTMLParser	::	IsHandler()	{
	
	return true;
	
}

BHandler * HTMLParser	::	Handler()	{
	
	return this;
	
}

bool HTMLParser	::	IsPersistent()	{
	
	return true;
	
}

uint32 HTMLParser	::	PlugID()	{
	
	return 'html';
	
}

char * HTMLParser	::	PlugName()	{
	
	return "HTML Parser";
	
}

float HTMLParser	::	PlugVersion()	{
	
	return 0.0;
	
}

void HTMLParser	::	Heartbeat()	{
	
}

status_t HTMLParser	::	ReceiveBroadcast( BMessage * message )	{
	
	printf( "HTMLParser is receiving broadcast:\n" );
	
	int32 command = 0;
	message->FindInt32( "command", &command );
	
	switch ( command )	{
		case COMMAND_INFO_REQUEST:	{
			printf( "COMMAND_INFO_REQUEST called\n" );
			int32 replyto = 0;
			message->FindInt32( "ReplyTo", &replyto );
			switch( message->what )	{
				case GetSupportedMIMEType:	{
					if ( message->HasBool( "supportedmimetypes" ) )	{
						BMessage types( SupportedMIMEType );
						types.AddString( "mimetype", "text/html" );
						types.AddInt32( "command", COMMAND_INFO );
						PlugClass * plug = NULL;
						if ( message->HasPointer( "ReplyToPointer" ) )	{
							message->FindPointer( "ReplyToPointer", (void**) &plug );
							if ( plug != NULL )	{
								printf( "Replying to broadcast\n" );
								plug->BroadcastReply( &types );
							}
						}
						else	{	
							BMessage container;
							container.AddMessage( "message", &types );
							//PlugMan->Broadcast( PlugID(), replyto, &container );
						}
					}
					break;
				}
			}
		}	
		case COMMAND_INFO:	{
			switch ( message->what )	{
				case PlugInLoaded:	{
					PlugClass * plug = NULL;
					message->FindPointer(  "plugin", (void **) &plug );
					if ( plug != NULL )	{
							printf( "A plugin has been loaded\n" );
							if ( ( plug->Type() & TARGET_CACHE ) != 0 )	{
								cache = (CachePlug *) plug;
								userToken = cache->Register( Type(), "HTML Parser" );
							}
					}
					break;
				}
				case PlugInUnLoaded:	{
					uint32 type = 0;
					type = message->FindInt32( "type" );
					if ( ( type & TARGET_CACHE ) != 0 )	{
						printf( "Cache unloaded\n" );
						cache = NULL;
					}
					break;
				}
				case ProtocolConnectionClosed:	{
					printf( "Got data\n" );
					
					bool requestDone = false;
					message->FindBool( "request_done", &requestDone );
					
					if ( !requestDone )	{
						// I'll wait
						break;
					}
					
					const char * url = NULL;
					message->FindString( "url", &url );
					
					if ( url != NULL )	{
						printf( "Getting data of url: %s\n", url );
					}
					else	{
						// What the heck
						break;
					}
			
					int32 fileToken = cache->FindObject( userToken, url );
					ssize_t fileSize = cache->GetObjectSize( userToken, fileToken );
					
					printf( "File size: %i\n", (int) fileSize );
					
					string content;
					char * buffer = new char[ 2000 ];
					ssize_t bytesRead = 0;
					int totalBytes = 0;
					bytesRead = cache->Read( userToken, fileToken, buffer, 2000 );
					while (  bytesRead > 0 )	{
						totalBytes += bytesRead;
						if ( totalBytes > fileSize )	{
							printf( "Ahem. Whose bug is this ?\n" );
							printf( "Complete text gotten:\n%s", content.c_str() );
							break;
						}
						printf( "Got part of data: %i\n", totalBytes  );
						content += buffer;
						memset( buffer, 0, 1000 );
						bytesRead = cache->Read( userToken, fileToken, buffer, 2000 );
					}
					delete[] buffer;

					printf( "Put all data in content\n" );
			
					// Create a DOM document
					TDocumentShared document( new TDocument() );
					document->setSmartPointer( document );
					
					// Reset to initial values
					reset();

					// Set the content and start parsing
					setContent( content );
					startParsing( document );
					
					printf( "Data parsed\n" );
			
					//showDocument();

					if ( PlugMan )	{
						BMessage * done = new BMessage( ReturnedData );
						done->AddInt32( "command", COMMAND_INFO );
						done->AddString( "type", "dom" );
						done->AddPointer( "data_pointer", &mDocument );
						
						BMessage container;
						container.AddMessage( "message", done );
						delete done;
						done = NULL;
						
						PlugMan->Broadcast( TARGET_PARSER, ALL_TARGETS, &container );
					}
					break;
				}
			}
			break;
		}
		default:	{
			message->PrintToStream();
			return PLUG_DOESNT_HANDLE;
		}
	}
	
	return PLUG_HANDLE_GOOD;
	
}

int32 HTMLParser	::	Type()	{
	
	return TARGET_PARSER;
	
}
