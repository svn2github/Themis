/*	EntityDeclParser implementation
	See EntityDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "EntityDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"

// DOM headers
#include "TDocument.h"
#include "TElement.h"

EntityDeclParser	::	EntityDeclParser( SGMLTextPtr aDocText,
													  TDocumentPtr aDTD )
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

	try	{
		processPsPlus();
		entity = processEntityName();
		processPsPlus();
		processEntityText( entity );
		processPsStar();
		process( mMdc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	return true;

}

TElementPtr EntityDeclParser	::	processEntityName()	{
	
	try	{
		string name = processGenEntityName();
		TElementPtr entity = mDTD->createElement( name );
		mCharEntities->appendChild( entity );
		return entity;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}
	try	{
		string name = processParEntityName();
		TElementPtr entity = mDTD->createElement( name );
		mParEntities->appendChild( entity );
		return entity;
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

}

void EntityDeclParser	::	processEntityText( TElementPtr & entity )	{

	try	{
		processParLiteral( entity );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
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

	try	{
		processPsPlus();
		processParLiteral( entity );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	return true;

}


string EntityDeclParser	::	processGenEntityName()	{

	return processName();
	
}

string EntityDeclParser	::	processParEntityName()	{

	process( mPero );

	try	{
		processPsPlus();
		return processName();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
}
