#include <string.h>
#include <StorageKit.h>
#include "prefsman.h"

PrefsManager::PrefsManager(const char *PREF_FILENAME, int32 FLAGS)
{
	this->init();
	if(PREF_FILENAME != NULL)
	{
		//BEntry entry();
		int32 length = strlen(PREF_FILENAME);
		this->pref_filename = new char[length+1];
		memset((char*)this->pref_filename,0,length+1);
		strncpy((char*)this->pref_filename,PREF_FILENAME,length);
	}
}
PrefsManager::PrefsManager(entry_ref *ref)
{
	this->init();
}
PrefsManager::~PrefsManager()
{
	if( this->base_dir_entry != NULL ) delete this->base_dir_entry;
	if( this->pref_filename != NULL)
	{
		memset((char*)this->pref_filename,0,strlen(this->pref_filename)+1);
		delete this->pref_filename;
		this->pref_filename = NULL;
	}
}
void PrefsManager::init(void)
{
	this->pref_filename = NULL;
	BPath path;
	if( find_directory(B_USER_SETTINGS_DIRECTORY,&path) == B_OK)
	{
		path.Append("Themis/",true);
		this->base_dir_entry = new BEntry(path.Path(),true);
		if( this->base_dir_entry->Exists() == false)
		{
			create_directory(path.Path(),0777);
			
		}
	}
}
bool PrefsManager::createPrefsFile(const char *PREF_FILENAME, int32 FLAGS)
{
	bool file_created = false;
	return file_created;
}
bool PrefsManager::deletePrefsFile()
{
	bool file_deleted = false;
	return file_deleted;
}
BMessage *PrefsManager::loadPrefs()
{
	BMessage *prefs = NULL;
	return prefs;
}
int32 PrefsManager::savePrefs(BMessage *PREFS)
{
	int32 bytes_saved = 0;
	return bytes_saved;
}
const char *PrefsManager::filename(void)
{
	return this->pref_filename;
}
