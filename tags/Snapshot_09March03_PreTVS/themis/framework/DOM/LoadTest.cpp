/*	LoadTest
	Testing of how to load an HTML document
	Will end up in the trash can (probably)

	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	01-07-2002

*/

#include <fstream>
#include <string>
#include <algorithm>
#include <ctype.h>

#include "LoadTest.h"

#include "TDocument.h"
#include "TElement.h"
#include "TNode.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"
#include "TText.h"
#include "TComment.h"

HTMLParser	::	HTMLParser()	{

	mPos = 0;
	mOldPos = 0;
	mLineNr = 1;
	mCharNr = 1;
	mTag = "";
	mContent = "";
	mCloseTag = false;
	
}

HTMLParser	::	~HTMLParser()	{
	
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
	
	if ( mContent[mPos] == ' ' || mContent[mPos] == '\t' || mContent[mPos] == '\n' ||
		 mContent[mPos] == '\0' || isspace( mContent[mPos] ) || mContent[mPos] == '\xa' )	{
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
	mTag = "";
	mCloseTag = false;

	while ( mContent[mPos] != '<' )	{
		// Not the start of a tag yet
		nextChar();
	}
	
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
			cout << "End of tag. No attribute... Next character: " << mContent[mPos] << endl;
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
			cout << mLineNr << " " << mCharNr << ": Getting text...\n";
			result = getText( aConserveSpaces );
			mStringType = TEXT;
			break;
		}
		case '<' :	{
			cout << mLineNr << " " << mCharNr << ": Getting tag...\n";
			result = getTag();
			mStringType = TAG;
			break;
		}
		case '=' :	{
			cout << mLineNr << " " << mCharNr << ": Getting attribute value: ";
			result = getAttrValue();
			cout << result << endl;
			mStringType = ATTRVALUE;
			break;
		}
		default :	{
			cout << mLineNr << " " << mCharNr << ": Getting attribute: ";
			result = getAttribute();
			cout << result << endl;
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
	cout << "doctype tag found\n";

}

void HTMLParser	::	htmlTag()	{

	cout << "html tag found\n";

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

			cout << "htmlTag: Unexpected tag found: " << mTag << ". Skipping...\n";
			skipTag();

		}
		else	{
			if ( !mTag.compare( "html" ) )	{
				cout << "html closing tag found\n";

				insideHtml = false;
									
			}
			else	{
				cout << "htmlTag: Unexpected closing tag found: " << mTag << ". Skipping...\n";
			}
		}
	}
	
}

void HTMLParser	::	commentTag( TElementShared aParent )	{

	cout << "comment tag found\n";

	string data = getComment();

	// Add to parent
	TCommentShared comment = mDocument->createComment( data );
	aParent->appendChild( comment );

	cout << "Comment:\n";
	cout << data << endl;
	
}

void HTMLParser	::	headTag( TElementShared aParent )	{

	cout << "head tag found\n";
	
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
			cout << "headTag: Unexpected tag found: " << mTag << ". Skipping...\n";
			skipTag();

		}
		else	{			
			if ( isHeadTag() )	{
				cout << "head closing tag found\n";

				// End the while loop
				insideHead = false;
			}
			else	{
				cout << "headTag: Unexpected closing tag found: " << mTag << ". Skipping...\n";
			}
		}
	}
	
}

void HTMLParser	::	normalHeadTag( TElementShared aParent )	{

	cout << mTag << " tag found\n";

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
				cout << mTag << " closing tag found\n";
				insideHeadLevel = false;
			}
			else	{
				cout << "normalHead: Unexpected closing tag found: " << mTag << ". Skipping...\n";
			}
		}
		else	{
			if ( isCommentTag() )	{
				commentTag( element );
				continue;
			}
			cout << "normalHead: Unexpected tag found: " << mTag << ". Skipping...\n";
			skipTag();
		}
	}

	TTextShared text = mDocument->createText( data );
	element->appendChild( text );

	cout << "Text is: " << data << endl;
	
}

void HTMLParser	::	bodyStyleTag( TElementShared aParent, bool aInsideForm )	{
	
	cout << mTag << " tag found\n";

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
					cout << "bodyStyle: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
						cout << mTag << " closing tag found\n";
		
						// End the while loop
						insideBodyStyle = false;
					}
					else	{
						cout << "bodyStyle: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << "Text is:" << endl << data << endl;
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	adressTag( TElementShared aParent )	{

	cout << "adress tag found\n";

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
					cout << "adress: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( "adress" ) )	{
						cout << mTag << "closing tag found\n";
		
						// End the while loop
						insideAdress = false;
					}
					else	{
						cout << "adress: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << "Text is:" << endl << data << endl;
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
			cout << "blockLevel: Illegal tag found. Skipping...\n";
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
			cout << "textLevel: Illegal tag found. Skipping...\n";
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
	
	cout << mTag << " tag found\n";

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
						cout << mTag << " closed implicitly\n";
		
						// End the while loop
						insideFlow = false;
						backPedal();
						continue;
					}
		
					// Not a known tag
					cout << "flowLevel: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
						cout << mTag << " closing tag found\n";
		
						// End the while loop
						insideFlow = false;
						continue;
					}
					if ( isListTag() )	{
						cout << mTag << " closed implicitly\n";
		
						// End the while loop
						insideFlow = false;
						backPedal();
						continue;
					}

					cout << "flowLevel: Unexpected closing tag found: " << mTag << ". Skipping...\n";

				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << "Text is:" << endl << data << endl;
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

	cout << "p tag found\n";

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
						cout << "p closing tag found\n";
						insideP = false;
						continue;
					}
					if ( isAdressTag() ||
						 isBodyStyleTag() ||
						 isFormTag() ||
						 isListTag() )	{
						cout << "p closed implicitly\n";
						insideP = false;
						backPedal();
						continue;
					}
		
					cout << "p: Unexpected closing tag found: " << mTag << ". Skipping...\n";
		
				}
				else	{
					if ( isBlockLevelTag() ||
						 isHeadingTag() ||
						 isLITag() )	{
						cout << "p closed implicitly\n";
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
					cout << "p: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();

				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << "Text is:" << endl << data << endl;
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}

}

