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
															 TDocumentShared aDTD,
															 TElementShared aParEntities,
															 TElementShared aCharEntities )
							:	DeclarationParser( aDocText, aDTD, aParEntities, aCharEntities )	{

	printf( "Constructing ElementDeclParser\n" );

	// Element to store the element declarations of the DTD
	mElements = mDTD->createElement( "elements" );
	mDTD->appendChild( mElements );
	
}

ElementDeclParser	::	~ElementDeclParser()	{

	printf( "Destroying ElementDeclParser\n" );
	
	//showTree( mElements, 0 );
	
}

void ElementDeclParser	::	processDeclaration()	{

	// Define an element to store the element declaration
	TElementShared element;

	process( mMdo );
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

	try	{
		processTagMin( element );
		try	{
			processPsPlus();
		}
		catch( ReadException r )	{
			r.setFatal();
			throw r;
		}
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	try	{
		processDeclContent();
	}
	catch( ReadException r )	{
		// Not declared content. Must be a content model
		try	{
			processContentModel( element );
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

TElementShared ElementDeclParser	::	processElementType()	{

	string text = "";
	
	try	{
		text = processGI();
		TElementShared element = mDTD->createElement( text );
		return element;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}

	try	{
		text = processNameGroup();
		TElementShared element = mDTD->createElement( text );
		return element;
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}
	
	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
									"Element type expected" );
	
}

void ElementDeclParser	::	processTagMin( TElementShared aElement )	{

	try	{
		process( kO );
		aElement->setAttribute( "start", "false" );
	}
	catch( ReadException r )	{
		// Not O minimization. Try minus
		process( kMinus );
		aElement->setAttribute( "start", "true" );
	}

	try	{
		processPsPlus();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	try	{
		process( kO );
		aElement->setAttribute( "end", "false" );
	}
	catch( ReadException r )	{
		// Not O minimization. Try minus
		try	{
			process( kMinus );
			aElement->setAttribute( "end", "true" );
		}
		catch( ReadException r )	{
			r.setFatal();
			throw r;
		}
	}
	
}

void ElementDeclParser	::	processDeclContent()	{

	try	{
		process( kCDATA );
	}
	catch( ReadException r )	{
		// Not CDATA. Try RCDATA
		try	{
			process( kRCDATA );
		}
		catch( ReadException r )	{
			// Not RCDATA. Try EMPTY
			process( kEMPTY );
		}
	}

}

void ElementDeclParser	::	processContentModel( TElementShared aElement )	{

	try	{
		process( kANY );
	}
	catch( ReadException r )	{
		// Not ANY. Must be a model group
		TElementShared modelGroup = processModelGroup();
		aElement->appendChild( modelGroup );
	}

	try	{
		processPsPlus();
		processExceptions( aElement );
	}
	catch( ReadException r )	{
		// Do nothing
	}

}

TElementShared ElementDeclParser	::	processModelGroup()	{
	
	process( mGrpo );

	TElementShared grpo = mDTD->createElement( "()" );

	try	{
		processTsStar();
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}

	TElementShared firstPart;
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

	TElementShared subModelGroup;
	bool smgFound = true;
	while ( smgFound )	{
		try	{
			subModelGroup = processSubModelGroup();
			TNodeShared first = make_shared( subModelGroup->getFirstChild() );
			subModelGroup->insertBefore( firstPart, first );
			firstPart = subModelGroup;
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

	TElementShared occIndicator;
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

TElementShared ElementDeclParser	::	processSubModelGroup()	{

	TElementShared connector = processConnector();
	
	try	{
		processTsStar();
	}
	catch( ReadException r )	{
		if ( r.isFatal() )	{
			throw r;
		}
	}

	TElementShared contentToken;
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

TElementShared ElementDeclParser	::	processContentToken()	{

	TElementShared contentToken;

	try	{
		contentToken = processPrimContentToken();
	}
	catch( ReadException r )	{
		// Not a primitive content token. Must be a model group
		contentToken = processModelGroup();
	}

	return contentToken;

}

TElementShared ElementDeclParser	::	processPrimContentToken()	{

	TElementShared primContentToken;

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
		primContentToken = mDTD->createElement( kPCDATA );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
	return primContentToken;
		
}

TElementShared ElementDeclParser	::	processElementToken()	{

	TElementShared elementToken = mDTD->createElement( processGI() );
	TElementShared occIndicator;
	try	{
		occIndicator = processOccIndicator();
		occIndicator->appendChild( elementToken );
		return occIndicator;
	}
	catch( ReadException r )	{
		// Do nothing
	}
	
	return elementToken;
	
}

TElementShared ElementDeclParser	::	processOccIndicator()	{

	try	{
		process( mOpt );
		TElementShared occIndicator = mDTD->createElement( mOpt );
		return occIndicator;
	}
	catch( ReadException r )	{
		// Do  nothing
	}
	try	{
		process( mPlus );
		TElementShared occIndicator = mDTD->createElement( mPlus );
		return occIndicator;
	}
	catch( ReadException r )	{
		// Do  nothing
	}
	try	{
		process( mRep );
		TElementShared occIndicator = mDTD->createElement( mRep );
		return occIndicator;
	}
	catch( ReadException r )	{
		// Do  nothing
	}

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
									"Occurence indicator expected" );

}

void ElementDeclParser	::	processExceptions( TElementShared aElement )	{
	
	try	{
		processExclusions( aElement );
		try	{
			processPsPlus();
			processInclusions( aElement );
		}
		catch( ReadException r )	{
			// Was optional. Do nothing
		}
		return;
	}
	catch( ReadException r )	{
		// Not an exclusion. Try an inclusion
		processInclusions( aElement );
	}
	
}

void ElementDeclParser	::	processExclusions( TElementShared aElement )	{
	
	process( kMinus );
	try	{
		processNameGroup();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
}

void ElementDeclParser	::	processInclusions( TElementShared aElement )	{

	process( mPlus );
	try	{
		processNameGroup();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	
}

void ElementDeclParser	::	showTree( const TNodeShared aNode, int aSpacing )	{
	
	TNodeListShared children = aNode->getChildNodes();
	int length = children->getLength();
	if ( length != 0 )	{
		for ( int i = 0; i < length; i++ )	{
			TNodeShared child = make_shared( children->item( i ) );
			for ( int j = 0; j < aSpacing; j++ )	{
				cout << "  ";
			}
			cout << "Child name: " << child->getNodeName().c_str() << endl;
			if ( child->getNodeType() == ELEMENT_NODE )	{
				// Check for attributes
				TNamedNodeMapShared attributes = child->getAttributes();
				for ( unsigned int j = 0; j < attributes->getLength(); j++ )	{
					TNodeWeak attr = attributes->item( j );
					TNodeShared tempAttr = make_shared( attr );
					for ( int j = 0; j < aSpacing + 1; j++ )	{
						cout << "  ";
					}
					cout << "Attribute " << tempAttr->getNodeName();
					cout << " with value " << tempAttr->getNodeValue() << endl;
				}
			}
			showTree( child, aSpacing + 1 );
		}
	}	
}
