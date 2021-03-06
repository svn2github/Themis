/*	FileProtocol implementation
	See FileProtocol.hpp for more information
*/

// Standard C headers
#include <stdio.h>

// Standard C++ headers
#include <string>

// BeOS headers
#include <Message.h>
#include <be/storage/File.h>
#include <be/storage/NodeInfo.h>

// Themis headers
#include "commondefs.h"
#include "plugman.h"
#include "cacheplug.h"

// FileProtocol headers
#include "FileProtocol.hpp"

// Namespaces used
using namespace std;

// Constants used
const unsigned int kBufferSize = 2000;

FileProtocol * fileProt;
BMessage ** appSettings_p;
BMessage * appSettings;

status_t Initialize(void * info) {
	
	fileProt = NULL;
	if (info != NULL) {
		BMessage * message = (BMessage *) info;
		if (message->HasPointer("settings_message_ptr")) {
			message->FindPointer("settings_message_ptr", (void **) &appSettings_p);
			appSettings = *appSettings_p;
		}
		fileProt = new FileProtocol(message);
	}
	else {
		fileProt = new FileProtocol();
	}

	return B_OK;

}

status_t Shutdown(bool now) {

	delete fileProt;

	return B_OK;

}

PlugClass * GetObject() {

	return fileProt;

}

FileProtocol :: FileProtocol(BMessage * info)
			 : BHandler("FileProtocol"),
			   ProtocolPlugClass(info, "FileProtocol") {

	cache = (CachePlug *) PlugMan->FindPlugin(CachePlugin);
	userToken = 0;

	if (cache != NULL) {
		userToken = cache->Register(Type(), "File Protocol");
	}

}

FileProtocol :: ~FileProtocol() {

}

void FileProtocol :: MessageReceived(BMessage * message) {

	BHandler::MessageReceived(message);

}

bool FileProtocol :: IsHandler() {

	return true;

}

BHandler * FileProtocol :: Handler() {

	return this;

}

bool FileProtocol :: IsPersistent() {

	return true;

}

uint32 FileProtocol :: PlugID() {

	return 'file';

}

char * FileProtocol :: PlugName() {

	return "File Protocol";

}

float FileProtocol :: PlugVersion() {

	return 0.3;

}

void FileProtocol :: Heartbeat() {

}

status_t FileProtocol :: ReceiveBroadcast(BMessage * message) {

	int32 command = 0;
	message->FindInt32("command", &command);

	switch (command) {
		case COMMAND_INFO: {
			switch (message->what) {
				case PlugInLoaded: {
					PlugClass * plug = NULL;
					message->FindPointer("plugin", (void **) &plug);
					if (plug != NULL) {
						if (plug->PlugID() == 'cash') {
							cache = (CachePlug *) plug;
							userToken = cache->Register(Type(), "File Protocol");
						}
					}
					break;
				}
				case PlugInUnLoaded: {
					uint32 type = 0;
					type = message->FindInt32("type");
					if ((type & TARGET_CACHE) != 0) {
						cache = NULL;
					}
					break;
				}
			}
			break;
		}
		case COMMAND_RETRIEVE: {
			switch(message->what) {
				case SH_RETRIEVE_START: {
					const char * url = NULL;
					message->FindString("url", &url);

					if (url != NULL) {
						string urlString(url);
						string protocolString(urlString.substr( 0, 7 ));
						string fileLocation(urlString.substr(7, urlString.size() - 7));
						if (!protocolString.compare("file://")) {
							printf("FILE_PROTO: File location: %s\n", fileLocation.c_str());
							BFile file(fileLocation.c_str(), B_READ_ONLY);
							BNodeInfo fileInfo(&file);
							if (fileInfo.InitCheck() == B_OK) {
								char * mimeType = new char[255];
								status_t status = fileInfo.GetType(mimeType);
								if (status == B_OK) {
									// Put the file contents in the cache
									int32 objectToken =
										cache->CreateObject(userToken, fileLocation.c_str(), TYPE_DISK_FILE);
									
									BMessage * fileMessage = new BMessage(SH_LOADING_PROGRESS);
									int32 site_id = 0;
									int32 url_id = 0;
									message->FindInt32("site_id", &site_id);
									message->FindInt32("url_id", &url_id);
									
									fileMessage->AddInt32("site_id", site_id);
									fileMessage->AddInt32("url_id", url_id);
									fileMessage->AddInt32("command", COMMAND_INFO);
									fileMessage->AddBool("request_done", true);
									fileMessage->AddString("url", url);
									fileMessage->AddInt32("cache_object_token", objectToken);
									fileMessage->AddInt64("bytes_received", cache->GetObjectSize(userToken,objectToken));
									fileMessage->AddString("mime-type", mimeType);
									Broadcast(MS_TARGET_ALL, fileMessage);
									delete fileMessage;
								}
								else {
									printf( "FILE_PROTO: Error loading file\n" );
								}
								delete[] mimeType;
							}
							else {
								printf( "FILE_PROTO: Error loading file\n" );
							}
						}
					}
					break;
				}
			}
		}
		default: {
			return PLUG_DOESNT_HANDLE;
		}
	}

	return PLUG_HANDLE_GOOD;

}

status_t FileProtocol :: BroadcastReply(BMessage * message) {

	return B_OK;

}

uint32 FileProtocol :: BroadcastTarget() {

	return MS_TARGET_FILE_PROTOCOL;

}

int32 FileProtocol :: Type() {

	return MS_TARGET_FILE_PROTOCOL;

}

void FileProtocol :: ConnectionEstablished(connection * conn) {

}

void FileProtocol :: ConnectionDisconnected(connection * conn, uint32 reason = 0) {

}

void FileProtocol :: DataWaiting(connection * conn) {

}
