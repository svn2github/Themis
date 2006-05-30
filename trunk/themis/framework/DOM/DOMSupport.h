/* Support definitions for the DOM core

	Mark Hellegers (mark@firedisk.net)
	11-12-2001

*/

#ifndef DOMSUPPORT_H
#define DOMSUPPORT_H

// Boost headers
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

class TNode;
class TElement;
class TText;
class TNodeListContainer;
class TNodeList;
class TNamedNodeMap;
class TAttr;
class TCharacterData;
class TComment;
class TDocument;

enum	{
	ELEMENT_NODE = 1,
	ATTRIBUTE_NODE,
	TEXT_NODE,
	CDATA_SECTION_NODE,
	ENTITY_REFERENCE_NODE,
	ENTITY_NODE,
	PROCESSING_INSTRUCTION_NODE,
	COMMENT_NODE,
	DOCUMENT_NODE,
	DOCUMENT_TYPE_NODE,
	DOCUMENT_FRAGMENT_NODE,
	NOTATION_NODE
};

enum ExceptionCode	{
	INDEX_SIZE_ERR = 1,
	DOMSTRING_SIZE_ERR,
	HIERARCHY_REQUEST_ERR,
	WRONG_DOCUMENT_ERR,
	INVALID_CHARACTER_ERR,
	NO_DATA_ALLOWED_ERR,
	NO_MODIFICATION_ALLOWED_ERR,
	NOT_FOUND_ERR,
	NOT_SUPPORTED_ERR,
	INUSE_ATTRIBUTE_ERR,
	INVALID_STATE_ERR,
	SYNTAX_ERR,
	INVALID_MODIFICATION_ERR,
	NAMESPACE_ERR,
	INVALID_ACCESS_ERR,
	VALIDATION_ERR,
	TYPE_MISMATCH_ERR
};

enum TreePosition	{
	TREE_POSITION_PRECEDING = 0x01,
	TREE_POSITION_FOLLOWING = 0x02,
	TREE_POSITION_ANCESTOR = 0x04,
	TREE_POSITION_DESCENDANT = 0x08,
	TREE_POSITION_SAME = 0x10,
	TREE_POSITION_EXACT_SAME = 0x20,
	TREE_POSITION_DISCONNECTED = 0x00
};

// TDOMString is equal to string for now.
// Is not UTF-16, so must be changed!
typedef string TDOMString;

// TDOMObject is equal to void
// As far as I know, there's no Object class in C++
typedef boost::shared_ptr<void> TDOMObject;

// TDOMUserData is equal to void
// Same as for TDOMObject
typedef boost::shared_ptr<void> TDOMUserData;

// TDOMTimeStamp is equal to unsigned long long
// Should be able to store a number of milliseconds
typedef unsigned long long TDOMTimeStamp;

// Internally used codes
enum NodeChange	{
	NODE_ADDED = 1,
	NODE_REMOVED
};

// Typedefs used
typedef boost::weak_ptr<TNode> TNodeWeak;
typedef boost::shared_ptr<TNode> TNodePtr;

typedef boost::weak_ptr<TDocument> TDocumentWeak;
typedef boost::shared_ptr<TDocument> TDocumentPtr;

typedef boost::weak_ptr<TElement> TElementWeak;
typedef boost::shared_ptr<TElement> TElementPtr;

typedef boost::weak_ptr<TText> TTextWeak;
typedef boost::shared_ptr<TText> TTextPtr;

typedef boost::weak_ptr<TNamedNodeMap> TNamedNodeMapWeak;
typedef boost::shared_ptr<TNamedNodeMap> TNamedNodeMapPtr;

typedef boost::weak_ptr<TAttr> TAttrWeak;
typedef boost::shared_ptr<TAttr> TAttrPtr;

typedef boost::weak_ptr<TComment> TCommentWeak;
typedef boost::shared_ptr<TComment> TCommentPtr;

typedef boost::weak_ptr<TCharacterData> TCharacterDataWeak;
typedef boost::shared_ptr<TCharacterData> TCharacterDataPtr;

typedef boost::shared_ptr<TNodeListContainer> TNodeListContainerPtr;
typedef boost::shared_ptr<TNodeList> TNodeListPtr;

#endif
