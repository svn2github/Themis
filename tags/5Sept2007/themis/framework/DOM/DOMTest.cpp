/* DOMTest class
	See DOMTest.h for what it does
*/

#include "DOMTest.h"
#include "DOMSupport.h"
#include "TAttr.h"
#include "TNamedNodeMap.h"
#include "TElement.h"
#include "TNodeList.h"
#include "TText.h"
#include "TDocument.h"
#include "TDOMException.h"

void DOMTest	::	showDocument( const TDocumentPtr document )	{
	
	printf( "Root of the tree: " );
	showTree( document, 1 );
	
}

void DOMTest	::	showTree( const TNodePtr node, int spacing )	{
	
	printf( "%s\n", node->getNodeName().c_str() );
	TNodeListPtr children = node->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = 0; i < length; i++ )	{
			TNodePtr child = children->item( i );
			for ( int j = 0; j < spacing; j++ )	{
				printf( "  " );
			}
			printf( "Child name: " );
			showTree( child, spacing + 1 );
			if ( child->getNodeType() == ELEMENT_NODE )	{
				// Check for attributes
				TNamedNodeMapPtr attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodePtr attr = attributes->item( j );
					for ( int j = 0; j < spacing + 1; j++ )	{
						printf( "  " );
					}
					printf( "Attribute %s", attr->getNodeName().c_str() );
					printf( " with value %s\n", attr->getNodeValue().c_str() );
				}
			}
		}
	}
}

DOMTest	::	DOMTest()	:	BApplication( "application/x-vnd.Themis-DOMTest" )	{
	
	printf( "Let's kick off!\n" );

	TDocumentPtr document( new TDocument() );
	//document->setSmartPointer( document );
	
	printf( "Create an Element\n" );
	TElementPtr scooby = document->createElement( "scooby" );
		
	printf( "Attach element to document\n" );
	document->appendChild( scooby );

	printf( "Element created\n" );
	printf( "Try to append same node\n" );
	try	{
		scooby->appendChild( scooby );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}

	TElementPtr element = document->createElement( "cookie" );

	printf( "Try to append an element\n" );
	try	{
		scooby->appendChild( element );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}

	printf( "Tag name of the element: %s\n", element->getTagName().c_str() );
	
	printf( "Add an attribute to the element\n" );
	element->setAttribute( "me", "king" );
	
	printf( "Retrieve the attribute that was just created\n" );
	printf( "Attribute me has value: %s\n", element->getAttribute( "me" ).c_str() );

	TAttrPtr attr = shared_static_cast<TAttr> ( element->getAttributes()->getNamedItem( "me" ) );
	printf( "Attribute is attached to: %s\n", attr->getOwnerElement()->getNodeName().c_str() );

	printf( "Remove attribute me through NamedNodeMap\n" );
	element->getAttributes()->removeNamedItem( "me" );
	
	if ( element->getAttribute( "me" ) == "" )	{
		printf( "Removal succesfull\n" );
	}
	else	{
		printf( "Removal failed\n" );
	}

	printf( "Adding an attribute to the element for later use\n" );
	element->setAttribute( "piet", "joe" );

	printf( "Try to append parent to node\n" );
	try	{
		element->appendChild( scooby );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}

	printf( "Create a text node\n" );
	//DOMPointer<TText> textNode( new TText( "Text test" ) );
	TTextPtr textNode = document->createText( "This is a test" );
	printf( "Text node created\n" );
	printf( "Try to add an element to the text node\n" );
	try	{
		textNode->insertBefore( scooby, TNodePtr() );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}
	
	printf( "Get the name of the text node\n" );
	printf( "Name: %s\n", textNode->getNodeName().c_str() );

	printf( "Get a node\n" );
	TNodePtr newNode = scooby->getFirstChild();
	
	printf( "Test a node for sameness\n" );
	if ( element->isSameNode( newNode ) )	{
		printf( "Nodes are the same\n" );
	}
	else	{
		printf( "Nodes are different\n" );
	}

	printf( "Get type of node\n" );
	printf( "Type of node is: %s\n", element->getNodeTypeString() );

	printf( "Creating some elements...\n" );
	TElementPtr base = document->createElement( "base" );
	TElementPtr item1 = document->createElement( "item1" );
	TElementPtr item2 = document->createElement( "item2" );
	TElementPtr item3 = document->createElement( "item3" );
	TElementPtr item4 = document->createElement( "item4" );
	TElementPtr item5 = document->createElement( "item1" ); // Watch out: name is item1
	printf( "Create a tree with the elements...\n" );
	base->appendChild( item1 );
	base->appendChild( item2 );
	base->appendChild( item3 );
	item3->appendChild( item5 );
	item5->appendChild( item4 );

	printf( "Add base to document\n" );
	document->appendChild( base );

	printf( "Printing tree...\n" );
	showDocument( document );

	TNodeListPtr result = base->getElementsByTagName( "item1" );
	printf( "There are %i elements with tag name item1\n", (int) result->getLength() );
	printf( "Still here\n" );

	TNodeListPtr resultStar = base->getElementsByTagName( "*" );
	printf( "There are %i elements in base\n", (int) resultStar->getLength() );

	printf( "Removing an item with tag name item1...\n" );
	base->removeChild( item1 );
	item1.reset();

	printf( "There are %i elements with tag name item1\n", (int) result->getLength() );

	printf( "Removing a small piece of a tree...\n" );
	base->removeChild( item3 );
	
	resultStar = base->getElementsByTagName( "*" );
	printf( "There are %i elements in base\n", (int) resultStar->getLength() );

	TTextPtr text1 = document->createText( "bla " );
	TTextPtr text2 = document->createText( "blub " );
	base->appendChild( text1 );
	base->appendChild( text2 );
	
	TTextWeak textWeak = text1;
	
	printf( "Testing Text functions...\n" );
	printf( "Whole text of nodes\n%s\n", text1->getWholeText().c_str() );
	printf( "Replacing the whole text\n" );
	text1.reset();
	text2->replaceWholeText( "This works" );
	printf( "Text2 replaced\n" );
	printf( "Whole text replaced by: %s\n", text2->getWholeText().c_str() );
	if ( textWeak.expired() )	{
		printf( "Node removed as it should be\n" );
	}
	else	{
		printf( "Erm, this shouldn't happen\n" );
	}

	printf( "Printing what's left of the tree...\n" );
	showDocument( document );
	printf( "Printing a deep clone of the tree\n" );
	TNodePtr deepClone = document->cloneNode( true );
	showTree( deepClone, 1 );
	printf( "Printing a shallow clone of the tree\n" );
	TNodePtr shallowClone = document->cloneNode( false );
	showTree( shallowClone, 1 );

	printf( "Cleaning up...\n" );

	// Clean up and exit
	scooby.reset();
	element.reset();
	
	printf( "Exiting...\n" );
	be_app->PostMessage( B_QUIT_REQUESTED );
	
}

int main()	{
	
	DOMTest myApp;
	myApp.Run();
	
	return 0;
	
}
