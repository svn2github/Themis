/*	BaseParser implementation
	See BaseParser.hpp for more information
*/

// Standard C headers
#include <stdio.h>
#include <ctype.h>

// BaseParser headers
#include "BaseParser.hpp"
#include "ReadException.hpp"
#include "State.hpp"
#include "SGMLSupport.hpp"

// DOM headers
#include "TNode.h"
#include "TElement.h"
#include "TNodeList.h"

BaseParser	::	BaseParser()	{
	
	//printf( "BaseParser constructed\n" );

	// SGMLText to store the text
	mDocText = SGMLTextPtr( new SGMLText() );

	createDTD();

	setupSyntax();

}

BaseParser	::	~BaseParser()	{
	
	//printf( "BaseParser destroyed\n" );
	
}

void BaseParser	::	setDocText( SGMLTextPtr aDocText )	{
	
	mDocText = aDocText;
	
}

void BaseParser	::	createDTD()	{

	// Document to store information about the dtd
	mDTD = TDocumentPtr( new TDocument() );

	// Element to store parameter entities
	mParEntities = mDTD->createElement( "parEntities" );
	mDTD->appendChild( mParEntities );

	// Element to store character entities
	mCharEntities = mDTD->createElement( "charEntities" );
	mDTD->appendChild( mCharEntities );
	
}

void BaseParser	::	setupSyntax()	{
	
	// Reference syntax
	mMdo	= "<!";	
	mMdc	= ">";
	mGrpo	= "(";
	mGrpc	= ")";
	mDso	= "[";
	mMsc	= "]]";
	mRefc	= ";";
	mCom	= "--";
	mPero	= "%";
	mLit		= "\"";
	mLitA	= "'";
	mRni		= "#";
	mOpt	= "?";
	mPlus	= "+";
	mRep	= "*";
	mAnd	= "&";
	mOr		= "|";
	mSeq	= ",";
	mStago	= "<";
	mTagc	= ">";
	mEtago	=	"</";
	mVi		= "=";

}

bool BaseParser	::	process( const string & symbol, bool aException )	{

	State save = mDocText->saveState();
	unsigned int i = 0;
	while ( i < symbol.size() )	{
		if ( mDocText->getChar() == symbol[ i ] )	{
			mDocText->nextChar();
			i++;
		}
		else	{
			mDocText->restoreState( save );
			if ( ! aException )	{
				return false;
			}
			else	{
				string error = "Expected ";
				error += symbol;
				throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), error );
			}
		}
	}
	
	return true;
	
}

bool BaseParser	::	processS( bool aException )	{

	if ( isspace( mDocText->getChar() ) || iscntrl( mDocText->getChar() ) )	{
		mDocText->nextChar();
	}
	else	{
		if ( aException )	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"S expected" );
		}
		else	{
			return false;
		}
	}

	return true;
	
}

bool BaseParser	::	processEe( bool aException )	{
	
	if ( iscntrl( mDocText->getChar() ) )	{
		mDocText->nextChar();
	}
	else	{
		if ( aException )	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"Ee expected" );
		}
		else	{
			return false;
		}
	}
	
	return true;
	
}

void BaseParser	::	processComment()	{

	string comment = "";

	process( mCom );
	bool comFound = false;
	while ( ! comFound )	{
		if ( process( mCom, false ) )	{
			comFound = true;
		}
		else	{
			// Not yet found. Read next character
			comment += mDocText->getChar();
			mDocText->nextChar();
		}
	}
	
}

void BaseParser	::	processParEntityReference()	{

	// Fix me
	State save = mDocText->saveState();
	process( mPero );
	
	string name = "";
	try	{
		name = processName();
	}
	catch( ReadException r )	{
		// Probably not a parameter entity reference. Set index back
		mDocText->restoreState( save );
		throw r;
	}

	// Can be omitted
	process( mRefc, false );

	TNodeListPtr children = mParEntities->getChildNodes();

	for ( unsigned int i = 0; i < children->getLength(); i++ )	{
		TNodePtr node = children->item( i );
		if ( node->getNodeName() == name )	{
			TElementPtr child = shared_static_cast<TElement>( node );
			// Check what kind of text it is
			if ( child->getAttribute( "type" ) != kPUBLIC )	{
				Position entityPosition = mEntityTexts[ name ];
				if ( entityPosition.getSize() != 0 )	{
					mDocText->addEntity( entityPosition );
				}
			}
			else	{
				//printf( "Equal to PUBLIC\n" );
			}
		}
	}
	
}

