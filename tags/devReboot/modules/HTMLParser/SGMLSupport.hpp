/*	Support definitions for the SGML Parser

	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-04-2003
	
*/

#ifndef SGMLSUPPORT_HPP
#define SGMLSUPPORT_HPP

// Standard C++ headers
#include <string>

// Namespaces used
using std::string;

// Convenience variables
const string kENTITY	= "ENTITY";
const string kENTITIES	= "ENTITIES";
const string kID		= "ID";
const string kIDREF		= "IDREF";
const string kIDREFS	= "IDREFS";
const string kNAME		= "NAME";
const string kNAMES		= "NAMES";
const string kNMTOKEN	= "NMTOKEN";
const string kNMTOKENS	= "NMTOKENS";
const string kNUMBER	= "NUMBER";
const string kNUMBERS	= "NUMBERS";
const string kNUTOKEN	= "NUTOKEN";
const string kNUTOKENS	= "NUTOKENS";
const string kELEMENT	= "ELEMENT";
const string kATTLIST	= "ATTLIST";
const string kCDATA		= "CDATA";
const string kPCDATA	= "PCDATA";
const string kSDATA		= "SDATA";
const string kRCDATA	= "RCDATA";
const string kEMPTY		= "EMPTY";
const string kPI		= "PI";
const string kANY		= "ANY";
const string kO			= "O";
const string kMinus		= "-";
const string kFIXED		= "FIXED";
const string kREQUIRED	= "REQUIRED";
const string kCURRENT	= "CURRENT";
const string kCONREF	= "CONREF";
const string kIMPLIED	= "IMPLIED";
const string kIGNORE	= "IGNORE";
const string kINCLUDE	= "INCLUDE";
const string kTEMP		= "TEMP";
const string kSYSTEM	= "SYSTEM";
const string kPUBLIC	= "PUBLIC";
const string kDOCTYPE	= "DOCTYPE";

// Boost headers
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

// Class declarations
class TElementDeclaration;
class TSchema;
class TSchemaRule;

// Typedefs used
typedef boost::weak_ptr<TElementDeclaration> TElementDeclarationWeak;
typedef boost::shared_ptr<TElementDeclaration> TElementDeclarationPtr;

typedef boost::weak_ptr<TSchema> TSchemaWeak;
typedef boost::shared_ptr<TSchema> TSchemaPtr;

typedef boost::weak_ptr<TSchemaRule> TSchemaRuleWeak;
typedef boost::shared_ptr<TSchemaRule> TSchemaRulePtr;

#endif
