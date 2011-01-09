#include <string.h>
#include <Entry.h>
#include <Path.h>
#include <Directory.h>
#include <Node.h>
#include <FindDirectory.h>
#include <File.h>
#include <stdio.h>
#include "prefsman.h"

PrefsManager::PrefsManager(const char *PREF_FILENAME, int32 FLAGS)
{
	this->init();
	if(PREF_FILENAME != NULL)
	{
		//BEntry entry();
		int32 length = strlen(PREF_FILENAME);
		strncpy((char*)this->pref_filename,PREF_FILENAME,min_c(length,B_FILE_NAME_LENGTH));
		BPath settingsFilePath(this->base_dir_entry);
		settingsFilePath.Append(this->pref_filename,false);
		BEntry settingsFileEntry(settingsFilePath.Path(),true);
//		printf("Settings File Path: %s\tExists? %s\n",settingsFilePath.Path(),settingsFileEntry.Exists()?"yes":"no");
		status_t error = get_ref_for_path(settingsFilePath.Path(), &this->ref);
		BPath testPath(&this->ref);
		//printf("Test result: %s\n",testPath.Path());
		if( (FLAGS & PrefsManager::FLAG_CREATE_FILE) != 0 )
		{
			this->createPrefsFile(PREF_FILENAME,FLAGS);
		}
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
		memset((char*)this->pref_filename,0,B_FILE_NAME_LENGTH+1);
		delete this->pref_filename;
		this->pref_filename = NULL;
	}
}
void PrefsManager::init(void)
{
	this->pref_filename = new char[B_FILE_NAME_LENGTH+1];
	memset((char*)this->pref_filename,0,B_FILE_NAME_LENGTH+1);
	BPath path;
	if( find_directory(B_USER_SETTINGS_DIRECTORY,&path) == B_OK)
	{
		path.Append("Themis/",true);
		this->base_dir_entry = new BEntry(path.Path(),true);
		if( this->base_dir_entry->Exists() == false)
		{
			create_directory(path.Path(),0644);
			
		}
	}
}
status_t PrefsManager::createPrefsFile(const char *PREF_FILENAME, int32 FLAGS)
{
	status_t file_created = B_ERROR;
	if( PREF_FILENAME != NULL && strlen(PREF_FILENAME) > 0 )
	{
		BPath prefsDirectory(this->base_dir_entry);
		prefsDirectory.Append(PREF_FILENAME,false);
		entry_ref pref_ref;
		get_ref_for_path(prefsDirectory.Path(), &pref_ref);
		prefsDirectory.Unset();
		int32 flags = B_WRITE_ONLY|B_CREATE_FILE|B_FILE_EXISTS;
		if((FLAGS & PrefsManager::FLAG_REPLACE_FILE) != 0 ) flags |= B_ERASE_FILE|~B_FILE_EXISTS;
		BFile file;
		file_created = file.SetTo(&pref_ref,flags);
		file.Unset();
	}
	return file_created;
}
status_t PrefsManager::deletePrefsFile()
{
	status_t file_deleted = false;
	BEntry prefsFile(&this->ref,true);
	if( prefsFile.Exists() ) file_deleted = prefsFile.Remove();
	else file_deleted = B_ENTRY_NOT_FOUND;
	prefsFile.Unset();
	return file_deleted;
}
BMessage *PrefsManager::loadPrefs(status_t *RESULT)
{
	BMessage *prefs = NULL;
	status_t result = B_OK;
	BEntry prefsFileEntry(&this->ref,true);
	if( prefsFileEntry.Exists() )
	{
		BFile prefsFile(&this->ref,B_READ_ONLY);
		BMessage *tmpPrefs = new BMessage();
		off_t size = 0;
		prefsFile.GetSize(&size);
		if( size > 0L )
		{
			result = tmpPrefs->Unflatten(&prefsFile);
			if( result == B_OK) prefs = tmpPrefs;
			else delete tmpPrefs;
		}
		prefsFile.Unset();
	}
	if( RESULT != NULL) *RESULT = result;
	prefsFileEntry.Unset();
	return prefs;
}
ssize_t PrefsManager::savePrefs(BMessage *PREFS,status_t *RESULT)
{
	ssize_t bytes_saved = 0;
	BEntry prefsFileEntry(&this->ref,true);
	if( prefsFileEntry.Exists() == false ) this->createPrefsFile(this->pref_filename);
	BFile prefsFile(&prefsFileEntry,B_WRITE_ONLY|B_ERASE_FILE);
	if( prefsFile.IsWritable() )
	{
		status_t result;
		result = PREFS->Flatten(&prefsFile,&bytes_saved);
		if( RESULT != NULL ) *RESULT = result;
		if( result != B_OK) bytes_saved = 0;
	}
	else if( RESULT != NULL ) *RESULT = B_NOT_ALLOWED;
	return bytes_saved;
}
void PrefsManager::setFilename(const char *FILENAME)
{
	if(this->pref_filename != NULL)
	{
		memset((char*)this->pref_filename,0,B_FILE_NAME_LENGTH+1);
	}
	strncpy((char*)this->pref_filename,FILENAME,min_c(strlen(FILENAME),B_FILE_NAME_LENGTH));
}
status_t PrefsManager::renameTo(const char *NEW_FILENAME, bool CLOBBER)
{
	status_t result = B_ERROR;
	BEntry prefsFileEntry(&this->ref,true);
	if( prefsFileEntry.Exists() )
	{
		if( NEW_FILENAME != NULL && strlen(NEW_FILENAME ) > 0 )
		{
			 result = prefsFileEntry.Rename(NEW_FILENAME,CLOBBER);
			 this->setFilename(NEW_FILENAME);
		}
	}
	return result;
}
const char *PrefsManager::filename(void)
{
	return this->pref_filename;
}
const char *PrefsManager::fullPath(void)
{
	BPath path(&this->ref);
	return path.Path();
}
