/*	DOMTest. Developed for ThemisDOM.
	Tests the DOM to see if it works correctly.
	
	Mark Hellegers (M.H.Hellegers@stud.tue.nl)
	26-12-2001
*/

#include <Application.h>

#include <string.h>
#include <stdio.h>

#include "DOMSupport.h"

class DOMTest	:	public BApplication	{
	
	private:
		void showDocument( const TDocumentShared document );
		void showTree( const TNodeShared node, int spacing );
	
	public:
		DOMTest();
		
};