void HTMLParser	::	listTag( TElementShared aParent )	{
	
	cout << mTag << " tag found\n";

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
					cout << "list: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
						cout << mTag << " closing tag found\n";
		
						// End the while loop
						insideList = false;
					}
					else	{
						cout << "list: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	tableTag( TElementShared aParent )	{
	
	cout << "table tag found\n";

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
					cout << "table: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isTableTag() )	{
						cout <<  "table closing tag found\n";
		
						// End the while loop
						insideTable = false;
					}
					else	{
						cout << "table: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	trTag( TElementShared aParent )	{
	
	cout << "tr tag found\n";

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
					cout << "tr: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isTrTag() )	{
						cout <<  "tr closing tag found\n";
		
						// End the while loop
						insideTr = false;
					}
					else	{
						cout << "tr: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	preTag( TElementShared aParent )	{

	cout << "pre tag found\n";

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
						cout << "pre closing tag found\n";
						insidePre = false;
						continue;
					}
		
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
					cout << "pre: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();

				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << "Text is:" << endl << data << endl;
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	emptyElementTag( TElementShared aParent )	{

	cout << mTag << " tag found\n";

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
	
	cout << "select tag found\n";

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
					cout << "select: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isSelectTag() )	{
						cout <<  "tr closing tag found\n";
		
						// End the while loop
						insideSelect = false;
					}
					else	{
						cout << "select: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( data.compare( " " ) && data.compare( "" ) )	{
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	pcDataTag( TElementShared aParent, bool aConserveSpaces )	{
	
	cout << mTag << " tag found\n";

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
		
					cout << mTag << " closed implicitly\n";
	
					// End the while loop
					insidePcData = false;
					backPedal();
					continue;
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
						cout << mTag << " closing tag found\n";
		
						// End the while loop
						insidePcData = false;
					}
					else	{
						cout << mTag << " closed implicitly\n";
		
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
					cout << "Text is:" << endl << data << endl;
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	appletTag( TElementShared aParent, bool aConserveSpaces, bool aInsideAnchor )	{
	
	cout << "applet tag found\n";

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
					cout << "applet: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isAppletTag() )	{
						cout <<  "tr closing tag found\n";
		
						// End the while loop
						insideApplet = false;
					}
					else	{
						cout << "applet: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( ( data.compare( " " ) && data.compare( "" ) ) || ( aConserveSpaces && data.compare( "" ) ) )	{
					cout << "Text is:" << endl << data << endl;
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	mapTag( TElementShared aParent )	{
	
	cout << "map tag found\n";

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
					cout << "map: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( isMapTag() )	{
						cout <<  "map  closing tag found\n";
		
						// End the while loop
						insideMap = false;
					}
					else	{
						cout << "map: Unexpected closing tag found: " << mTag << ". Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				if ( ( data.compare( " " ) && data.compare( "" ) ) )	{
					cout << "Text found in illegal place. Skipping...\n";
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	normalTextTag( TElementShared aParent, bool aConserveSpaces, bool aInsideAnchor )	{

	cout << mTag << " tag found\n";

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
						cout << tag << " closing tag found\n";
						insideNormalText = false;
						continue;
					}
		
					cout << "normalText: Unexpected closing tag found: " << mTag << ". Skipping...\n";
		
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
					cout << "normalText: Unexpected tag found: " << mTag << ". Skipping...\n";
					skipTag();

				}
				break;
			}
			case TEXT :	{
				if ( ( data.compare( " " ) && data.compare( "" ) ) || ( aConserveSpaces && data.compare( "" ) ) )	{
					cout << "Text is:" << endl << data << endl;
					TTextShared text = mDocument->createText( data );
					element->appendChild( text );
				}
				break;
			}
		}
	}

}

int main( int argc, char * argv[] )	{
	
	ifstream file;

	if ( argc < 2 )	{
		cout << "Please supply a document to load\n";
		return 1;
	}
	else	{
		file.open( argv[ 1 ] );
	}

	cout << "Loading document...\n";
	string content;
	char ch;
	while ( file.get( ch ) )	{
		content += ch;
	}
	cout << "Document loaded. " << content.size() << " characters\n";
	cout << "Parsing file...\n";
	
	// Create a DOM document
	TDocumentShared document( new TDocument() );
	document->setSmartPointer( document );
	
	// Create a HTMLParser object
	HTMLParser htmlParser;
	htmlParser.setContent( content );

	htmlParser.startParsing( document );
	
	htmlParser.showDocument();
	
	return 0;
	
}
