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
																	 TDocumentShared aDTD,
																	 TElementShared aParEntities,
																	 TElementShared aCharEntities )
								:	DeclarationParser( aDocText, aDTD, aParEntities, aCharEntities )	{

	printf( "Constructing MarkedSecDeclParser\n" );
	
}

MarkedSecDeclParser	::	~MarkedSecDeclParser()	{

	printf( "Destroying MarkedSecDeclParser\n" );
	
}

void MarkedSecDeclParser	::	processDeclaration()	{

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
	
}

void MarkedSecDeclParser	::	processStatusKeyWordSpec()	{

	// Fix me
	
	processPsPlus();
	processStatusKeyWord();
	processPsStar();
	
}

void MarkedSecDeclParser	::	processStatusKeyWord()	{

	try	{
		process( kCDATA );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}	
	try	{
		process( kIGNORE );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}	
	try	{
		process( kINCLUDE );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}	
	try	{
		process( kRCDATA );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}	
	try	{
		process( kTEMP );
		return;
	}
	catch( ReadException r )	{
		// Do nothing
	}	

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Status keyword expected" );
	
}
