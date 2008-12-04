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
#include "TSchema.hpp"

// DOM headers
#include "TNode.h"
#include "TElement.h"
#include "TNodeList.h"

BaseParser	::	BaseParser( TSchemaPtr aSchema )	{
	
	//printf( "BaseParser constructed\n" );

	// SGMLText to store the text
	mDocText = SGMLTextPtr( new SGMLText() );

	setupSyntax();

	setSchema( aSchema );

}

BaseParser	::	~BaseParser()	{
	
	//printf( "BaseParser destroyed\n" );
	
}

void BaseParser	::	setSchema( TSchemaPtr aSchema )	{

	// Setup the shortcuts if the schema has been filled.
	if (aSchema.get() != NULL) {
		mSchema = aSchema;
		mCharEntities = mSchema->getCharEntities();
		mParEntities = mSchema->getParEntities();
		mElements = mSchema->getElements();
		mAttrLists = mSchema->getAttrLists();
	}
	
}

void BaseParser	::	setDocText( SGMLTextPtr aDocText )	{
	
	mDocText = aDocText;
	
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
	mLit	= "\"";
	mLitA	= "'";
	mRni	= "#";
	mOpt	= "?";
	mPlus	= "+";
	mRep	= "*";
	mAnd	= "&";
	mOr		= "|";
	mSeq	= ",";
	mStago	= "<";
	mTagc	= ">";
	mEtago	= "</";
	mVi		= "=";

}

bool BaseParser	::	process( const string & symbol, bool aException )	{

	unsigned int i = 0;
	if ( mDocText->getChar() == symbol[ i ] )	{
		State save = mDocText->saveState();
		mDocText->nextChar();
		i++;
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
					throw ReadException( mDocText->getLineNr(),
													mDocText->getCharNr(),
													error );
				}
			}
		}
		return true;
	}
	else {
		if ( ! aException )	{
			return false;
		}
		else	{
			string error = "Expected ";
			error += symbol;
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											error );
		}
	}
	
}

bool BaseParser	::	processS()	{

	char c = mDocText->getChar();

	if ( isspace( c ) || iscntrl( c ) )	{
		mDocText->nextChar();
	}
	else	{
		return false;
	}

	return true;
	
}

bool BaseParser	::	processEe()	{
	
	if ( iscntrl( mDocText->getChar() ) )	{
		mDocText->nextChar();
	}
	else	{
		return false;
	}
	
	return true;
	
}

bool BaseParser	::	processComment()	{

	string comment = "";

	if ( ! process( mCom, false ) )	{
		return false;
	}
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
	
	return true;
	
}

bool BaseParser	::	processParEntityReference()	{

	// Fix me
	State save = mDocText->saveState();
	if ( ! process( mPero, false ) )	{
		return false;
	}
	
	string name =  processName( false );
	if ( name == "" )	{
		mDocText->restoreState( save );
		return false;
	}

	// Can be omitted
	process( mRefc, false );

	TNodeListPtr entities = mParEntities->getElementsByTagName(name);
	TNodePtr node = entities->item(0);
	TElementPtr entity = shared_static_cast<TElement>( node );
	if ( entity->getAttribute( "type" ) != kPUBLIC )	{
		Position entityPosition = mSchema->getEntityPosition(name);
		if ( entityPosition.getSize() != 0 )	{
			mDocText->addEntity( entityPosition );
		}
	}
	else	{
		//printf( "Equal to PUBLIC\n" );
	}

	return true;
	
}

string BaseParser	::	processLiteral()	{

	if ( process( mLit, false ) )	{
		return mLit;
	}
	if ( process( mLitA, false ) )	{
		return mLitA;
	}

	return "";

}

bool BaseParser	::	processParLiteral( TElementPtr & entity )	{

	string text = "";
	string literal = processLiteral();
	if ( literal == "" )	{
		return false;
	}

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
			text += processRepParData();
		}
	}

	Position textPosition( start, mDocText->getIndex() - start - 1, lineNr, charNr );

	mSchema->addEntity(entity->getNodeName(), textPosition);

	entity->setAttribute( "text", text );
	
	return true;
	
}

string BaseParser	::	processMinLiteral()	{
	
	string text = "";
	string literal = processLiteral();
	if ( literal == "" )	{
		return "";
	}

	bool litFound = false;
	while ( ! litFound )	{
		if ( process( literal, false ) )	{
			litFound = true;
		}
		else	{
			// Not yet found. Process replacable parameter data
			text += processMinData();
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

string BaseParser	::	processName( bool aException )	{

	string name = "";

	if ( ! isalpha( mDocText->getChar() ) )	{
		if ( aException )	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"Not a name" );
		}
		else	{
			return "";
		}
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
		return "";
	}
	
	return token;
	
}

void BaseParser	::	processSStar()	{
	
	bool sFound = true;
	while ( sFound )	{
		sFound = processS();
	}

}

bool BaseParser	::	processPs()	{

	if ( processS() )	{
		return true;
	}
	if ( processEe() )	{
		return true;
	}
	if ( processComment() )	{
		return true;
	}
	if ( processParEntityReference() )	{
		return true;
	}

	return false;
	
}

void BaseParser	::	processPsStar()	{

	bool psFound = true;
	while ( psFound )	{
		psFound = processPs();
	}
	
}

bool BaseParser	::	processPsPlus( bool aException )	{

	if ( ! processPs() )	{
		if ( aException )	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"Ps expected" );
		}
		else	{
			return false;
		}
	}
	processPsStar();
	
	return true;
	
}

bool BaseParser	::	processTs()	{

	if ( processS() )	{
		return true;
	}
	if ( processEe() )	{
		return true;
	}
	if ( processParEntityReference() )	{
		return true;
	}

	return false;
		
}

void BaseParser	::	processTsStar()	{

	bool tsFound = true;
	while ( tsFound )	{
		tsFound =	processTs();
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

string BaseParser	::	processGI( bool aException )	{

	if ( mDocText->getChar() == '/' )	{
		if ( aException )	{
			throw ReadException( mDocText->getLineNr(),
											mDocText->getCharNr(),
											"Is an end tag",
											END_TAG_FOUND );
		}
		else	{
			return "";
		}
	}

	// Generic identifier and name are equivalent
	string name = processName( false);
	if ( name == "" && aException )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"Name expected",
										GENERIC );
		
	}
	
	return name;
	
}

string BaseParser	::	processAttrValueSpec( bool aException )	{

	string attrValueSpec = processAttrValueLit();
	if ( attrValueSpec == "" )	{
		attrValueSpec = processAttrValue();
	}
	if ( attrValueSpec == "" && aException )	{
		throw ReadException( mDocText->getLineNr(),
										mDocText->getCharNr(),
										"AttrValueSpec expected" );
	}

	return attrValueSpec;
	
}

string BaseParser	::	processAttrValue()	{

	// FIX ME!!!!!!!!!!!!!!!!!!!!!
	string result = processCharData( mTagc );
	//processNameToken();
	
	return result;
	
}

string BaseParser	::	processAttrValueLit()	{

	string result = "";
	string literal = processLiteral();
	if ( literal == "" )	{
		return "";
	}

	bool litFound = false;
	while ( ! litFound )	{
		if ( process( literal, false ) )	{
			litFound = true;
		}
		else	{
			// Not yet found. Process replacable parameter data
			result += processRepCharData();
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
