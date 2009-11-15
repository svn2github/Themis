/*
	Copyright (c) 2009 Mark Hellegers. All Rights Reserved.
	
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
	
	Original Author: 	Mark Hellegers (mark@firedisk.net)
	Project Start Date: October 18, 2000
	Class Start Date: November 15, 2009
*/

/*	CSSParserPlugin implementation
	See CSSParserPlugin.hpp for more information
	
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <fstream>
#include <string>
#include <algorithm>
#include <ctype.h>

// Be headers
#include <Message.h>
#include <DataIO.h>

// CSSParser headers
#include "CSSParserPlugin.hpp"
#include "commondefs.h"
#include "plugman.h"
#include "PrefsDefs.h"

CSSParserPlugin * parser;
BMessage ** appSettings_p;
BMessage * appSettings;

status_t Initialize(void * aInfo) {
	
	parser = NULL;
	if (aInfo != NULL) {
		BMessage * message = (BMessage *) aInfo;
		if (message->HasPointer("settings_message_ptr")) {
			message->FindPointer("settings_message_ptr", (void **) & appSettings_p);
			appSettings = *appSettings_p;
		}
		parser = new CSSParserPlugin(message);
	}
	else {
		parser = new CSSParserPlugin();
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

CSSParserPlugin :: CSSParserPlugin(BMessage * aInfo)
		   		: BHandler("CSSParser"),
			 	  PlugClass(aInfo , "CSSParser") {
	
	mCache = (CachePlug *) PlugMan->FindPlugin(CachePlugin);
	mUserToken = 0;
	
	if (mCache != NULL) {
		mUserToken = mCache->Register(Type(), "CSS Parser");
	}

	mParser = new CSSParser();
	
	// We only support one mimetype at the momemt.
	mMimeTypes.push_back("text/css");
	
}

CSSParserPlugin :: ~CSSParserPlugin() {

	delete mParser;

}

bool CSSParserPlugin :: MessageSentByCache(BMessage * aMessage) {

	PlugClass * plug = NULL;
	aMessage->FindPointer("plugin", (void **) &plug);
	if (plug != NULL && plug->PlugID() == 'cash') {
		return true;
	}
	else {
		return false;
	}
}

void CSSParserPlugin :: RegisterCache(BMessage * aMessage) {

	PlugClass * plug = NULL;
	aMessage->FindPointer("plugin", (void **) &plug);
	if (plug != NULL) {
		mCache = (CachePlug *) plug;
		mUserToken = mCache->Register(Type(), PlugName());
	}
}

void CSSParserPlugin :: UnregisterCache(BMessage * aMessage) {

	mCache = NULL;
}

bool CSSParserPlugin :: CacheRegistered() const {
	
	return (mCache ? true : false);

}

void CSSParserPlugin :: ReplySupportedMimeTypes(BMessage * aMessage) {

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

bool CSSParserPlugin :: IsDocumentSupported(BMessage * aMessage) {
	
	bool result = false;

	const char * mimeType = NULL;
	aMessage->FindString("mimetype", &mimeType);
	if (mimeType != NULL) {
		bool mimeTypeFound = true;
		string mimeTypeString = "";
		unsigned int i = 0;
		while (mimeTypeFound && mimeType[i] != '\0') {
			if (mimeType[i] == ';') {
				mimeTypeFound = false;
			}
			else {
				mimeTypeString += mimeType[i];
			}
			i++;
		}
		vector<string>::iterator iter = find(mMimeTypes.begin(), mMimeTypes.end(), mimeTypeString);
		result = (iter != mMimeTypes.end());
	}
	
	return result;

}

void CSSParserPlugin :: NotifyParseFinished(void * aDocument,
											string aType,
											BMessage * aOriginalMessage) {

	int32 siteID = 0;
	int32 urlID = 0;
	aOriginalMessage->FindInt32("site_id", &siteID);
	aOriginalMessage->FindInt32("url_id", &urlID);

	BMessage * done = new BMessage(SH_PARSE_DOC_FINISHED);
	done->AddInt32("command", COMMAND_INFO);
	done->AddString("type", aType.c_str());
	done->AddPointer("pointer", aDocument);
	done->AddInt32("site_id", siteID);
	done->AddInt32("url_id", urlID);
	
	// Message created. Broadcast it.
	Broadcast(MS_TARGET_ALL, done);
	
	Debug("File parsed", PlugID());
	delete done;

}

void CSSParserPlugin :: ParseDocument(string aURL,
								 	  BMessage * aOriginalMessage) {
	
	if (CacheRegistered()) {
		int32 fileToken = mCache->FindObject(mUserToken, aURL.c_str());
		ssize_t fileSize = mCache->GetObjectSize(mUserToken, fileToken);
		
		if (fileSize == 0) {
			Debug("Requested file is 0 bytes long. Aborting parse", PlugID());
		}
		else {
			// Read the file and parse it.
			const unsigned int BUFFER_SIZE = 2000;
			string content = "";
			char * buffer = new char[BUFFER_SIZE];
			ssize_t bytesRead = 0;
			int totalBytes = 0;
			bool foundData = true;
			while (foundData) {
				if (totalBytes > fileSize) {
					foundData = false;
					printf("Reading more bytes than possible. Skipping last buffer\n");
				}
				else {
					bytesRead = mCache->Read(mUserToken, fileToken, buffer, BUFFER_SIZE);
					if (bytesRead > 0) {
						content += buffer;
						memset(buffer, 0, BUFFER_SIZE);
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
				mParser->parse(content);
				
				NotifyParseFinished(mDocuments.end() - 1, "cssdom", aOriginalMessage);
			}
		}
	}
	else {
		printf("Not registered with the cache. Nothing to do\n");
	}
	
}

void CSSParserPlugin :: MessageReceived(BMessage * aMessage) {

	BHandler::MessageReceived(aMessage);

}

bool CSSParserPlugin :: IsHandler() {

	return true;

}

BHandler * CSSParserPlugin :: Handler() {

	return this;

}

bool CSSParserPlugin :: IsPersistent() {

	return true;

}

uint32 CSSParserPlugin :: PlugID() {

	return 'cssp';

}

char * CSSParserPlugin :: PlugName() {

	return "CSS Parser";

}

float CSSParserPlugin :: PlugVersion() {

	return 0.2;

}

void CSSParserPlugin :: Heartbeat() {

}

status_t CSSParserPlugin :: ReceiveBroadcast(BMessage * aMessage) {

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
				case CSS_CHANGED_PARSER:	{
					Debug("Request to change base css file", PlugID());
					if (appSettings != NULL) {
						const char * path;
						appSettings->FindString(kPrefsActiveCSSPath, &path);
						string cssLoad = "Loading new CSS file: ";
						cssLoad += path;
						printf("%s\n", cssLoad.c_str());
						Debug(cssLoad.c_str(), PlugID());
						CSSStyleSheetPtr document = mParser->parse(path);
						mDocuments.push_back(document);
						NotifyParseFinished(mDocuments.end() - 1, "cssdom", aMessage);
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

status_t CSSParserPlugin :: BroadcastReply(BMessage * aMessage) {

	return B_OK;

}

uint32 CSSParserPlugin :: BroadcastTarget() {

	return MS_TARGET_CSS_PARSER;

}

int32 CSSParserPlugin :: Type() {

	return TARGET_PARSER;

}
