/*	LoadTest
	Testing of how to load an HTML document
	Will end up in the trash can (probably)

	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	01-07-2002

*/

#include <fstream>
#include <string>
#include <algorithm>

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

void HTMLParser	::	getNextTag()	{
	
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
		mTag += mContent[mPos];
		mPos++;
	}
	
}

void HTMLParser	::	setContent( string aContent )	{
	
	mContent = aContent;
	
}

void HTMLParser	::	startParsing( TDocumentShared document )	{
	
	mDocument = document;
	
	getNextTag();
	
	doctypeTag();
	htmlTag();

}

bool HTMLParser	::	isStartTag()	{
	
	return !mCloseTag;

}

string HTMLParser	::	getText( bool conserveSpaces = true )	{
	
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

void HTMLParser	::	doctypeTag()	{
	
	// I don't really care about this tag for now
	// but I'll just check to see if it's there
	if ( !mTag.compare( "!DOCTYPE" ) )	{
		// We've got a doctype tag
		cout << "doctype tag found\n";
	
		// Get the next tag to recognize
		getNextTag();

	}

}

void HTMLParser	::	htmlTag()	{

	if ( !mTag.compare( "html" ) )	{
		// Found html tag
		cout << "html tag found\n";

		// Add to DOM tree
		TElementShared element = mDocument->createElement( "html" );
		mDocument->appendChild( element );
		
		// Get the next tag to recognize
		getNextTag();
		
		headTag( element );
		bodyTag( element );
		
		bool insideHtml = true;
		
		while ( insideHtml )	{
			if ( !isStartTag() )	{
				if ( !mTag.compare( "html" ) )	{
					cout << "html closing tag found\n";

					insideHtml = false;
										
					// We're done. Nothing comes after this.
				}
			}
		}
	}	
	
}

void HTMLParser	::	headTag( TElementShared parent )	{

	if ( !mTag.compare( "head" ) )	{
		// Found head tag
		cout << "head tag found\n";
		
		// Add to parent
		TElementShared element = mDocument->createElement( "head" );
		parent->appendChild( element );
		
		// Get the next tag to recognize
		getNextTag();
		
		bool insideHead = true;
		
		while ( insideHead )	{
			if ( isStartTag() )	{
				if ( !mTag.compare( "title" ) )	{
					titleTag( element );
				}
				//styleTag();
				//scriptTag();
				//isindexTag();
				//baseTag();
				//metaTag();
				//linkTag();
				else	{
					// Not a known tag
					cout << "unknown tag found!!!\n";
					
					// Ignore it and go to the next one
					getNextTag();
				}
			}
			else	{			
				if ( !mTag.compare( "head" ) )	{
					cout << "head closing tag found\n";
	
					// End the while loop
					insideHead = false;
					getNextTag();
						
				}
				else	{
					// Invalid end tag
					// Ignore it and go to the next one
					getNextTag();
				}
			}
		}
	}
	
}

void HTMLParser	::	titleTag( TElementShared parent )	{

	// Name is already recognized in headTag
	cout << "title tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "title" );
	parent->appendChild( element );

	bool insideTitle = true;
	
	string title;
	
	while ( insideTitle )	{
		
		title += getText();
		
		getNextTag();
		if ( !isStartTag() )	{
			if ( !mTag.compare( "title" ) )	{
				cout << "title closing tag found\n";
				insideTitle = false;
				getNextTag();
			}
		}
	}

	cout << "Title is: " << title << endl;
	
}

void HTMLParser	::	bodyTag( TElementShared parent )	{
	
	if ( !mTag.compare( "body" ) )	{
		// Found body tag
		cout << "body tag found\n";

		// Add to parent
		TElementShared element = mDocument->createElement( "body" );
		parent->appendChild( element );

		// Ignoring the attributes for now
		
		// Get the next tag to recognize
		getNextTag();
		
		bool insideBody = true;
		
		while ( insideBody )	{
			if ( isStartTag() )	{
				if ( !mTag.compare( "p" ) )	{
					pTag( element );
				}
				else	{
					if ( !mTag.compare( "h1" ) ||
						 !mTag.compare( "h2" ) ||
						 !mTag.compare( "h3" ) ||
						 !mTag.compare( "h4" ) ||
						 !mTag.compare( "h5" ) ||
						 !mTag.compare( "h6" ) )	{
						 	headingTag( element );
					}
					else	{
						if ( !mTag.compare( "pre" ) )	{
							preTag( element );
						}
						else	{
							if ( !mTag.compare( "hr" ) )	{
								hrTag( element );
							}
							else	{
								if ( !mTag.compare( "blockquote" ) )	{
									blockquoteTag( element );
								}
								else	{
									// Not a known tag
									cout << "unknown tag found!!!\n";
									
									// Ignore it and go to the next one
									getNextTag();
								}
							}
						}
					}
				}
			}
			else	{			
				if ( !mTag.compare( "body" ) )	{
					cout << "body closing tag found\n";
	
					// End the while loop
					insideBody = false;
					getNextTag();	
				}
				else	{
					// Invalid end tag
					// Ignore it and go to the next one
					getNextTag();
				}
			}
		}
	}
	
}

void HTMLParser	::	pTag( TElementShared parent )	{

	// Name is already recognized in parent tag
	cout << "p tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "p" );
	parent->appendChild( element );
	
	bool insideP = true;
	
	string text;
	
	while ( insideP )	{
		
		text += getText( false );
		
		getNextTag();
		if ( !isStartTag() )	{
			if ( !mTag.compare( "p" ) )	{
				cout << "p closing tag found\n";
				insideP = false;
				getNextTag();
			}
		}
	}

	cout << "Text is:" << endl << text << endl;
	
}

void HTMLParser	::	headingTag( TElementShared parent )	{

	// Name is already recognized in parent tag
	cout << mTag << " tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( mTag );
	parent->appendChild( element );
	
	bool insideHeading = true;
	
	string heading;
	
	while ( insideHeading )	{
		
		heading += getText( false );
		
		getNextTag();
		if ( !isStartTag() )	{
			if ( !mTag.compare( "h1" ) ||
				 !mTag.compare( "h2" ) ||
				 !mTag.compare( "h3" ) ||
				 !mTag.compare( "h4" ) ||
				 !mTag.compare( "h5" ) ||
				 !mTag.compare( "h6" ) )	{
				cout << mTag << " closing tag found\n";
				insideHeading = false;
				getNextTag();
			}
		}
	}

	cout << "Text is:" << endl << heading << endl;
	
}

void HTMLParser	::	preTag( TElementShared parent )	{

	// Name is already recognized in parent tag
	cout << "pre tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "pre" );
	parent->appendChild( element );
	
	bool insidePre = true;
	
	string text;
	
	while ( insidePre )	{
		
		text += getText();
		
		getNextTag();
		if ( !isStartTag() )	{
			if ( !mTag.compare( "pre" ) )	{
				cout << "pre closing tag found\n";
				insidePre = false;
				getNextTag();
			}
		}
	}

	cout << "Text is:" << endl << text << endl;
	
}

void HTMLParser	::	hrTag( TElementShared parent )	{

	// Name is already recognized in parent tag
	cout << "hr tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "hr" );
	parent->appendChild( element );
	
	getNextTag();
	
}

void HTMLParser	::	blockquoteTag( TElementShared parent )	{

	// Name is already recognized in parent tag
	cout << "blockquote tag found\n";

	// Add to parent
	TElementShared element = mDocument->createElement( "blockquote" );
	parent->appendChild( element );
	
	bool insideBlockquote = true;
	
	string text;
	
	while ( insideBlockquote )	{
		
		text += getText( false );
		
		getNextTag();
		if ( !isStartTag() )	{
			if ( !mTag.compare( "blockquote" ) )	{
				cout << "blockquote closing tag found\n";
				insideBlockquote = false;
				getNextTag();
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
