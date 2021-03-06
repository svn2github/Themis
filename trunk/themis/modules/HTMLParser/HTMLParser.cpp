/* HTMLParser implementation
	See HTMLParser.h for some more information
*/

// Standard C headers
#include <stdio.h>
#include <ctype.h>

// Standard C++ headers
#include <fstream>
#include <string>
#include <algorithm>
#include <ctype.h>

// Be headers
#include <Message.h>
#include <DataIO.h>
#include <storage/Directory.h>
#include <storage/Path.h>

// HTMLParser headers
#include "HTMLParser.h"

// DOM headers
#include "TDocument.h"

// SGMLParser headers
#include "SGMLText.hpp"
#include "TSchema.hpp"
#include "SGMLScanner.hpp"
#include "HTMLParserPrefsView.hpp"

// Themis headers
#include "framework/app.h"
#include "framework/SiteHandler.h"
#include "commondefs.h"
#include "plugman.h"
#include "PrefsDefs.h"
#include "DOMEntry.hpp"

HTMLParser * parser;
BMessage ** appSettings_p;
BMessage * appSettings;

// Constants used
const char* const kPreferredDTD = "HTML.4.0.Transitional.DTD";


status_t Initialize(void * aInfo) {
	
	parser = NULL;
	if (aInfo != NULL) {
		BMessage * message = (BMessage *) aInfo;
		if (message->HasPointer("settings_message_ptr")) {
			message->FindPointer("settings_message_ptr", (void **) & appSettings_p);
			appSettings = *appSettings_p;
		}
		parser = new HTMLParser(message);
	}
	else {
		parser = new HTMLParser();
	}

	return B_OK;

}

status_t Shutdown(bool aNow) {

	delete parser;

	return B_OK;

}

PlugClass * GetObject()	{

	return parser;

}

HTMLParser :: HTMLParser(BMessage * aInfo)
		   : BHandler("HTMLParser"),
			 PlugClass(aInfo , "HTMLParser") {
	
	mCache = (CachePlug *) PlugMan->FindPlugin(CachePlugin);
	mUserToken = 0;
	
	if (mCache != NULL) {
		mUserToken = mCache->Register(Type(), "HTML Parser");
	}

	TSchemaPtr schema = TSchemaPtr(new TSchema());
	schema->setup();
	
	SGMLScanner * scanner = new SGMLScanner();
	mParser = new SGMLParser(scanner, schema);
	
	// We only support one mimetype at the momemt.
	mMimeTypes.push_back("text/html");
	
	// Check the settings...
	if(!appSettings->HasString(kPrefsDTDDirectory)) {
		BString dir;
		AppSettings->FindString(kPrefsSettingsDirectory, &dir);
		dir.Append("/dtd");
		AppSettings->AddString(kPrefsDTDDirectory, dir.String());
		BEntry ent(dir.String(), true);
		if (!ent.Exists()) {
			create_directory(dir.String(), 0555);
		}
	}
	
}

HTMLParser :: ~HTMLParser() {

	delete mParser;

}

bool HTMLParser :: MessageSentByCache(BMessage * aMessage) {

	PlugClass * plug = NULL;
	aMessage->FindPointer("plugin", (void **) &plug);
	if (plug != NULL && plug->PlugID() == 'cash') {
		return true;
	}
	else {
		return false;
	}
}

void HTMLParser :: RegisterCache(BMessage * aMessage) {

	PlugClass * plug = NULL;
	aMessage->FindPointer("plugin", (void **) &plug);
	if (plug != NULL) {
		mCache = (CachePlug *) plug;
		mUserToken = mCache->Register(Type(), PlugName());
	}
}

void HTMLParser :: UnregisterCache(BMessage * aMessage) {

	mCache = NULL;
}

bool HTMLParser :: CacheRegistered() const {
	
	return (mCache ? true : false);

}

void HTMLParser :: ReplySupportedMimeTypes(BMessage * aMessage) {

	if (aMessage->HasBool( "supportedmimetypes" )) {
		BMessage types(SupportedMIMEType);
		unsigned int length = mMimeTypes.size();
		for (unsigned int i = 0; i < length; i++) {
			types.AddString("mimetype", mMimeTypes[i].c_str());
		}
		types.AddInt32("command", COMMAND_INFO);
		PlugClass * plug = NULL;
		if (aMessage->HasPointer("ReplyToPointer")) {
			aMessage->FindPointer("ReplyToPointer", (void**) &plug);
			if (plug != NULL) {
				plug->BroadcastReply(&types);
			}
		}
	}
}

