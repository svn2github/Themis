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
//#include "TDocument.h"

DOMTest	::	DOMTest()	:	BApplication( "application/x-vnd.Themis-DOMTest" )	{
	
	printf( "Let's kick off!\n" );

/*
	TDocumentShared document( new TDocument() );
	document->setSmartPointer( document );
	
	TElementShared creationTest = document->createElement( "nameOfTag" );
*/
	printf( "Create a Node\n" );
	//DOMPointer<TNode> node( new TNode( ELEMENT_NODE ) );
	TNodeShared node( new TNode( ELEMENT_NODE ) );
	node->setSmartPointer( node );
	
	printf( "Node created\n" );
	printf( "Try to append same node\n" );
	try	{
		node->appendChild( node );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}

	//DOMPointer<TElement> element( new TElement( "cookie" ) );
	TElementShared element( new TElement( "cookie" ) );
	element->setSmartPointer( element );

	printf( "Try to append an element\n" );
	try	{
		node->appendChild( element );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}

	printf( "Tag name of the element: %s\n", element->getTagName().c_str() );
	
	printf( "Add an attribute to the element\n" );
	element->setAttribute( "me", "king" );
	
	printf( "Retrieve the attribute that was just created\n" );
	printf( "Attribute me has value: %s\n", element->getAttribute( "me" ).c_str() );

	TAttrShared attr = shared_static_cast<TAttr> ( make_shared( element->getAttributes()->getNamedItem( "me" ) ) );
	printf( "Attribute is attached to: %s\n", attr->getOwnerElement().get()->getNodeName().c_str() );

	printf( "Remove attribute me through NamedNodeMap\n" );
	element->getAttributes()->removeNamedItem( "me" );
	
	if ( element->getAttribute( "me" ) == "" )	{
		printf( "Removal succesfull\n" );
	}
	else	{
		printf( "Removal failed\n" );
	}

	printf( "Try to append parent to node\n" );
	try	{
		element->appendChild( node );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}

	printf( "Create a text node\n" );
	//DOMPointer<TText> textNode( new TText( "Text test" ) );
	TNodeShared textNode( new TNode( TEXT_NODE ) );
	textNode->setSmartPointer( textNode );
	printf( "Node created\n" );
	printf( "Try to add a node to the text node\n" );
	try	{
		textNode->insertBefore( node, TNodeShared() );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}
	
	printf( "Get the name of the text node\n" );
	printf( "Name: %s\n", textNode->getNodeName().c_str() );

	printf( "Get a node\n" );
	const TNodeWeak newNode = node->getFirstChild();
	
	printf( "Test a node for sameness\n" );
	if ( element->isSameNode( make_shared( newNode ) ) )	{
		printf( "Nodes are the same\n" );
	}
	else	{
		printf( "Nodes are different\n" );
	}

	printf( "Get type of node\n" );
	printf( "Type of node is: %s\n", element->getNodeTypeString() );

	printf( "Creating some elements...\n" );
	//DOMPointer<TElement> base( new TElement( "base" ) );
	TElementShared base( new TElement( "base" ) );
	base->setSmartPointer( base );
	//DOMPointer<TElement> item1( new TElement( "item1" ) );
	TElementShared item1( new TElement( "item1" ) );
	item1->setSmartPointer( item1 );
	//DOMPointer<TElement> item2( new TElement( "item2" ) );
	TElementShared item2( new TElement( "item2" ) );
	item2->setSmartPointer( item2 );
	//DOMPointer<TElement> item3( new TElement( "item3" ) );
	TElementShared item3( new TElement( "item3" ) );
	item3->setSmartPointer( item3 );
	//DOMPointer<TElement> item4( new TElement( "item4" ) );
	TElementShared item4( new TElement( "item4" ) );
	item4->setSmartPointer( item4 );
	//DOMPointer<TElement> item5( new TElement( "item1" ) );
	TElementShared item5( new TElement( "item1" ) );
	item5->setSmartPointer( item5 );
	printf( "Create a tree with the elements...\n" );
	base->appendChild( item1 );
	base->appendChild( item2 );
	base->appendChild( item3 );
	item3->appendChild( item5 );
	item5->appendChild( item4 );

	TNodeListShared result = base->getElementsByTagName( "item1" );
	printf( "There are %i elements with tag name item1\n", (int) result->getLength() );
	printf( "Still here\n" );

	TNodeListShared resultStar = base->getElementsByTagName( "*" );
	printf( "There are %i elements in base\n", (int) resultStar->getLength() );

	printf( "Removing an item with tag name item1...\n" );
	base->removeChild( item1 );
	item1.reset();

	printf( "There are %i elements with tag name item1\n", (int) result->getLength() );

	printf( "Removing a small piece of a tree...\n" );
	base->removeChild( item3 );
	
	resultStar = base->getElementsByTagName( "*" );
	printf( "There are %i elements in base\n", (int) resultStar->getLength() );

	//DOMPointer<TText> text1( new TText( "bla " ) );
	TTextShared text1( new TText( "bla " ) );
	text1->setSmartPointer( text1 );
	//DOMPointer<TText> text2( new TText( "bla " ) );
	TTextShared text2( new TText( "blub" ) );
	text2->setSmartPointer( text2 );
	base->appendChild( text1 );
	base->appendChild( text2 );
	
	TTextWeak textWeak = text1;
	
	//text2.reset();
	
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

	printf( "Cleaning up...\n" );

	// Clean up and exit
	node.reset();
	element.reset();
	
	printf( "Exiting...\n" );
	be_app->PostMessage( B_QUIT_REQUESTED );
	
}

int main()	{
	
	DOMTest myApp;
	myApp.Run();
	
	return 0;
	
}
