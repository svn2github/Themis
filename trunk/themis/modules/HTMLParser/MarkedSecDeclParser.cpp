/*	MarkedSecDeclParser implementation
	See MarkedSecDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "MarkedSecDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"

MarkedSecDeclParser	::	MarkedSecDeclParser( SGMLTextPtr aDocText,
																	 TDocumentPtr aDTD )
								:	DeclarationParser( aDocText, aDTD )	{

	//printf( "Constructing MarkedSecDeclParser\n" );
	
}

MarkedSecDeclParser	::	~MarkedSecDeclParser()	{

	//printf( "Destroying MarkedSecDeclParser\n" );
	
}

bool MarkedSecDeclParser	::	processDeclaration()	{

	process( mMdo );
	process( mDso );
	
	try	{
		processStatusKeyWordSpec();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	try	{
		process( mDso );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	bool mscFound = false;
	while ( ! mscFound )	{
		try	{
			process( mMsc );
			mscFound = true;
		}
		catch( ReadException r )	{
			// Not found yet
			mDocText->nextChar();
		}
	}
	
	try	{
		process( mMdc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	return true;
	
}

void MarkedSecDeclParser	::	processStatusKeyWordSpec()	{

	// Fix me
	
	processPsPlus();
	processStatusKeyWord();
	processPsStar();
	
}

void MarkedSecDeclParser	::	processStatusKeyWord()	{

	if ( process( kCDATA, false ) )	{
		return;
	}
	if ( process( kIGNORE, false ) )	{
		return;
	}
	if ( process( kINCLUDE, false ) )	{
		return;
	}
	if ( process( kRCDATA, false ) )	{
		return;
	}
	if ( process( kTEMP, false ) )	{
		return;
	}

	throw ReadException( mDocText->getLineNr(),
									mDocText->getCharNr(),
									"Status keyword expected" );
	
}
