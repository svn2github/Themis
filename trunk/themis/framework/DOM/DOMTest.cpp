/* DOMTest class
	See DOMTest.h for what it does
*/

#include "DOMTest.h"

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
	
	TNode * otherNode = new TNode( ELEMENT_NODE );

	printf( "Try to append another node\n" );
	try	{
		node->appendChild( otherNode );
	}
	catch ( TDOMException e )	{
		printf( "Exception caught : %s\n", e.getString() );
	}

	printf( "Try to append parent to node\n" );
	try	{
		otherNode->appendChild( node );
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
	if ( otherNode->isSameNode( newNode ) )	{
		printf( "Nodes are the same\n" );
	}
	else	{
		printf( "Nodes are different\n" );
	}
	
	printf( "Get type of node\n" );
	printf( "Type of node is: %s\n", otherNode->getNodeTypeString() );
	
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
