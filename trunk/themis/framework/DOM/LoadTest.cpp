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

HTMLParser	::	HTMLParser()	{

	mPos = 0;
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
	
	if ( !mTag.compare( "!doctype" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isHtmlTag()	{
	
	if ( !mTag.compare( "html" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isHeadTag()	{
	
	if ( !mTag.compare( "head" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isBodyTag()	{
	
	if ( !mTag.compare( "body" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isTitleTag()	{
	
	if ( !mTag.compare( "title" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isStyleTag()	{
	
	if ( !mTag.compare( "style" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isScriptTag()	{
	
	if ( !mTag.compare( "script" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isIsIndexTag()	{
	
	if ( !mTag.compare( "isindex" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isBaseTag()	{
	
	if ( !mTag.compare( "base" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isMetaTag()	{
	
	if ( !mTag.compare( "meta" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isLinkTag()	{
	
	if ( !mTag.compare( "link" ) )	{
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
	
	if ( !mTag.compare( "h1" ) ||
		 !mTag.compare( "h2" ) ||
		 !mTag.compare( "h3" ) ||
		 !mTag.compare( "h4" ) ||
		 !mTag.compare( "h5" ) ||
		 !mTag.compare( "h6" ) )	{
		return true;
	}
	
	return false;
	
}	 

bool HTMLParser	::	isAdressTag()	{
	
	if ( !mTag.compare( "adress" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isPTag()	{
	
	if ( !mTag.compare( "p" ) )	{
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
	
	if ( !mTag.compare( "ul" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isOLTag()	{
	
	if ( !mTag.compare( "ol" ) )	{
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
	
	if ( !mTag.compare( "li" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isDDTag()	{
	
	if ( !mTag.compare( "dd" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isDTTag()	{
	
	if ( !mTag.compare( "dt" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isPreTag()	{
	
	if ( !mTag.compare( "pre" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isBodyStyleTag()	{

	if ( !mTag.compare( "div" ) ||
		 !mTag.compare( "center" ) ||
		 !mTag.compare( "blockquote" ) )	{
		return true;
	}
	
	return false;

}	

bool HTMLParser	::	isFormTag()	{
	
	if ( !mTag.compare( "form" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isEmptyBlockTag()	{

	if ( !mTag.compare( "isindex" ) ||
		 !mTag.compare( "hr" ) )	{
		return true;
	}
	
	return false;

}

bool HTMLParser	::	isTableTag()	{
	
	if ( !mTag.compare( "table" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isFontStyleTag()	{
	
	if ( isFontStylePreTag() ||
		 !mTag.compare( "i" ) ||
		 !mTag.compare( "big" ) ||
		 !mTag.compare( "small" ) ||
		 !mTag.compare( "sub" ) ||
		 !mTag.compare( "sup" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isFontStylePreTag()	{
	
	if ( !mTag.compare( "tt" ) ||
		 !mTag.compare( "b" ) ||
		 !mTag.compare( "u" ) ||
		 !mTag.compare( "strike" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isPhraseTag()	{

	if ( !mTag.compare( "em" ) ||
		 !mTag.compare( "strong" ) ||
		 !mTag.compare( "dfn" ) ||
		 !mTag.compare( "code" ) ||
		 !mTag.compare( "samp" ) ||
		 !mTag.compare( "kbd" ) ||
		 !mTag.compare( "var" ) ||
		 !mTag.compare( "cite" ) )	{
		return true;
	}
	
	return false;

}	

bool HTMLParser	::	isFormFieldTag()	{
	
	if ( !mTag.compare( "input" ) ||
		 !mTag.compare( "select" ) ||
		 !mTag.compare( "textarea" ) )	{
		return true;
	}
	
	return false;
	
}	 

bool HTMLParser	::	isAnchorTag()	{
	
	if ( !mTag.compare( "a" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isEmptyTextTag()	{

	if ( !mTag.compare( "img" ) ||
		 isEmptyTextPreTag() )	{
		return true;
	}
	
	return false;

}	

bool HTMLParser	::	isEmptyTextPreTag()	{

	if ( !mTag.compare( "basefont" ) ||
		 !mTag.compare( "br" ) )	{
		return true;
	}
	
	return false;

}	

bool HTMLParser	::	isAppletTag()	{
	
	if ( !mTag.compare( "applet" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isFontTag()	{
	
	if ( !mTag.compare( "font" ) )	{
		return true;
	}
	
	return false;
	
}

bool HTMLParser	::	isMapTag()	{
	
	if ( !mTag.compare( "map" ) )	{
		return true;
	}
	
	return false;
	
}

string HTMLParser	::	getTag()	{
	
	mOldPos = mPos;
	
	// Reset the tag and tag information
	mTag = "";
	mCloseTag = false;

	while ( mPos != mContent.size() && mContent[mPos] != '<' )	{
		// Not the start of a tag yet
		mPos++;
	}
	
	if ( mPos == mContent.size() )	{
		mTag = "";
	}
	else	{
		mPos++;
	}

	if ( mPos != mContent.size() && mContent[mPos] == '/' )	{
		// Is an end tag
		mCloseTag = true;
		mPos++;
	}
	
	while ( mPos != mContent.size() && mContent[mPos] != ' ' && mContent[mPos] != '>' )	{
		mTag += tolower( mContent[mPos] );
		mPos++;
	}
	
	return mTag;
	
}

string HTMLParser	::	getText( bool aConserveSpaces = false )	{
	
	mOldPos = mPos;
	
	while ( mPos != mContent.size() && mContent[mPos] != '>' )	{
		mPos++;
	}
	
	if ( mPos != mContent.size() )	{
		mPos++;
	}
	else	{
		return "";
	}
	
	string result;
	char last = 's';
	
	while ( mPos != mContent.size() && mContent[mPos] != '<' )	{
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
		mPos++;
	}
	
	return result;
	
}

string HTMLParser	::	getAttrValue()	{
	
	mOldPos = mPos;
	
	string result;
	
	mPos++;
	
	while ( mPos != mContent.size() && mContent[mPos] == ' ' )	{
		mPos++;
	}
	
	if ( mPos == mContent.size() )	{
		return result;
	}
	
	if ( mContent[mPos] == '"' )	{
		mPos++;
		while ( mPos != mContent.size() && mContent[mPos] != '"' )	{
			result += mContent[mPos];
			mPos++;
		}
		mPos++;
	}
	else	{
		while ( mPos != mContent.size() && mContent[mPos] != '>' && mContent[mPos] != ' ' )	{
			result += mContent[mPos];
			mPos++;
		}
	}
	
	return result;
	
}

string HTMLParser	::	getAttribute()	{

	mOldPos = mPos;

	string result;

	while ( mPos != mContent.size() && mContent[mPos] == ' ' )	{
		mPos++;
	}
	
	if ( mPos == mContent.size() || mContent[mPos] == '>' )	{
		return result;
	}
	
	while ( mPos != mContent.size() && mContent[mPos] != '=' && mContent[mPos] != ' ' && mContent[mPos] != '>' )	{
		// I'm assuming case does not matter with attribute names
		result+= tolower( mContent[mPos] );
		mPos++;
	}
	
	return result;
	
}

string HTMLParser	::	getString( bool aConserveSpaces )	{

	cout << "Getting string...\n";

	string result;

	if ( mPos != mContent.size() )	{
		switch ( mContent[mPos] )	{
			case '>' :	{
				cout << "Getting text...\n";
				result = getText( aConserveSpaces );
				mStringType = TEXT;
				break;
			}
			case '<' :	{
				cout << "Getting tag...\n";
				result = getTag();
				mStringType = TAG;
				break;
			}
			case '=' :	{
				cout << "Getting attribute value...\n";
				result = getAttrValue();
				mStringType = ATTRVALUE;
				break;
			}
			default :	{
				cout << "Getting attribute...\n";
				result = getAttribute();
				mStringType = ATTR;
			}
		}
	}

	return result;
	
}

void HTMLParser	::	skipTag()	{
	
	while ( mPos != mContent.size() && mContent[mPos] != '>' )	{
		// Not the end of this tag yet
		if ( mContent[mPos] == '"' )	{
			// Might be a > in the quote, not sure if it is legal
			mPos++;
			while ( mPos != mContent.size() && mContent[mPos] != '"' )	{
				mPos++;
			}
		}
		mPos++;
	}
	
}

void HTMLParser	::	backPedal()	{
	
	mPos = mOldPos;
	
}

void HTMLParser	::	startParsing( TDocumentShared aDocument )	{
	
	mDocument = aDocument;
	
	bool insideDoc = true;
	
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
		
		cout << "Unknown tag found. Skipping...\n";
		skipTag();
		
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

			cout << "Unknown tag found. Skipping...\n";
			skipTag();

		}
		else	{
			if ( !mTag.compare( "html" ) )	{
				cout << "html closing tag found\n";

				insideHtml = false;
									
			}
			else	{
				cout << "Unknown closing tag found. Skipping...\n";
			}
		}
	}
	
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
			
			// Not a known tag
			cout << "Unknown tag found. Skipping...\n";
			skipTag();

		}
		else	{			
			if ( isHeadTag() )	{
				cout << "head closing tag found\n";

				// End the while loop
				insideHead = false;
			}
			else	{
				cout << "Unknown closing tag found. Skipping...\n";
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
				cout << "Unknown closing tag found. Skipping...\n";
			}
		}
		else	{
			cout << "Unknown tag found. Skipping...\n";
			skipTag();
		}
	}

	TTextShared text = mDocument->createText( data );
	element->appendChild( text );

	cout << "Text is: " << data << endl;
	
}

void HTMLParser	::	bodyStyleTag( TElementShared aParent )	{
	
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

		cout << "Received string " << data.size() << ": " << data << endl;

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
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
						blockLevelTag( element );
						continue;
					}
					if ( isTextLevelTag() )	{
						textLevelTag( element );
						continue;
					}
		
					// Not a known tag
					cout << "Unknown tag found. Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
						cout << mTag << " closing tag found\n";
		
						// End the while loop
						insideBodyStyle = false;
					}
					else	{
						cout << "Unknown closing tag found. Skipping...\n";
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

	cout << " adress tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "adress" );
	aParent->appendChild( element );

	bool insideAdress = true;
	string attribute;
	
	while ( insideAdress )	{

		string data = getString();

		cout << "Received string " << data.size() << ": " << data << endl;

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
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
		
					// Not a known tag
					cout << "Unknown tag found. Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( "adress" ) )	{
						cout << mTag << " closing tag found\n";
		
						// End the while loop
						insideAdress = false;
					}
					else	{
						cout << "Unknown closing tag found. Skipping...\n";
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

void HTMLParser	::	blockLevelTag( TElementShared aParent )	{

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
		//formTag( aParent );
		skipTag();
		return;
	}
	if ( isEmptyBlockTag() )	{
		emptyElementTag( aParent );
		return;
	}
	if ( isTableTag() )	{
		//tableTag( aParent );
		skipTag();
		return;
	}

}

void HTMLParser	::	textLevelTag( TElementShared aParent )	{
	
	if ( isFontStyleTag() ||
		 isPhraseTag() ||
		 isFontTag() )	{
		normalTextTag( aParent );
		return;
	}
	if ( isFormFieldTag() )	{
		//formFieldTag( aParent );
		skipTag();
		return;
	}
	if ( isAnchorTag() )	{
		aTag( aParent );
		return;
	}
	if ( isEmptyTextTag() )	{
		emptyElementTag( aParent );
		return;
	}
	if ( isAppletTag() )	{
		//appletTag( aParent );
		skipTag();
		return;
	}
	if ( isMapTag() )	{
		//mapTag( aParent );
		skipTag();
		return;
	}
	
}

void HTMLParser	::	flowLevelTag( TElementShared aParent )	{
	
	if ( isBlockLevelTag() )	{
		blockLevelTag( aParent );
		return;
	}
	if ( isTextLevelTag() )	{
		textLevelTag( aParent );
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

		cout << "Received string " << data.size() << ": " << data << endl;

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
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
						 isFormTag() )	{
						cout << "p closed implicitly\n";
						insideP = false;
						backPedal();
						continue;
					}
		
					cout << "Unknown closing tag found. Skipping...\n";
		
				}
				else	{
					if ( isBlockLevelTag() )	{
						cout << "p closed implicitly\n";
						insideP = false;
						backPedal();
						continue;
					}
					if ( isTextLevelTag() )	{
						textLevelTag( element );
						continue;
					}

					// Not a known tag
					cout << "Unknown tag found. Skipping...\n";
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

		cout << "Received string " << data.size() << ": " << data << endl;

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
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
		
					// Not a known tag
					cout << "Unknown tag found. Skipping...\n";
					skipTag();
					
				}
				else	{			
					if ( !mTag.compare( tag ) )	{
						cout << mTag << " closing tag found\n";
		
						// End the while loop
						insideList = false;
					}
					else	{
						cout << "Unknown closing tag found. Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				cout << "Text found in illegal place. Skipping...\n";
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

		cout << "Received string " << data.size() << ": " << data << endl;

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
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
		
					cout << "Unknown closing tag found. Skipping...\n";
		
				}
				else	{
					if ( isFontStylePreTag() ||
						 isPhraseTag() )	{
						normalTextTag( element, true );
						continue;
					}
/*
					if ( isFormFieldTag() )	{
						formFielTag( element, true );
						continue;
					}
*/
					if ( isAnchorTag() )	{
						aTag( element, true );
						continue;
					}
/*
					if ( isAppletTag() )	{
						appletTag( element, true );
						continue;
					}
*/	
					if ( isEmptyTextPreTag() )	{
						emptyElementTag( element );
						continue;
					}
/*
					if ( isMap() )	{
						mapTag( element );
						continue;
					}
*/						
					// Not a known tag
					cout << "Unknown tag found. Skipping...\n";
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

void HTMLParser	::	aTag( TElementShared aParent, bool aConserveSpaces )	{

	cout << "a tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "a" );
	aParent->appendChild( element );
	
	bool insideA = true;
	
	string attribute;
	
	while ( insideA )	{
		
		string data = getString( aConserveSpaces );

		cout << "Received string " << data.size() << ": " << data << endl;

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
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
					if ( !mTag.compare( "a" ) )	{
						cout << " a closing tag found\n";
						insideA = false;
						continue;
					}
		
					cout << "Unknown closing tag found. Skipping...\n";
		
				}
				else	{
					if ( isFontStyleTag() || isPhraseTag() )	{
						textLevelTag( element );
						continue;
					}
					if ( !mTag.compare( "font" ) )	{
						textLevelTag( element );
						continue;
					}
					if ( isEmptyTextTag() )	{
						emptyElementTag( element );
						continue;
					}

					// Not a known tag
					cout << "Unknown tag found. Skipping...\n";
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

void HTMLParser	::	emptyElementTag( TElementShared aParent )	{

	cout << mTag << " tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );
	
	bool insideEmptyElement = true;
	
	string attribute;
	
	while ( insideEmptyElement )	{
		
		string data = getString();

		cout << "Received string " << data.size() << ": " << data << endl;

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
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

void HTMLParser	::	normalTextTag( TElementShared aParent, bool aConserveSpaces )	{

	cout << mTag << " tag found\n";

	// Save the tag name
	string tag = mTag;

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );
	
	bool insideTextLevel = true;
	string text;
	string attribute;
	
	while ( insideTextLevel )	{
		
		string data = getString( aConserveSpaces );

		cout << "Received string " << data.size() << ": " << data << endl;

		switch ( mStringType )	{
			case ATTR :	{
				attribute = data;
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
						insideTextLevel = false;
						continue;
					}
		
					cout << "Unknown closing tag found. Skipping...\n";
		
				}
				else	{
					if ( isFontStyleTag() || isPhraseTag() )	{
						textLevelTag( element );
						continue;
					}
					if ( !mTag.compare( "a" ) )	{
						aTag( element );
						continue;
					}

					// Not a known tag
					cout << "Unknown tag found. Skipping...\n";
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
