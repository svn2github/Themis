/* DOMTest class
	See DOMTest.h for what it does
*/

#include "DOMTest.h"
#include "TAttr.h"
#include "TNamedNodeMap.h"

DOMTest	::	DOMTest()	:	BApplication( "application/x-vnd.Themis-DOMTest" )	{
	
	printf( "Let's kick off!\n" );

	printf( "Create a Node\n" );
	TNode * node = new TNode( ELEMENT_NODE );
	printf( "Node created\n" );
	printf( "Try to append same node\n" );
	try	{
		node->appendChild( node );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}
	
	TElement * element = new TElement( "cookie" );

	printf( "Try to append an element\n" );
	try	{
		node->appendChild( element );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}

	printf( "Tag name of the element: %s\n", element->getTagName().String() );
	
	printf( "Add an attribute to the element\n" );
	element->setAttribute( "me", "king" );
	
	printf( "Retrieve the attribute that was just created\n" );
	printf( "Attribute me has value: %s\n", element->getAttribute( "me" ).String() );

	TAttr * attr = (TAttr *) element->getAttributes()->getNamedItem( "me" );
	printf( "Attribute is attached to: %s\n", attr->getOwnerElement()->getNodeName()->String() );

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
	TNode * textNode = new TNode( TEXT_NODE );
	printf( "Node created\n" );
	printf( "Try to add a node to the text node\n" );
	try	{
		textNode->insertBefore( node, NULL );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}
	
	printf( "Get the name of the text node\n" );
	printf( "Name: %s\n", textNode->getNodeName()->String() );

	printf( "Get a node\n" );
	const TNode * newNode = node->getFirstChild();
	
	printf( "Test a node for sameness\n" );
	if ( element->isSameNode( newNode ) )	{
		printf( "Nodes are the same\n" );
	}
	else	{
		printf( "Nodes are different\n" );
	}
	
	printf( "Get type of node\n" );
	printf( "Type of node is: %s\n", element->getNodeTypeString() );
	
	printf( "Cleaning up...\n" );

	// Clean up and exit
	delete node;
	delete textNode;
	printf( "Exiting...\n" );
	be_app->PostMessage( B_QUIT_REQUESTED );
	
}

int main()	{
	
	DOMTest myApp;
	myApp.Run();
	
	return 0;
	
}
