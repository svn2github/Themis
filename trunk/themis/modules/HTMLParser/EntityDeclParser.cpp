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
													  TDocumentShared aDTD,
													  TElementShared aParEntities,
													  TElementShared aCharEntities )
								:	DeclarationParser( aDocText, aDTD, aParEntities, aCharEntities )	{

	//printf( "Constructing EntityDeclParser\n" );
	
}

EntityDeclParser	::	~EntityDeclParser()	{

	//printf( "Destroying EntityDeclParser\n" );
	
}

map<string, Position> EntityDeclParser	::	getEntityTexts()	{
	
	return mEntityTexts;
	
}

void EntityDeclParser	::	processDeclaration()	{

	// Create an element to store the entity
	TElementShared entity;

	process( mMdo );
	process( kENTITY );

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		entity = processEntityName();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}		

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	try	{
		processEntityText( entity );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}		

	try	{
		processPsStar();
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}
	
	try	{
		process( mMdc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
//	printf( "Entity found: %s with text: %s\n", entity->getNodeName().c_str(),
//			  entity->getAttribute( "text" ).c_str() );

}

TElementShared EntityDeclParser	::	processEntityName()	{
	
	try	{
		string name = processGenEntityName();
		TElementShared entity = mDTD->createElement( name );
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
		TElementShared entity = mDTD->createElement( name );
		mParEntities->appendChild( entity );
		return entity;
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

}

void EntityDeclParser	::	processEntityText( TElementShared & entity )	{

	try	{
		processParLiteral( entity );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		processDataText( entity );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	try	{
		processExtEntitySpec( entity );
		return;
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

}

void EntityDeclParser	::	processDataText( TElementShared & entity )	{

	try	{
		process( kCDATA );
		entity->setAttribute( "type", kCDATA );
	}
	catch( ReadException r )	{
		// Not CDATA. Try SDATA
		try	{
			process( kSDATA );
			entity->setAttribute( "type", kSDATA );
		}
		catch( ReadException r )	{
			// Not SDATA. Must be PI
			process( kPI );
			entity->setAttribute( "type", kPI );
		}
	}

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		processParLiteral( entity );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

}


string EntityDeclParser	::	processGenEntityName()	{

	return processName();
	
}

string EntityDeclParser	::	processParEntityName()	{

	process( mPero );

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		return processName();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
}
