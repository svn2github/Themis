/*	LoadTest
	Testing of how to load an HTML document
	Will end up in the trash can (probably)

	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	01-07-2002

*/

#include <fstream>
#include <string>
#include <stack>

#include "TDocument.h"
#include "TElement.h"
#include "TNode.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"

void showTree( const TNodeShared node, int spacing )	{
	
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

void showDocument( const TDocumentShared document )	{
	
	cout << "Document layout...\n";
	showTree( document, 1 );

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
	TNodeShared currentNode( document );
	
	unsigned int pos = 0;
	
	stack< string > tags;
	
	while ( pos != content.size() )	{
		if ( content[pos] == '<' )	{
			pos++;
			if ( content[pos] == '/' )	{
				// Closing tag
				pos++;
				string tag;
				while ( pos != content.size() && content[pos] != '>' )	{
					tag += content[pos];
					pos++;
				}
				if ( tags.top() == tag )	{
					// Tags match
					tags.pop();
					currentNode = currentNode->getParentNode();
				}
				else	{
					cerr << "Mismatch in closing tag\n";
					return 1;
				}
			}
			else	{
				// Opening tag
				string tag;
				while ( pos != content.size() && content[pos] != '/' &&
						  content[pos] != '>' && content[pos] != ' ' )	{
					tag += content[pos];
					pos++;
				}
				TElementShared element = document->createElement( tag );
				currentNode->appendChild( element );
				while ( pos != content.size() && content[pos] == ' ' )	{
					pos++;						
				}
				while ( content[pos] != '/' && content[pos] != '>' )	{
					// Element has attributes
					string attribute;
					string value;
					while ( pos != content.size() && content[pos] != ' '
							  && content[pos] != '/' && content[pos] != '>'
							  && content[pos] != '=' )	{
						attribute += content[pos];
						pos++;
					}
					if ( content[pos] == '=' )	{
						pos++;
						while ( pos != content.size() && content[pos] != ' '
								  && content[pos] != '/' && content[pos] != '>' )	{
							value += content[pos];
							pos++;
						}
					}
					while ( pos != content.size() && content[pos] == ' ' )	{
						pos++;						
					}
					element->setAttribute( attribute, value );
				}
				if ( content[pos] != '/' )	{
					// Tag doesn't close immediately
					tags.push( tag );
					currentNode = element;
				}
			}
		}
		
		pos++;
	}
	
	showDocument( document );
	
	return 0;
	
}
