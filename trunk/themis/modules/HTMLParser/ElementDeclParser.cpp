/*	ElementDeclParser implementation
	See ElementDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "ElementDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"
#include "TSchema.hpp"
#include "TElementDeclaration.hpp"

// DOM headers
#include "TElement.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"

ElementDeclParser	::	ElementDeclParser( SGMLTextPtr aDocText,
										   TSchemaPtr aDTD )
					:	DeclarationParser( aDocText, aDTD )	{

}

ElementDeclParser	::	~ElementDeclParser()	{

}

bool ElementDeclParser	::	processDeclaration()	{

	// Define an element to store the element declaration
	TElementDeclarationPtr declaration = mSchema->createElementDeclaration();
	TElementPtr content = mSchema->createElement( "content" );
	TElementPtr element;

	//process( mMdo );
	if ( ! process( kELEMENT, false ) )	{
		return false;
	}
	
	processPsPlus();
	element = processElementType();
	if ( element.get() == NULL )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Element type expected",
										GENERIC, true );
	}
	processPsPlus();

	if ( ! processTagMin( declaration ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Tag minimization expected",
										GENERIC, true );
	}		
	processPsPlus();

	if ( ! processDeclContent( content ) )	{
		// Not declared content. Must be a content model
		if ( ! processContentModel( content ) )	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"Declared content of content model expected",
											GENERIC, true );
		}
	}

	processPsStar();
	if ( ! process( mMdc, false ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Element declaration not closed correctly",
										GENERIC, true );
	}		

	if ( element->hasChildNodes() )	{
		// Name group.
		declaration->appendChild( element );
	}
	else	{
		TElementPtr elements = mSchema->createElement( "elements" );
		elements->appendChild( element );
		declaration->appendChild( elements );
	}
	// Only add the content model if it has one.
	if ( content->hasChildNodes() ) {
		declaration->appendChild( content );
	}
	
	mElements->appendChild( declaration );
	
	return true;

}

TElementPtr ElementDeclParser	::	processElementType()	{

	string name = processGI( false );
	if ( name == "" )	{
		TElementPtr group = processNameGroup();
		if ( group.get() != NULL )	{
			return group;
		}
	}
	else	{
		TElementPtr element = mSchema->createElement( name );
		return element;
	}

	return TElementPtr();

}

bool ElementDeclParser	::	processTagMin( TElementDeclarationPtr aDeclaration )	{

	bool start = false;
	bool end = false;

	if ( process( kO, false ) )	{
		start = false;
	}
	else	{
		// Not O minimization. Try minus
		if ( process( kMinus, false ) )	{
			start = true;
		}
		else	{
			return false;
		}
	}

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	if ( process( kO, false ) )	{
		end = false;
	}
	else	{
		// Not O minimization. Try minus
		if ( process( kMinus, false ) )	{
			end = true;
		}
		else	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"End minimization expected",
											GENERIC,
											true );
		}
	}

	aDeclaration->setMinimization( start, end );

	return true;
	
}

bool ElementDeclParser	::	processDeclContent( TElementPtr aElement )	{

	if ( process( kCDATA, false ) )	{
		TElementPtr cdata = mSchema->createElement( kCDATA );
		aElement->appendChild( cdata );
		return true;
	}

	if ( process( kRCDATA, false ) )	{
		TElementPtr rcdata = mSchema->createElement( kRCDATA );
		aElement->appendChild( rcdata );
		return true;
	}
		
	if ( process( kEMPTY, false ) )	{
		// This declaration doesn't have any content.
		return true;
	}

	return false;

}

bool ElementDeclParser	::	processContentModel( TElementPtr aElement )	{

	if ( ! process( kANY, false ) )	{
		// Not ANY. Must be a model group
		TElementPtr modelGroup = processModelGroup();
		if ( modelGroup.get() == NULL )	{
			return false;
		}
		else	{
			aElement->appendChild( modelGroup );
		}
	}

	if ( processPsPlus( false ) )	{
		TElementPtr exceptions = processExceptions();
		if (exceptions->hasChildNodes()) {
			aElement->appendChild( exceptions );
		}
	}
	
	return true;

}

TElementPtr ElementDeclParser	::	processModelGroup()	{
	
	if ( ! process( mGrpo, false ) )	{
		return TElementPtr();
	}

	processTsStar();

	TElementPtr firstPart;
	firstPart = processContentToken();
	if ( firstPart.get() == NULL )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Content token expected",
										GENERIC, true );
	}
	
	processTsStar();

//	TElementPtr grpo = mDTD->createElement( "()" );
	TElementPtr subModelGroup;
	bool smgFound = true;
	while ( smgFound )	{
		subModelGroup = processSubModelGroup();
		if ( subModelGroup.get() == NULL )	{
			smgFound = false;
			continue;
		}
		if ( subModelGroup->getNodeName() == firstPart->getNodeName() )	{
			firstPart->appendChild( subModelGroup->getFirstChild() );
		}
		else	{
			TNodePtr first = subModelGroup->getFirstChild();
			subModelGroup->insertBefore( firstPart, first );
			firstPart = subModelGroup;
		}
	}
	
	if ( ! process( mGrpc, false ) )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Model group not closed correctly",
										GENERIC, true );
	}		

//	grpo->appendChild( firstPart );

	processOccIndicator( firstPart );
	
	return firstPart;
	
}

TElementPtr ElementDeclParser	::	processSubModelGroup()	{

	TElementPtr connector = processConnector();
	if ( connector.get() == NULL )	{
		return TElementPtr();
	}
	
	processTsStar();

	TElementPtr contentToken;
	contentToken = processContentToken();
	connector->appendChild( contentToken );
	
	processTsStar();
	
	return connector;
	
}

TElementPtr ElementDeclParser	::	processContentToken()	{

	TElementPtr contentToken;

	contentToken = processPrimContentToken();
	if ( contentToken.get() == NULL )	{
		contentToken = processModelGroup();
	}

	return contentToken;

}

TElementPtr ElementDeclParser	::	processPrimContentToken()	{

	TElementPtr primContentToken;

	if ( ! process( mRni, false ) )	{
		// Not an rni. Must be an element token
		primContentToken = processElementToken();
	}
	else	{
		if ( process( kPCDATA, false ) )	{
			primContentToken =
				mSchema->createElement( mRni + kPCDATA );
		}
		else	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"PCDATA expected",
											GENERIC, true );
		}
	}
	
	return primContentToken;
		
}

TElementPtr ElementDeclParser	::	processElementToken()	{

	string gi = processGI( false );
	if ( gi == "" )	{
		return TElementPtr();
	}

	TElementPtr elementToken = mSchema->createElement( gi );
	processOccIndicator( elementToken );
	
	return elementToken;
	
}

void ElementDeclParser	::	processOccIndicator( TElementPtr aElementToken )	{

	if ( process( mOpt, false ) )	{
		aElementToken->setAttribute("minOccurs", "0");
		aElementToken->setAttribute("maxOccurs", "1");
	}
	else {
		if ( process( mPlus, false ) )	{
			aElementToken->setAttribute("minOccurs", "1");
		}
		else {
			if ( process( mRep, false ) )	{
				aElementToken->setAttribute("minOccurs", "0");
			}
		}
	}

}

TElementPtr ElementDeclParser	::	processExceptions()	{

	TElementPtr exceptions = mSchema->createElement( "exceptions" );
	
	if ( processExclusions( exceptions ) )	{
		// WARNING. This looks dodgy.
		// Probably want to have processInclusions
		// in this if as well as outside.
		processPsPlus( false );
	}
	
	processInclusions( exceptions );

	return exceptions;
	
}

bool ElementDeclParser	::	processExclusions( TElementPtr aExceptions )	{
	
	if ( ! process( kMinus, false ) )	{
		return false;
	}

	TElementPtr exclusions = mSchema->createElement( kMinus );

	TElementPtr nameGroup = processNameGroup();
	if ( nameGroup.get() == NULL )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Name group expected",
										GENERIC, true );
	}
	exclusions->appendChild( nameGroup );

	aExceptions->appendChild( exclusions );
	
	return true;
	
}

bool ElementDeclParser	::	processInclusions( TElementPtr aExceptions )	{

	if ( ! process( mPlus, false ) )	{
		return false;
	}

	TElementPtr inclusions = mSchema->createElement( mPlus );

	TElementPtr nameGroup = processNameGroup();
	if ( nameGroup.get() == NULL )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Name group expected",
										GENERIC, true );
	}
	inclusions->appendChild( nameGroup );

	aExceptions->appendChild( inclusions );

	return true;
	
}