string HTMLParser :: GetDTDPathFromSettings() const {

	string result = "";
	const char * dtdPath = NULL;
	
	if (appSettings != NULL) {
		status_t found = appSettings->FindString(kPrefsActiveDTDPath, &dtdPath);
		if (found != B_OK) {
			// Not set yet, find one in the DTD directory.
			BString dtdDir;
			appSettings->FindString(kPrefsSettingsDirectory, &dtdDir);
			dtdDir.Append("/dtd/");
					
			BDirectory dir(dtdDir.String());
			if(dir.InitCheck() != B_OK) {
				printf("DTD directory (%s) not found!\n", dtdDir.String());
				result = "";
			}
			else {
				BEntry entry;
				char name[B_FILE_NAME_LENGTH];
				bool foundPreferredDTD = false;
				while (dir.GetNextEntry(&entry, false) != B_ENTRY_NOT_FOUND && !foundPreferredDTD) {
					BPath path;
					entry.GetPath(&path);
					entry.GetName(name);
					BString dtd = name;
					if (dtd.Compare(kPreferredDTD) == 0)
						// This is our preferred DTD, use it.
						foundPreferredDTD = true;
				}
				dtdDir.Append(name);
				// Add it to the settings, so we don't have to do this the next time.
				appSettings->AddString(kPrefsActiveDTDPath, dtdDir.String());
				// Convert to a C++ string to return
				result = dtdDir.String();
			}
		}
		else {
			result = dtdPath;
		}
	}

	return result;

}

bool HTMLParser :: IsDocumentSupported(BMessage * aMessage) {
	
	bool result = false;

	const char * mimeType = NULL;
	aMessage->FindString("mime-type", &mimeType);
	if (mimeType != NULL) {
		bool mimeTypeFound = true;
		string mimeTypeString = "";
		unsigned int i = 0;
		while (mimeTypeFound && mimeType[i] != '\0') {
			if (mimeType[i] == ';') {
				mimeTypeFound = false;
			}
			else {
				mimeTypeString += tolower(mimeType[i]);
			}
			i++;
		}
		vector<string>::iterator iter = find(mMimeTypes.begin(), mMimeTypes.end(), mimeTypeString);
		result = (iter != mMimeTypes.end());
	}
	
	return result;

}

void HTMLParser :: NotifyParseFinished(TDocumentPtr aDocument,
									   string aType,
									   BMessage * aOriginalMessage) {

	int32 siteId = 0;
	int32 urlId = 0;
	aOriginalMessage->FindInt32("site_id", &siteId);
	aOriginalMessage->FindInt32("url_id", &urlId);

	/* Get an unique ID from the app for the DOM entry */
	int32 domId = ((App *)be_app)->GetNewID();
	DOMEntry * entry = new DOMEntry(domId, aDocument);
	((App *)be_app)->GetSiteHandler()->AddEntry(entry, siteId, urlId);

	BMessage * done = new BMessage(SH_PARSE_DOC_FINISHED);
	done->AddInt32("command", COMMAND_INFO);
	done->AddString("type", aType.c_str());
	done->AddInt32("site_id", siteId);
	done->AddInt32("url_id", urlId);
	done->AddInt32("dom_id", domId);

	
	// Message created. Broadcast it.
	Broadcast(MS_TARGET_ALL, done);
	
	Debug("File parsed", PlugID());
	delete done;

}

void HTMLParser :: ParseDocument(string aURL,
								 BMessage * aOriginalMessage) {
	
	if (CacheRegistered()) {
		int32 fileToken = 0;
		aOriginalMessage->FindInt32("cache_object_token", &fileToken);
		ssize_t fileSize = mCache->GetObjectSize(mUserToken, fileToken);
		
		if (fileSize == 0) {
			Debug("Requested file is 0 bytes long. Aborting parse", PlugID());
		}
		else {
			// Read the file and parse it.
			const unsigned int BUFFER_SIZE = 2000;
			string content = "";
			char * buffer = new char[BUFFER_SIZE + 1];
			memset(buffer, 0, BUFFER_SIZE + 1);
			ssize_t bytesRead = 0;
			int totalBytes = 0;
			bool foundData = true;
			bool resetPosition = true;
			while (foundData) {
				if (totalBytes > fileSize) {
					foundData = false;
					printf("Reading more bytes than possible. Skipping last buffer\n");
				}
				else {
					bytesRead = mCache->Read(mUserToken,
											 fileToken,
											 buffer,
											 BUFFER_SIZE,
											 resetPosition);
					// Make sure we are not reading the same part over and over.
					resetPosition = false;
					if (bytesRead > 0) {
						content += buffer;
						memset(buffer, 0, BUFFER_SIZE + 1);
						totalBytes += bytesRead;
					}
					else {
						foundData = false;
					}
				}
			}
			delete[] buffer;
			
			// Parse it
			if (content.size() != 0) {
				if (mActiveDTDPath == "") {
					mActiveDTDPath = GetDTDPathFromSettings();
				}
				TDocumentPtr document = mParser->parse(mActiveDTDPath.c_str(), content);
				document->setDocumentURI(aURL);
				mDocuments.push_back(document);
				
				NotifyParseFinished(document, "dom", aOriginalMessage);
			}
		}
	}
	else {
		printf("Not registered with the cache. Nothing to do\n");
	}
	
}

