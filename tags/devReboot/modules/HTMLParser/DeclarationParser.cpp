/*	DeclarationParser implementation
	See DeclarationParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "DeclarationParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"
#include "TSchema.hpp"

// DOM headers
#include "TElement.h"
#include "TNodeList.h"

DeclarationParser	::	DeclarationParser(SGMLTextPtr aDocText,
										  TSchemaPtr aSchema )
					:	BaseParser( aSchema )	{

	//printf( "Constructing DeclarationParser\n" );
	
	setDocText( aDocText );

}

DeclarationParser	::	~DeclarationParser()	{

	//printf( "Destroying DeclarationParser\n" );
	
}

void DeclarationParser	::	setDocText( SGMLTextPtr aDocText )	{
	
	mDocText = aDocText;
	
}

bool DeclarationParser	::	parse()	{
	
	return processDeclaration();
	
}

bool DeclarationParser	::	processDeclaration()	{
	
	return false;
	
}

TElementPtr DeclarationParser	::	processConnector()	{

	if ( process( mAnd, false ) )	{
		return mSchema->createElement( mAnd );
	}
	if ( process( mOr, false ) )	{
		return mSchema->createElement( mOr );
	}
	if ( process( mSeq, false ) )	{
		return mSchema->createElement( mSeq );
	}

	return TElementPtr();

}

string DeclarationParser	::	processNameTokenGroup()	{

	if ( ! process( mGrpo, false ) )	{
		return "";
	}
	
	processTsStar();
	processNameToken();
	
	bool inGroup = true;
	while ( inGroup )	{
		processTsStar();
		TElementPtr connector = processConnector();
		if ( connector.get() == NULL )	{
			inGroup = false;
			continue;
		}
		processTsStar();
		if ( processNameToken() == "" )	{
			inGroup = false;
		}
	}
	
	processTsStar();
	if ( ! process( mGrpc, false ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Closing of name token group expected",
										GENERIC, true );
	}

	return "nameTokenGroup";
	
}

TElementPtr DeclarationParser	::	processNameGroup()	{

	if ( ! process( mGrpo, false ) )	{
		return TElementPtr();
	}

	processTsStar();
	
	string name = processName( false );
	if ( name == "" )	{
		return TElementPtr();
	}
	
	TElementPtr group = mSchema->createElement( "elements" );
	TElementPtr element = mSchema->createElement( name );
	group->appendChild( element );

	//TElementPtr subGroup;
	bool inGroup = true;
	while ( inGroup )	{
		processTsStar();
		TElementPtr connector = processConnector();
		if ( connector.get() == NULL )	{
			inGroup = false;
			continue;
		}
		processTsStar();
		name = processName( false );
		if ( name == "" )	{
			inGroup = false;
		}
		else	{
			element = mSchema->createElement( name );
			group->appendChild( element );
		}
	}

	processTsStar();
	if ( ! process( mGrpc, false ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Closing of name group expected",
										GENERIC, true );
	}

	return group;
	
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
	
	processPsPlus( false );
	processSystemId();
	
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
