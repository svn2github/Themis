/*	ElementDeclParser implementation
	See ElementDeclParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// SGMLParser headers
#include "ElementDeclParser.hpp"
#include "ReadException.hpp"
#include "SGMLSupport.hpp"

// DOM headers
#include "TDocument.h"
#include "TElement.h"
#include "TNodeList.h"
#include "TNamedNodeMap.h"

ElementDeclParser	::	ElementDeclParser( SGMLTextPtr aDocText,
															 TDocumentPtr aDTD )
							:	DeclarationParser( aDocText, aDTD )	{

	// Element to store the element declarations of the DTD
	mElements = mDTD->createElement( "elements" );
	mDTD->appendChild( mElements );
	
}

ElementDeclParser	::	~ElementDeclParser()	{

}

bool ElementDeclParser	::	processDeclaration()	{

	// Define an element to store the element declaration
	TElementPtr declaration = mDTD->createElement( "declaration" );
	TElementPtr minimization = mDTD->createElement( "minimization" );
	TElementPtr content = mDTD->createElement( "content" );
	TElementPtr element;

	//process( mMdo );
	if ( ! process( kELEMENT, false ) )	{
		return false;
	}
	
	try	{
		processPsPlus();
		element = processElementType();
		processPsPlus();
		processTagMin( minimization );
		processPsPlus();
		if ( ! processDeclContent( content, false ) )	{
			// Not declared content. Must be a content model
			processContentModel( content );
		}
		processPsStar();
		process( mMdc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	if ( element->hasChildNodes() )	{
		// Name group.
		declaration->appendChild( element );
	}
	else	{
		TElementPtr elements = mDTD->createElement( "elements" );
		declaration->appendChild( elements );
		elements->appendChild( element );
	}
	declaration->appendChild( minimization );
	declaration->appendChild( content );
	
	mElements->appendChild( declaration );
	
	return true;

}

TElementPtr ElementDeclParser	::	processElementType()	{

	try	{
		string name = processGI();
		TElementPtr element = mDTD->createElement( name );
		return element;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}

	try	{
		TElementPtr group = processNameGroup();
		return group;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}
	
	throw ReadException( mDocText->getLineNr(),
									mDocText->getCharNr(),
									"Element type expected" );
	
}

void ElementDeclParser	::	processTagMin( TElementPtr aElement )	{

	if ( process( kO, false ) )	{
		aElement->setAttribute( "start", "false" );
	}
	else	{
		// Not O minimization. Try minus
		if ( process( kMinus, false ) )	{
			aElement->setAttribute( "start", "true" );
		}
		else	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"Start minimization expected" );
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
		aElement->setAttribute( "end", "false" );
	}
	else	{
		// Not O minimization. Try minus
		if ( process( kMinus, false ) )	{
			aElement->setAttribute( "end", "true" );
		}
		else	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"End minimization expected",
											GENERIC,
											true );
		}
	}
	
}

bool ElementDeclParser	::	processDeclContent( TElementPtr aElement,
																	   bool aException )	{

	if ( process( kCDATA, false ) )	{
		TElementPtr cdata = mDTD->createElement( kCDATA );
		aElement->appendChild( cdata );
		return true;
	}

	if ( process( kRCDATA, false ) )	{
		TElementPtr rcdata = mDTD->createElement( kRCDATA );
		aElement->appendChild( rcdata );
		return true;
	}
		
	if ( process( kEMPTY, false ) )	{
		TElementPtr empty = mDTD->createElement( kEMPTY );
		aElement->appendChild( empty );
		return true;
	}

	if ( ! aException )	{
		return false;
	}
	else	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Declared content expected" );
	}

}

void ElementDeclParser	::	processContentModel( TElementPtr aElement )	{

	if ( ! process( kANY, false ) )	{
		// Not ANY. Must be a model group
		TElementPtr modelGroup = processModelGroup();
		aElement->appendChild( modelGroup );
	}

	try	{
		processPsPlus();
		TElementPtr exceptions = processExceptions();
		aElement->appendChild( exceptions );
	}
	catch( ReadException r )	{
		// Do nothing
	}

}

TElementPtr ElementDeclParser	::	processModelGroup()	{
	
	process( mGrpo );

	TElementPtr grpo = mDTD->createElement( "()" );

	try	{
		processTsStar();
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}

	TElementPtr firstPart;
	try	{
		firstPart = processContentToken();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		processTsStar();
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}

	TElementPtr subModelGroup;
	bool smgFound = true;
	while ( smgFound )	{
		try	{
			subModelGroup = processSubModelGroup();
			if ( subModelGroup->getNodeName() == firstPart->getNodeName() )	{
				firstPart->appendChild( subModelGroup->getFirstChild() );
			}
			else	{
				TNodePtr first = subModelGroup->getFirstChild();
				subModelGroup->insertBefore( firstPart, first );
				firstPart = subModelGroup;
			}
		}
		catch( ReadException r )	{
			// No sub model group. Exit the loop
			smgFound = false;
		}
	}
	
	try	{
		process( mGrpc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	grpo->appendChild( firstPart );

	return processOccIndicator( grpo );
	
}

TElementPtr ElementDeclParser	::	processSubModelGroup()	{

	TElementPtr connector = processConnector();
	
	try	{
		processTsStar();
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}

	TElementPtr contentToken;
	try	{	
		contentToken = processContentToken();
		connector->appendChild( contentToken );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		processTsStar();
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}
	
	return connector;
	
}

TElementPtr ElementDeclParser	::	processContentToken()	{

	TElementPtr contentToken;

	try	{
		contentToken = processPrimContentToken();
	}
	catch( ReadException r )	{
		// Not a primitive content token. Must be a model group
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
				mDTD->createElement( mRni + kPCDATA );
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

	TElementPtr elementToken = mDTD->createElement( processGI() );
	return processOccIndicator( elementToken );
	
}

TElementPtr ElementDeclParser	::	processOccIndicator( TElementPtr aElementToken )	{

	if ( process( mOpt, false ) )	{
		TElementPtr occIndicator = mDTD->createElement( mOpt );
		occIndicator->appendChild( aElementToken );
		return occIndicator;
	}
	if ( process( mPlus, false ) )	{
		TElementPtr occIndicator = mDTD->createElement( mPlus );
		occIndicator->appendChild( aElementToken );
		return occIndicator;
	}
	if ( process( mRep, false ) )	{
		TElementPtr occIndicator = mDTD->createElement( mRep );
		occIndicator->appendChild( aElementToken );
		return occIndicator;
	}

	return aElementToken;

}

TElementPtr ElementDeclParser	::	processExceptions()	{
	
	TElementPtr exceptions = mDTD->createElement( "exceptions" );
	
	if ( processExclusions( exceptions ) )	{
		try	{
			processPsPlus();
		}
		catch( ReadException r )	{
			// Was optional. Do nothing
		}
	}
	
	processInclusions( exceptions );

	return exceptions;
	
}

bool ElementDeclParser	::	processExclusions( TElementPtr aExceptions )	{
	
	if ( ! process( kMinus, false ) )	{
		return false;
	}

	TElementPtr exclusions = mDTD->createElement( kMinus );

	try	{
		TElementPtr nameGroup = processNameGroup();
		exclusions->appendChild( nameGroup );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	aExceptions->appendChild( exclusions );
	
	return true;
	
}

bool ElementDeclParser	::	processInclusions( TElementPtr aExceptions )	{

	if ( ! process( mPlus ) )	{
		return false;
	}

	TElementPtr inclusions = mDTD->createElement( mPlus );

	try	{
		TElementPtr nameGroup = processNameGroup();
		inclusions->appendChild( nameGroup );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	aExceptions->appendChild( inclusions );

	return true;
	
}

void ElementDeclParser	::	showTree( const TNodePtr aNode, int aSpacing )	{
	
	TNodeListPtr children = aNode->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = 0; i < length; i++ )	{
			TNodePtr child = children->item( i );
			for ( int j = 0; j < aSpacing; j++ )	{
				cout << "  ";
			}
			cout << "Child name: " << child->getNodeName().c_str() << endl;
			if ( child->getNodeType() == ELEMENT_NODE )	{
				// Check for attributes
				TNamedNodeMapPtr attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodePtr attr = attributes->item( j );
					for ( int j = 0; j < aSpacing + 1; j++ )	{
						cout << "  ";
					}
					cout << "Attribute " << attr->getNodeName();
					cout << " with value " << attr->getNodeValue() << endl;
				}
			}
			showTree( child, aSpacing + 1 );
		}
	}	
}