void HTMLParser :: MessageReceived(BMessage * aMessage) {

	switch (aMessage->what) {
		case DTD_CHANGED_PARSER: {
			Debug("Request to change parser", PlugID());
			BString dtdString;
			aMessage->FindString("DTDFileString", &dtdString);
			if (appSettings->HasString(kPrefsActiveDTDPath)) {
				appSettings->ReplaceString(kPrefsActiveDTDPath, dtdString.String());
			}
			else {
				appSettings->AddString(kPrefsActiveDTDPath, dtdString.String());
			}
			mActiveDTDPath = dtdString.String();
			if (mActiveDTDPath != "") {
				string dtdLoad = "Loading new DTD: ";
				dtdLoad += mActiveDTDPath;
				Debug(dtdLoad.c_str(), PlugID());
				TSchemaPtr schema = TSchemaPtr(new TSchema());
				schema->setup();
				
				SGMLScanner * scanner = new SGMLScanner();
				mParser = new SGMLParser(scanner, schema);
				mParser->loadSchema(mActiveDTDPath.c_str());
				Debug("New DTD loaded", PlugID());
			}
			break;
		}
		default: {
			BHandler::MessageReceived(aMessage);
		}
	}
	

}

bool HTMLParser :: IsHandler() {

	return true;

}

BHandler * HTMLParser :: Handler() {

	return this;

}

bool HTMLParser :: IsPersistent() {

	return true;

}

uint32 HTMLParser :: PlugID() {

	return 'html';

}

char * HTMLParser :: PlugName() {

	return "HTML Parser";

}

float HTMLParser :: PlugVersion() {

	return 0.7;

}

void HTMLParser :: Heartbeat() {

}

status_t HTMLParser :: ReceiveBroadcast(BMessage * aMessage) {

	int32 command = 0;
	aMessage->FindInt32("command", &command);

	switch (command) {
		case COMMAND_INFO_REQUEST: {
			switch (aMessage->what) {
				case GetSupportedMIMEType: {
					ReplySupportedMimeTypes(aMessage);
					break;
				}
			}
		}	
		case COMMAND_INFO: {
			switch (aMessage->what) {
				case PlugInLoaded: {
					if (MessageSentByCache(aMessage)) {
						RegisterCache(aMessage);
					}
					break;
				}
				case PlugInUnLoaded: {
					if (MessageSentByCache(aMessage)) {
						UnregisterCache(aMessage);
					}
					break;
				}
				case SH_LOADING_PROGRESS: {
					if (IsDocumentSupported(aMessage)) {
						bool requestDone;
						aMessage->FindBool("request_done", &requestDone);
						if (requestDone) {
							const char * url = NULL;
							aMessage->FindString("url", &url);
							if (url == NULL) {
								// What the heck
								printf("HTMLPARSER: Aborting. No url specified.\n");
							}
							else {
								ParseDocument(url, aMessage);
							}
						}
					}
					break;
				}
			}
			break;
		}
		default:	{
			return PLUG_DOESNT_HANDLE;
		}
	}

	return PLUG_HANDLE_GOOD;

}

status_t HTMLParser :: BroadcastReply(BMessage * aMessage) {

	return B_OK;

}

uint32 HTMLParser :: BroadcastTarget() {

	return MS_TARGET_HTML_PARSER;

}

int32 HTMLParser :: Type() {

	return TARGET_PARSER;

}

char * HTMLParser :: SettingsViewLabel() {
	
	return "HTML Parser";
}

BView * HTMLParser :: SettingsView(BRect aFrame) {
	
	HTMLParserPrefsView * view = new HTMLParserPrefsView(
		aFrame,
		"HTML Parser",
		this);
	
	return view;
	
}
