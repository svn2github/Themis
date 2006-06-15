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
																	 TSchemaPtr aDTD )
								:	DeclarationParser( aDocText, aDTD )	{

	//printf( "Constructing MarkedSecDeclParser\n" );
	
}

MarkedSecDeclParser	::	~MarkedSecDeclParser()	{

	//printf( "Destroying MarkedSecDeclParser\n" );
	
}

bool MarkedSecDeclParser	::	processDeclaration()	{

	if ( ! process( mMdo, false ) )	{
		return false;
	}

	if ( ! process( mDso, false ) )	{
		return false;
	}
	
	processStatusKeyWordSpec();

	if ( ! process( mDso, false ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Dso expected",
										GENERIC, true );
	}
	
	bool mscFound = false;
	while ( ! mscFound )	{
		if ( process( mMsc, false ) )	{
			mscFound = true;
		}
		else	{
			// Not found yet
			mDocText->nextChar();
		}
	}
	
	if ( ! process( mMdc, false ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Marked section declaration not closed correctly",
										GENERIC, true );
	}		
	
	return true;
	
}

void MarkedSecDeclParser	::	processStatusKeyWordSpec()	{

	// Fix me
	processPsPlus();
	if ( ! processStatusKeyWord() )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Status keyword expected",
										GENERIC, true );
	}
	processPsStar();
	
}

bool MarkedSecDeclParser	::	processStatusKeyWord()	{

	if ( process( kCDATA, false ) )	{
		return true;
	}
	if ( process( kIGNORE, false ) )	{
		return true;
	}
	if ( process( kINCLUDE, false ) )	{
		return true;
	}
	if ( process( kRCDATA, false ) )	{
		return true;
	}
	if ( process( kTEMP, false ) )	{
		return true;
	}

	return false;
	
}
