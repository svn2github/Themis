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
void HTMLParser	::	setContent( string aContent )	{
	
	mContent = aContent;
	
}

bool HTMLParser	::	isStartTag()	{
	
	return !mCloseTag;

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

bool HTMLParser	::	isFontStyleTag()	{
	
	if ( !mTag.compare( "tt" ) ||
		 !mTag.compare( "i" ) ||
		 !mTag.compare( "b" ) ||
		 !mTag.compare( "u" ) ||
		 !mTag.compare( "strike" ) ||
		 !mTag.compare( "big" ) ||
		 !mTag.compare( "small" ) ||
		 !mTag.compare( "sub" ) ||
		 !mTag.compare( "sup" ) )	{
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

bool HTMLParser	::	isEmptyElementTag()	{

	if ( !mTag.compare( "img" ) ||
		 !mTag.compare( "br" ) )	{
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

string HTMLParser	::	getText( bool aConserveSpaces = true )	{
	
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

string HTMLParser	::	getString()	{

	cout << "Getting string...\n";

	string result;

	if ( mPos != mContent.size() )	{
		switch ( mContent[mPos] )	{
			case '>' :	{
				cout << "Getting text...\n";
				result = getText( false );
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

void HTMLParser	::	backPedal()	{
	
	mPos = mOldPos;
	
}

void HTMLParser	::	startParsing( TDocumentShared aDocument )	{
	
	mDocument = aDocument;
	
	bool insideDoc = true;
	
	while ( insideDoc )	{
		getTag();

		if ( !mTag.compare( "!doctype" ) )	{
			doctypeTag();
			continue;
		}

		if ( !mTag.compare( "html" ) )	{
			htmlTag();
			// Last tag, quit the loop
			insideDoc = false;
			continue;
		}
		
		cout << "Unknown tag found. Skipping...\n";
		
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
			if ( !mTag.compare( "head" ) )	{
				headTag( element );
				continue;
			}

			if ( !mTag.compare( "body" ) )	{
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
			if ( !mTag.compare( "title" ) )	{
				titleTag( element );
				continue;
			}
/*
			if ( !mTag.compare( "style" ) )	{
				styleTag( element );
				continue;
			}
			if ( !mTag.compare( "script" ) )	{
				scriptTag( element );
				continue;
			}
			if ( !mTag.compare( "isindex" ) )	{
				isindexTag( element );
				continue;
			}
			if ( !mTag.compare( "base" ) )	{
				baseTag( element );
				continue;
			}
			if ( !mTag.compare( "meta" ) )	{
				metaTag( element );
				continue;
			}
			if ( !mTag.compare( "link" ) )	{
				linkTag( element );
				continue;
			}
*/			
			// Not a known tag
			cout << "Unknown tag found. Skipping...\n";
			skipTag();

		}
		else	{			
			if ( !mTag.compare( "head" ) )	{
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

void HTMLParser	::	titleTag( TElementShared aParent )	{

	cout << "title tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "title" );
	aParent->appendChild( element );

	bool insideTitle = true;
	
	string title;
	
	while ( insideTitle )	{
		
		title += getText();
		
		getTag();

		if ( !isStartTag() )	{
			if ( !mTag.compare( "title" ) )	{
				cout << "title closing tag found\n";
				insideTitle = false;
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

	TTextShared text = mDocument->createText( title );
	element->appendChild( text );

	cout << "Title is: " << title << endl;
	
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
		string text = getString();

		cout << "Received string " << text.size() << ": " << text << endl;

		switch ( mStringType )	{
			case ATTR :	{
				attribute = text;
				break;
			}
			case ATTRVALUE :	{
				if ( attribute.compare( "" ) )	{
					// Attribute has a name
					// I'll declare it legal
					element->setAttribute( attribute, text );
					attribute = "";
				}
				break;
			}
			case TAG :	{
				if ( isStartTag() )	{
					if ( isHeadingTag() )	{
						headingTag( element );
						continue;
					}
					if ( !mTag.compare( "p" ) )	{
						pTag( element );
						continue;
					}
					if ( !mTag.compare( "pre" ) )	{
						preTag( element );
						continue;
					}
					if ( !mTag.compare( "hr" ) )	{
						hrTag( element );
						continue;
					}
/*
					if ( !mTag.compare( "blockquote" ) )	{
						blockquoteTag( element );
						continue;
					}
*/
					if ( isFontStyleTag() || isPhraseTag() )	{
						backPedal();
						pTag( element );
					}
					if ( !mTag.compare( "font" ) )	{
						backPedal();
						pTag( element );
						continue;
					}
					if ( isEmptyElementTag() )	{
						backPedal();
						pTag( element );
						continue;
					}
					if ( isBodyStyleTag() )	{
						bodyStyleTag( element );
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
				if ( text.compare( " " ) && text.compare( "" ) )	{
					cout << "Text is:" << endl << text << endl;
					TTextShared data = mDocument->createText( text );
					element->appendChild( data );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	pTag( TElementShared aParent )	{

	cout << "p tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "p" );
	aParent->appendChild( element );
	
	bool insideP = true;
	string text;
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
					if ( !mTag.compare( "p" ) )	{
						cout << "p closing tag found\n";
						insideP = false;
						continue;
					}
					if ( !mTag.compare( "body" ) )	{
						cout << "p closed implicitly\n";
						insideP = false;
						backPedal();
						continue;
					}
		
					cout << "Unknown closing tag found. Skipping...\n";
		
				}
				else	{
					if ( !mTag.compare( "p" ) )	{
						cout << "p closed implicitly\n";
						insideP = false;
						backPedal();
						continue;
					}
					if ( isFontStyleTag() || isPhraseTag() )	{
						textLevelTag( element );
						continue;
					}
					if ( !mTag.compare( "a" ) )	{
						aTag( element );
						continue;
					}
					if ( !mTag.compare( "font" ) )	{
						textLevelTag( element );
						continue;
					}
					if ( isEmptyElementTag() )	{
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

void HTMLParser	::	headingTag( TElementShared aParent )	{

	cout << mTag << " tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );
	
	bool insideHeading = true;
	
	string heading;
	
	while ( insideHeading )	{
		
		heading += getText( false );
		
		getTag();
		if ( !isStartTag() )	{
			if ( isHeadingTag() )	{
				cout << mTag << " closing tag found\n";
				insideHeading = false;
			}
			else	{
				cout << "Unknown closing tag found. Skipping...\n";
			}
		}
		else	{
			// Not a known tag
			cout << "Unknown tag found. Skipping...\n";
			skipTag();
		}
	}

	TTextShared content = mDocument->createText( heading );
	element->appendChild( content );

	cout << "Text is:" << endl << heading << endl;
	
}

void HTMLParser	::	preTag( TElementShared aParent )	{

	cout << "pre tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "pre" );
	aParent->appendChild( element );
	
	bool insidePre = true;
	
	string text;
	
	while ( insidePre )	{
		
		text += getText();
		
		getTag();
		if ( !isStartTag() )	{
			if ( !mTag.compare( "pre" ) )	{
				cout << "pre closing tag found\n";
				insidePre = false;
			}
			else	{
				cout << "Unknown closing tag found. Skipping...\n";
			}
		}
		else	{
			// Not a known tag
			cout << "Unknown tag found. Skipping...\n";
			skipTag();
		}
	}

	TTextShared content = mDocument->createText( text );
	element->appendChild( content );

	cout << "Text is:" << endl << text << endl;
	
}

void HTMLParser	::	hrTag( TElementShared aParent )	{

	cout << "hr tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "hr" );
	aParent->appendChild( element );
	
}

/*
void HTMLParser	::	blockquoteTag( TElementShared aParent )	{

	cout << "blockquote tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "blockquote" );
	aParent->appendChild( element );
	
	bool insideBlockquote = true;
	
	string attribute;
	
	while ( insideBlockquote )	{
		
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
					if ( !mTag.compare( "blockquote" ) )	{
						cout << " blockquote closing tag found\n";
						insideBlockquote = false;
						continue;
					}
		
					cout << "Unknown closing tag found. Skipping...\n";
		
				}
				else	{
					if ( !mTag.compare( "a" ) )	{
						aTag( element );
						continue;
					}
					if ( isFontStyleTag() || isPhraseTag() )	{
						textLevelTag( element );
						continue;
					}
					if ( !mTag.compare( "font" ) )	{
						textLevelTag( element );
						continue;
					}
					if ( isEmptyElementTag() )	{
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
*/

void HTMLParser	::	aTag( TElementShared aParent )	{

	cout << "a tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "a" );
	aParent->appendChild( element );
	
	bool insideA = true;
	
	string attribute;
	
	while ( insideA )	{
		
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
					if ( isEmptyElementTag() )	{
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

void HTMLParser	::	textLevelTag( TElementShared aParent )	{

	cout << mTag << " tag found\n";

	string tag = mTag; // To save the current tag info

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	aParent->appendChild( element );
	
	bool insideTextLevel = true;
	string text;
	string attribute;
	
	while ( insideTextLevel )	{
		
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