string BaseParser	::	processLiteral()	{

	if ( process( mLit, false ) )	{
		return mLit;
	}
	if ( process( mLitA, false ) )	{
		return mLitA;
	}

	throw ReadException( mDocText->getLineNr(),
									mDocText->getCharNr(),
									"Literal expected" );

}

void BaseParser	::	processParLiteral( TElementPtr & entity )	{

	string text = "";
	string literal = processLiteral();

	unsigned int start = mDocText->getIndex();
	unsigned int lineNr = mDocText->getLineNr();
	unsigned int charNr = mDocText->getCharNr();
	
	bool litFound = false;
	while ( ! litFound )	{
		if ( process( literal, false ) )	{
			litFound = true;
		}
		else	{
			// Not yet found. Process replacable parameter data
			try	{
				text += processRepParData();
			}
			catch( ReadException r )	{
				r.setFatal();
				throw r;
			}
		}
	}

	Position textPosition( start, mDocText->getIndex() - start - 1, lineNr, charNr );

	mEntityTexts.insert(
		map<string, Position>::value_type( entity->getNodeName(), textPosition ) );

	entity->setAttribute( "text", text );
	
}

string BaseParser	::	processMinLiteral()	{
	
	string text = "";
	string literal = processLiteral();

	bool litFound = false;
	while ( ! litFound )	{
		if ( process( literal, false ) )	{
			litFound = true;
		}
		else	{
			// Not yet found. Process replacable parameter data
			try	{
				text += processMinData();
			}
			catch( ReadException r )	{
				r.setFatal();
				throw r;
			}
		}
	}
	
	return text;
	
}

string BaseParser	::	processMinData()	{
	
	// Fix me
	string text = "";
	text += mDocText->getChar();
	mDocText->nextChar();
	
	return text;
	
}

string BaseParser	::	processName()	{

	string name = "";

	if ( ! isalpha( mDocText->getChar() ) )	{
		throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Not a name" );
	}
	
	name += toupper( mDocText->getChar() );
	char c = mDocText->nextChar();
	
	while ( isalnum( c ) ||
			  c == '-' ||
			  c == '.' )	{
		name += toupper( c );
		c = mDocText->nextChar();
	}
	
	return name;
	
}

string BaseParser	::	processNameToken()	{
	
	string token = "";

	unsigned int start = mDocText->getIndex();
	char c = mDocText->getChar();

	while ( isalnum( c ) ||
			  c == '-' ||
			  c == '.' )	{
		token += c;
		c = mDocText->nextChar();
	}
	
	if ( start == mDocText->getIndex() )	{
		throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Not a name token" );
	}
	
	return token;
	
}

void BaseParser	::	processSStar()	{
	
	bool sFound = true;
	while ( sFound )	{
		try	{
			processS();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				sFound = false;
			}
		}
	}

}

void BaseParser	::	processPs()	{

	switch( mDocText->getChar() )	{
		case '-':	{
			try	{
				processComment();
				return;
			}
			catch( ReadException r )	{
				if ( r.isFatal() )	{
					throw;
				}
				break;
			}
		}
		case '%':	{
			try	{
				processParEntityReference();
				return;
			}
			catch( ReadException r )	{
				if ( r.isFatal() )	{
					throw r;
				}
				break;
			}
		}
	}

	if ( processS( false ) )	{
		return;
	}
	if ( processEe( false ) )	{
		return;
	}

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Ps expected" );
	
}

void BaseParser	::	processPsStar()	{

	bool psFound = true;
	while ( psFound )	{
		try	{
			processPs();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				psFound = false;
			}
		}
	}
	
}

void BaseParser	::	processPsPlus()	{
	
	try	{
		processPs();
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}
	try	{
		processPsStar();
	}
	catch( ReadException r )	{
		// Do nothing
	}
	
}

void BaseParser	::	processTs()	{

	if ( mDocText->getChar() == mPero[0] )	{
		try	{
			processParEntityReference();
			return;
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
		}	
	}

	if ( processS( false ) )	{
		return;
	}
	if ( processEe( false ) )	{
		return;
	}

	throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Ts expected" );
	
}

