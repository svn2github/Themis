/* Support definitions for the DOM core

	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	11-12-2001

*/

#ifndef DOMSUPPORT_H
#define DOMSUPPORT_H

#include <String.h>

#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

#include <string>

class TNode;
class TElement;
class TText;
class TNodeListContainer;
class TNodeList;
class TNamedNodeMap;
class TAttr;

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
	INVALID_ACCESS_ERR
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

// Internally used codes
enum NodeChange	{
	NODE_ADDED = 1,
	NODE_REMOVED
};

// Convenience definitions
typedef boost::weak_ptr<TNode> TNodeWeak;
typedef boost::shared_ptr<TNode> TNodeShared;

typedef boost::weak_ptr<TElement> TElementWeak;
typedef boost::shared_ptr<TElement> TElementShared;

typedef boost::weak_ptr<TText> TTextWeak;
typedef boost::shared_ptr<TText> TTextShared;

typedef boost::shared_ptr<TNodeListContainer> TNodeListContainerShared;
typedef boost::shared_ptr<TNodeList> TNodeListShared;

typedef boost::weak_ptr<TNamedNodeMap> TNamedNodeMapWeak;
typedef boost::shared_ptr<TNamedNodeMap> TNamedNodeMapShared;

typedef boost::weak_ptr<TAttr> TAttrWeak;
typedef boost::shared_ptr<TAttr> TAttrShared;

#endif
