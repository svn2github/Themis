/*
Copyright (c) 2010 Raymond Rodgers. All Rights Reserved.

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without 
restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or 
sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following 
conditions: 

   The above copyright notice and this permission notice 
   shall be included in all copies or substantial portions 
   of the Software. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Original Author & Project Manager: Raymond Rodgers (Z3R0 One) <raymond@badlucksoft.com>
Project Start Date: October 18, 2000
*/
#ifndef _prefsman_
#define _prefsman_
#include <Message.h>
#include <File.h>
#include <Entry.h>

class PrefsManager
{
	private:
		BEntry *base_dir_entry;
		entry_ref ref;
		const char *pref_filename;
		BMessage *current_prefs;
		void init(void);
	public:
		PrefsManager(const char * PREF_FILENAME = NULL, int32 FLAGS = 0);
		PrefsManager(entry_ref *ref);
		virtual ~PrefsManager();
		bool createPrefsFile(const char *PREF_FILENAME = NULL, int32 FLAGS = 0);
		bool deletePrefsFile();
		BMessage *loadPrefs();
		int32 savePrefs(BMessage *PREFS);
		const char *filename(void);
};

#endif
