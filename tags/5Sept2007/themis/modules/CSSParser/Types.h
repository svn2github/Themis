#ifndef TYPES_H
#define TYPES_H
/* Types.h - general definitions
**
** Copyright 2001 pinc Software. All Rights Reserved.
*/

#ifdef __BEOS__
#	include <SupportDefs.h>

enum	/* return codes */
{
	RC_ERROR_BASE = B_GENERAL_ERROR_BASE,

	RC_NO_MEMORY = B_NO_MEMORY,
	RC_IO_ERROR = B_IO_ERROR,
	RC_PERMISSION_DENIED = B_PERMISSION_DENIED,
	RC_BAD_INDEX = B_BAD_INDEX,
	RC_BAD_TYPE = B_BAD_TYPE,
	RC_BAD_VALUE = B_BAD_VALUE,
	RC_MISMATCHED_VALUES = B_MISMATCHED_VALUES,
	RC_NAME_NOT_FOUND = B_NAME_NOT_FOUND,
	RC_NAME_IN_USE = B_NAME_IN_USE,
	RC_TIMED_OUT = B_TIMED_OUT,
    RC_INTERRUPTED = B_INTERRUPTED,
	RC_WOULD_BLOCK = B_WOULD_BLOCK,
    RC_CANCELED = B_CANCELED,
	RC_NO_INIT = B_NO_INIT,
	RC_BUSY = B_BUSY,
	RC_NOT_ALLOWED = B_NOT_ALLOWED,

	RC_ERROR = B_ERROR,
	RC_OK = B_OK,
	RC_NO_ERROR = B_OK
};
#else	/* !__BEOS__ */
#	error "No BeOS, no SupportDefs.h"
#	include <limits.h>

#	define RC_ERROR_BASE	LONG_MIN
enum	/* return codes */
{
	RC_NO_MEMORY = RC_ERROR_BASE,
	RC_IO_ERROR,					
	RC_PERMISSION_DENIED,		
	RC_BAD_INDEX,				
	RC_BAD_TYPE,					
	RC_BAD_VALUE,				
	RC_MISMATCHED_VALUES,		
	RC_NAME_NOT_FOUND,			
	RC_NAME_IN_USE,			
	RC_TIMED_OUT,			
    RC_INTERRUPTED,           
	RC_WOULD_BLOCK,    
    RC_CANCELED,          
	RC_NO_INIT,			
	RC_BUSY,					
	RC_NOT_ALLOWED,				

	RC_ERROR = -1,				
	RC_OK = 0,
	RC_NO_ERROR = 0
};
#endif	/* __BEOS__ */

// C++ "Enhancement"
#define abstract = 0

#endif	/* TYPES_H */
