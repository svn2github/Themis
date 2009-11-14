/*	DOMTest. Developed for ThemisDOM.
	Tests the DOM to see if it works correctly.
	
	Mark Hellegers (mark@firedisk.net)
	26-12-2001
*/

#include <Application.h>

#include <string.h>
#include <stdio.h>

#include "DOMSupport.h"

class DOMTest	:	public BApplication	{
	
	private:
		void showDocument( const TDocumentPtr document );
		void showTree( const TNodePtr node, int spacing );
	
	public:
		DOMTest();
		
};
