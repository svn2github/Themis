/*	DeclarationParser implementation
	See DeclarationParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"

// DOM headers
#include "TElement.h"
#include "TNodeList.h"

DeclarationParser	::	DeclarationParser( SGMLTextPtr aDocText,
															TDocumentPtr aDTD )
							:	BaseParser()	{

	//printf( "Constructing DeclarationParser\n" );
	
	setDocText( aDocText );

	setDTD( aDTD );	
}

DeclarationParser	::	~DeclarationParser()	{

	//printf( "Destroying DeclarationParser\n" );
	
}

void DeclarationParser	::	setDocText( SGMLTextPtr aDocText )	{
	
	mDocText = aDocText;
	
}

void DeclarationParser	::	setDTD( TDocumentPtr aDTD )	{

	mDTD = aDTD;
	
	TNodeListPtr list = mDTD->getChildNodes();
	unsigned int length = list->getLength();
	for ( unsigned int i = 0; i < length; i++ )	{
		TNodePtr node = list->item( i );
		TElementPtr element = shared_static_cast<TElement>( node );
		if ( element->getNodeName() == "parEntities" )	{
			mParEntities = element;
		}
		if ( element->getNodeName() == "charEntities" )	{
			mCharEntities = element;
		}
	}
	
	mEntityTexts.clear();
	
}

void DeclarationParser	::	parse( const map<string, Position> & aEntityTexts )	{

	mEntityTexts = aEntityTexts;

	processDeclaration();
	
}

void DeclarationParser	::	parse()	{
	
	processDeclaration();
	
}

void DeclarationParser	::	processDeclaration()	{
	
}

void DeclarationParser	::	processExtEntitySpec( TElementPtr & entity )	{

	processExternalId( entity );
	
}

void DeclarationParser	::	processExternalId( TElementPtr & entity )	{

	try	{
		process( kSYSTEM );
		entity->setAttribute( "type", kSYSTEM );
	}
	catch( ReadException r )	{
		// Not a system identifier. Must be public.
		try	{
			process( kPUBLIC );
			entity->setAttribute( "type", kPUBLIC );
			processPsPlus();
			string text = processPublicId();
			entity->setAttribute( "text", text );
		}
		catch( ReadException r )	{
			r.setFatal();
			throw r;
		}
	}
	
	try	{
		processPsPlus();
		processSystemId();
	}
	catch( ReadException r )	{
		// Do nothing
	}
	
}

string DeclarationParser	::	processPublicId()	{
	
	string text = "";

	text = processMinLiteral();

	return text;
	
}

string DeclarationParser	::	processSystemId()	{
	
	string text = "";
	
	text = processMinLiteral();
	
	return text;
	
}
