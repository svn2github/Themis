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

bool DeclarationParser	::	parse( const map<string, Position> & aEntityTexts )	{

	mEntityTexts = aEntityTexts;

	return processDeclaration();
	
}

bool DeclarationParser	::	parse()	{
	
	return processDeclaration();
	
}

bool DeclarationParser	::	processDeclaration()	{
	
	return false;
	
}

bool DeclarationParser	::	processExtEntitySpec( TElementPtr & entity )	{

	return processExternalId( entity );
	
}

bool DeclarationParser	::	processExternalId( TElementPtr & entity )	{

	if ( process( kSYSTEM, false ) )	{
		entity->setAttribute( "type", kSYSTEM );
	}
	else	{
		// Not a system identifier. Must be public.
		if ( process( kPUBLIC, false ) )	{
			entity->setAttribute( "type", kPUBLIC );
			processPsPlus();
			string text = processPublicId();
			entity->setAttribute( "text", text );
		}
		else	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"PUBLIC or SYSTEM identifier expected",
											GENERIC, true );
		}
	}
	
	try	{
		processPsPlus();
		processSystemId();
	}
	catch( ReadException r )	{
		// Do nothing
	}
	
	return true;
	
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
