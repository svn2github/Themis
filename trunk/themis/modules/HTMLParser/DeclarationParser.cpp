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

DeclarationParser	::	DeclarationParser( SGMLTextPtr aDocText,
															TDocumentShared aDTD,
															TElementShared aParEntities,
															TElementShared aCharEntities )
							:	BaseParser()	{

	//printf( "Constructing DeclarationParser\n" );
	
	mDocText = aDocText;
	mDTD = aDTD;
	mParEntities = aParEntities;
	mCharEntities = aCharEntities;
	
}

DeclarationParser	::	~DeclarationParser()	{

	//printf( "Destroying DeclarationParser\n" );
	
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

void DeclarationParser	::	processExtEntitySpec( TElementShared & entity )	{

	processExternalId( entity );
	
}

void DeclarationParser	::	processExternalId( TElementShared & entity )	{

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
