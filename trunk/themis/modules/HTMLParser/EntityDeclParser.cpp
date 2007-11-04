/*	EntityDeclParser implementation
	See EntityDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "EntityDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"
#include "TSchema.hpp"

// DOM headers
#include "TElement.h"

EntityDeclParser	::	EntityDeclParser( SGMLTextPtr aDocText,
													  TSchemaPtr aDTD )
								:	DeclarationParser( aDocText, aDTD )	{

}

EntityDeclParser	::	~EntityDeclParser()	{

}

map<string, Position> EntityDeclParser	::	getEntityTexts()	{
	
	return mEntityTexts;
	
}

bool EntityDeclParser	::	processDeclaration()	{

	// Create an element to store the entity
	TElementPtr entity;

	//process( mMdo );
	if ( ! process( kENTITY, false ) )	{
		return false;
	}

	processPsPlus();
	entity = processEntityName();
	processPsPlus();
	processEntityText( entity );
	processPsStar();
	if ( ! process( mMdc, false ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Entity declaration not closed correctly",
										GENERIC, true );
	}		
	
	return true;

}

TElementPtr EntityDeclParser	::	processEntityName()	{
	
	TElementPtr entity;
	
	string name = processGenEntityName();
	if ( name == "" )	{
		name = processParEntityName();
		if ( name == "" )	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"Entity name expected",
											GENERIC, true );
		}
		else	{
			entity = mSchema->createElement( name );
			mParEntities->appendChild( entity );
		}
	}
	else	{	
		entity = mSchema->createElement( name );
		mCharEntities->appendChild( entity );
	}

	return entity;

}

void EntityDeclParser	::	processEntityText( TElementPtr & entity )	{

	if ( processParLiteral( entity ) )	{
		return;
	}
	if ( processDataText( entity ) )	{
		return;
	}
	if ( processExtEntitySpec( entity ) )	{
		return;
	}

}

bool EntityDeclParser	::	processDataText( TElementPtr & entity )	{

	if ( process( kCDATA, false ) )	{
		entity->setAttribute( "type", kCDATA );
	}
	else	{
		// Not CDATA. Try SDATA
		if ( process( kSDATA, false ) )	{
			entity->setAttribute( "type", kSDATA );
		}
		else	{
			// Not SDATA. Must be PI
			if ( process( kPI, false ) )	{
				entity->setAttribute( "type", kPI );
			}
			else	{
				return false;
			}
		}
	}

	processPsPlus();
	if ( ! processParLiteral( entity ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Parameter literal expected",
										GENERIC, true );
	}

	return true;

}


string EntityDeclParser	::	processGenEntityName()	{

	return processName( false );
	
}

string EntityDeclParser	::	processParEntityName()	{

	if ( ! process( mPero, false ) )	{
		return "";
	}

	processPsPlus();
	string name = processName( false );
	if ( name == "" )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Parameter entity name expected",
										GENERIC, true );
	}
	
	return name;		
	
}