void BaseParser	::	processTsStar()	{

	bool tsFound = true;
	while ( tsFound )	{
		try	{
			processTs();
		}
		catch( ReadException r )	{
			if ( r.isFatal() )	{
				throw r;
			}
			else	{
				tsFound = false;
			}
		}
	}
	
}

string BaseParser	::	processRepParData()	{

	// Temporary. Fix me.
	string text = "";
	text += mDocText->getChar();
	mDocText->nextChar();
	
	return text;
	
}

string BaseParser	::	processRepCharData()	{

	// Temporary. Fix me.
	string text = "";
	text += mDocText->getChar();
	mDocText->nextChar();
	
	return text;
	
}

string BaseParser	::	processGI()	{

	if ( mDocText->getChar() == '/' )	{
		throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(), "Is an end tag",
										END_TAG_FOUND );
	}

	// Generic identifier and name are equivalent
	return processName();
	
}

TElementPtr BaseParser	::	processNameGroup()	{

	process( mGrpo );

	TElementPtr group = mDTD->createElement( "elements" );

	processTsStar();
	
	string name = processName();
	TElementPtr element = mDTD->createElement( name );
	group->appendChild( element );

	//TElementPtr subGroup;
	bool inGroup = true;
	while ( inGroup )	{
		try	{
			processTsStar();
			TElementPtr connector = processConnector();
			processTsStar();
			name = processName();
			element = mDTD->createElement( name );
			group->appendChild( element );
		}
		catch( ReadException r )	{
			inGroup = false;
		}
	}

	processTsStar();
	try	{
		process( mGrpc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	return group;
	
}

string BaseParser	::	processNameTokenGroup()	{

	process( mGrpo );
	
	processTsStar();
	
	processNameToken();
	
	bool inGroup = true;
	while ( inGroup )	{
		try	{
			processTsStar();
			processConnector();
			processTsStar();
			processNameToken();
		}
		catch( ReadException r )	{
			inGroup = false;
		}
	}
	
	processTsStar();
	try	{
		process( mGrpc );
	}
	catch( ReadException r )	{
		r.setFatal();
		throw r;
	}

	return "";
	
}

TElementPtr BaseParser	::	processConnector()	{

	TElementPtr connector;

	if ( process( mAnd, false ) )	{
		return mDTD->createElement( mAnd );
	}
	if ( process( mOr, false ) )	{
		return mDTD->createElement( mOr );
	}
	if ( process( mSeq, false ) )	{
		return mDTD->createElement( mSeq );
	}

	throw ReadException( mDocText->getLineNr(),
									mDocText->getCharNr(),
									"Connector expected" );

}

string BaseParser	::	processAttrValueSpec()	{

	try	{
		// Switched around from spec in book. Is easier
		return processAttrValueLit();
	}
	catch( ReadException r )	{
		// Do nothing
	}

	return processAttrValue();
	
}

string BaseParser	::	processAttrValue()	{

	// FIX ME!!!!!!!!!!!!!!!!!!!!!
	string result = processCharData( mTagc );
	if ( result == "" )	{
		throw ReadException( mDocText->getLineNr(), mDocText->getCharNr(),
										"AttrValue expected" );
	}
	//processNameToken();
	
	return result;
	
}

string BaseParser	::	processAttrValueLit()	{

	string result = "";
	string literal = processLiteral();

	bool litFound = false;
	while ( ! litFound )	{
		if ( process( literal, false ) )	{
			litFound = true;
		}
		else	{
			// Not yet found. Process replacable parameter data
			try	{
				result += processRepCharData();
			}
			catch( ReadException r )	{
				r.setFatal();
				throw r;
			}
		}
	}
	
	return result;
	
}

string BaseParser	::	processCharData( string aEndString, bool aSpaceEnd )	{

	string result;

	bool dataCharFound = true;
	while ( dataCharFound )	{
		State save = mDocText->saveState();
		if ( ! process( aEndString, false ) )	{
			if ( aSpaceEnd && isspace( mDocText->getChar() ) )	{
				dataCharFound = false;
			}
			else	{
				result += processDataChar();
			}
		}
		else	{
			mDocText->restoreState( save );
			dataCharFound = false;
		}
	}
	
	return result;
	
}

char BaseParser	::	processDataChar()	{

	char c = mDocText->getChar();

	mDocText->nextChar();

	return c;
	
}
