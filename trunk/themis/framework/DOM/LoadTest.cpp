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

HTMLParser	::	HTMLParser()	{

	mPos = 0;
	mTag = "";
	mContent = "";
	mCloseTag = false;
	
}

HTMLParser	::	~HTMLParser()	{
	
}

void HTMLParser	::	showTree( const TNodeShared node, int spacing )	{
	
	TNodeListShared children = node->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = 0; i < length; i++ )	{
			TNodeShared child = make_shared( children->item( i ) );
			for ( int j = 0; j < spacing; j++ )	{
				cout << "  ";
			}
			cout << "Child name: " << child->getNodeName().c_str() << endl;
			if ( child->getNodeType() == ELEMENT_NODE )	{
				// Check for attributes
				TNamedNodeMapShared attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodeWeak attr = attributes->item( j );
					TNodeShared tempAttr = make_shared( attr );
					for ( int j = 0; j < spacing + 1; j++ )	{
						cout << "  ";
					}
					cout << "Attribute " << tempAttr->getNodeName();
					cout << " with value " << tempAttr->getNodeValue() << endl;
				}
			}
			showTree( child, spacing + 1 );
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

void HTMLParser	::	setContent( string aContent )	{
	
	mContent = aContent;
	
}

bool HTMLParser	::	isStartTag()	{
	
	return !mCloseTag;

}

string HTMLParser	::	getText( bool conserveSpaces = true )	{
	
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
		if ( !conserveSpaces )	{
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
		result+= mContent[mPos];
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

void HTMLParser	::	startParsing( TDocumentShared document )	{
	
	mDocument = document;
	
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
				bodyTag( element );
				continue;
			}

			cout << "Unknown tag found. Skipping...\n";

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

void HTMLParser	::	headTag( TElementShared parent )	{

	cout << "head tag found\n";
	
	// Add to parent
	TElementShared element = mDocument->createElement( "head" );
	parent->appendChild( element );
	
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

void HTMLParser	::	titleTag( TElementShared parent )	{

	cout << "title tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "title" );
	parent->appendChild( element );

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
		}
	}

	cout << "Title is: " << title << endl;
	
}

void HTMLParser	::	bodyTag( TElementShared parent )	{
	
	cout << "body tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "body" );
	parent->appendChild( element );

	bool insideBody = true;
	string attribute;
	
	while ( insideBody )	{
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
					if ( !mTag.compare( "p" ) )	{
						pTag( element );
						continue;
					}
					if ( !mTag.compare( "h1" ) ||
						 !mTag.compare( "h2" ) ||
						 !mTag.compare( "h3" ) ||
						 !mTag.compare( "h4" ) ||
						 !mTag.compare( "h5" ) ||
						 !mTag.compare( "h6" ) )	{
						 	headingTag( element );
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
					if ( !mTag.compare( "blockquote" ) )	{
						blockquoteTag( element );
						continue;
					}
		
					// Not a known tag
					cout << "Unknown tag found. Skipping...\n";
					
				}
				else	{			
					if ( !mTag.compare( "body" ) )	{
						cout << "body closing tag found\n";
		
						// End the while loop
						insideBody = false;
					}
					else	{
						cout << "Unknown closing tag found. Skipping...\n";
					}
				}
				break;
			}
			case TEXT :	{
				// Let's see this as a paragraph
				if ( text.compare( " " ) && text.compare( "" ) )	{
					backPedal();
					pTag( element );
				}
				break;
			}
		}
	}
	
}

void HTMLParser	::	pTag( TElementShared parent )	{

	cout << "p tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "p" );
	parent->appendChild( element );
	
	bool insideP = true;
	string text;
	
	while ( insideP )	{
		
		text += getText( false );
		
		getTag();
		
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
			}
		}
	}

	cout << "Text is:" << endl << text << endl;
	
}

void HTMLParser	::	headingTag( TElementShared parent )	{

	cout << mTag << " tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	parent->appendChild( element );
	
	bool insideHeading = true;
	
	string heading;
	
	while ( insideHeading )	{
		
		heading += getText( false );
		
		getTag();
		if ( !isStartTag() )	{
			if ( !mTag.compare( "h1" ) ||
				 !mTag.compare( "h2" ) ||
				 !mTag.compare( "h3" ) ||
				 !mTag.compare( "h4" ) ||
				 !mTag.compare( "h5" ) ||
				 !mTag.compare( "h6" ) )	{
				cout << mTag << " closing tag found\n";
				insideHeading = false;
			}
			else	{
				cout << "Unknown closing tag found. Skipping...\n";
			}
		}
	}

	cout << "Text is:" << endl << heading << endl;
	
}

void HTMLParser	::	preTag( TElementShared parent )	{

	cout << "pre tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "pre" );
	parent->appendChild( element );
	
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
	}

	cout << "Text is:" << endl << text << endl;
	
}

void HTMLParser	::	hrTag( TElementShared parent )	{

	cout << "hr tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "hr" );
	parent->appendChild( element );
	
}

void HTMLParser	::	blockquoteTag( TElementShared parent )	{

	cout << "blockquote tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "blockquote" );
	parent->appendChild( element );
	
	bool insideBlockquote = true;
	
	string text;
	
	while ( insideBlockquote )	{
		
		text += getText( false );
		
		getTag();
		if ( !isStartTag() )	{
			if ( !mTag.compare( "blockquote" ) )	{
				cout << "blockquote closing tag found\n";
				insideBlockquote = false;
			}
			else	{
				cout << "Unknown closing tag found. Skipping...\n";
			}
		}
	}

	cout << "Text is:" << endl << text << endl;
	
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
