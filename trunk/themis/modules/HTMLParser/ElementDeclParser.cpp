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

void ElementDeclParser	::	processDeclaration()	{

	// Define an element to store the element declaration
	TElementPtr element;

	//process( mMdo );
	process( kELEMENT );
	
	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	try	{
		element = processElementType();
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

	// Find right element to put information in
	TElementPtr infoElement = element;
	if ( element->hasChildNodes() )	{
		// Name group. Assuming has connector as child
		TNodePtr connector = element->getFirstChild();
		TNodePtr child = connector->getFirstChild();
		infoElement = shared_static_cast<TElement>( child );
	}

	try	{
		processTagMin( infoElement );
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		processDeclContent( infoElement );
	}
	catch( ReadException r )	{
		// Not declared content. Must be a content model
		try	{
			processContentModel( infoElement );
		}
		catch( ReadException r )	{
			r.setFatal();
			throw r;
		}
	}

	mElements->appendChild( element );

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

}

TElementPtr ElementDeclParser	::	processElementType()	{

	string text = "";
	
	try	{
		text = processGI();
		TElementPtr element = mDTD->createElement( text );
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

void ElementDeclParser	::	processDeclContent( TElementPtr aElement )	{

	if ( process( kCDATA, false ) )	{
		TElementPtr cdata = mDTD->createElement( kCDATA );
		aElement->appendChild( cdata );
		return;
	}

	if ( process( kRCDATA, false ) )	{
		TElementPtr rcdata = mDTD->createElement( kRCDATA );
		aElement->appendChild( rcdata );
		return;
	}
		
	if ( process( kEMPTY, false ) )	{
		TElementPtr empty = mDTD->createElement( kEMPTY );
		aElement->appendChild( empty );
		return;
	}

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
									"Declared content expected" );

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

	TElementPtr occIndicator;
	try	{
		occIndicator = processOccIndicator();
		occIndicator->appendChild( grpo );
		return occIndicator;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}
	
	return grpo;
	
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

	try	{
		process( mRni );
	}
	catch( ReadException r )	{
		// Not an rni. Must be an element token
		primContentToken = processElementToken();
		return primContentToken;
	}
	try	{
		process( kPCDATA );
		primContentToken = mDTD->createElement( mRni + kPCDATA );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	return primContentToken;
		
}

TElementPtr ElementDeclParser	::	processElementToken()	{

	TElementPtr elementToken = mDTD->createElement( processGI() );
	try	{
		TElementPtr occIndicator = processOccIndicator();
		occIndicator->appendChild( elementToken );
		return occIndicator;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	
	return elementToken;
	
}

TElementPtr ElementDeclParser	::	processOccIndicator()	{

	try	{
		process( mOpt );
		TElementPtr occIndicator = mDTD->createElement( mOpt );
		return occIndicator;
	}
	catch( ReadException r )	{
		// Do  nothing
	}
	try	{
		process( mPlus );
		TElementPtr occIndicator = mDTD->createElement( mPlus );
		return occIndicator;
	}
	catch( ReadException r )	{
		// Do  nothing
	}
	try	{
		process( mRep );
		TElementPtr occIndicator = mDTD->createElement( mRep );
		return occIndicator;
	}
	catch( ReadException r )	{
		// Do  nothing
	}

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
									"Occurence indicator expected" );

}

TElementPtr ElementDeclParser	::	processExceptions()	{
	
	TElementPtr exceptions = mDTD->createElement( "exceptions" );
	
	try	{
		TElementPtr exclusions = processExclusions();
		exceptions->appendChild( exclusions );
		try	{
			processPsPlus();
			TElementPtr inclusions = processInclusions();
			exceptions->appendChild( inclusions );
		}
		catch( ReadException r )	{
			// Was optional. Do nothing
		}
	}
	catch( ReadException r )	{
		// Not an exclusion. Try an inclusion
		TElementPtr inclusions = processInclusions();
		exceptions->appendChild( inclusions );
	}

	return exceptions;
	
}

TElementPtr ElementDeclParser	::	processExclusions()	{
	
	process( kMinus );

	TElementPtr minus = mDTD->createElement( kMinus );

	try	{
		TElementPtr nameGroup = processNameGroup();
		minus->appendChild( nameGroup );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	return minus;
	
}

TElementPtr ElementDeclParser	::	processInclusions()	{

	process( mPlus );

	TElementPtr plus = mDTD->createElement( mPlus );

	try	{
		TElementPtr nameGroup = processNameGroup();
		plus->appendChild( nameGroup );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	return plus;
	
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
