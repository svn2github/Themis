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

	// Element to store the element declarations of the DTD
	mElements = mDTD->createElement( "elements" );
	mDTD->appendChild( mElements );
	
}

ElementDeclParser	::	~ElementDeclParser()	{

}

bool ElementDeclParser	::	processDeclaration()	{

	// Define an element to store the element declaration
	TElementDeclarationPtr declaration = mDTD->createElementDeclaration();
	TElementPtr content = mDTD->createElement( "content" );
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
		TElementPtr elements = mDTD->createElement( "elements" );
		declaration->appendChild( elements );
		elements->appendChild( element );
	}
	declaration->appendChild( content );
	
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
		TElementPtr element = mDTD->createElement( name );
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
		aElement->appendChild( exceptions );
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

	string gi = processGI( false );
	if ( gi == "" )	{
		return TElementPtr();
	}

	TElementPtr elementToken = mDTD->createElement( gi );
	processOccIndicator( elementToken );
	
	return elementToken;
	
}

void ElementDeclParser	::	processOccIndicator( TElementPtr aElementToken )	{

	if ( process( mOpt, false ) )	{
		aElementToken->setAttribute("minOccurs", "0");
		aElementToken->setAttribute("maxOccurs", "1");
	}
	if ( process( mPlus, false ) )	{
		aElementToken->setAttribute("minOccurs", "1");
	}
	if ( process( mRep, false ) )	{
		aElementToken->setAttribute("minOccurs", "0");
	}

}

TElementPtr ElementDeclParser	::	processExceptions()	{
	
	TElementPtr exceptions = mDTD->createElement( "exceptions" );
	
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

	TElementPtr exclusions = mDTD->createElement( kMinus );

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

	TElementPtr inclusions = mDTD->createElement( mPlus );

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
