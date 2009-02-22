/*
Copyright (c) 2004 Raymond "Z3R0 One" Rodgers. All Rights Reserved. 

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

Original Author & Project Manager: Raymond "Z3R0 One" Rodgers (z3r0_one@users.sourceforge.net)
Project Start Date: October 18, 2000
*/
#include "httpv4.h"
#include "cookieman.h"
#include "cacheplug.h"
#include "commondefs.h"
#include <stdio.h>
#include <Path.h>
#include <ctype.h>
#include <Autolock.h>
#include <String.h>

#define PlugIDdef 'http'
#define PlugNamedef "Network (http)"
#define PlugVersdef .60

extern BMessage *HTTPSettings;

//!The size of the receive buffer.
#define BUFFER_SIZE 10240

extern HTTPv4 *HTTP;
using namespace _Themis_Networking_;
header_info_st::~header_info_st()
{
	if (attribute!=NULL)
	{
		memset((char*)attribute,0,strlen(attribute)+1);
		delete attribute;
	}
	attribute=NULL;
	if (value!=NULL)
	{
		memset((char*)value,0,strlen(value)+1);
		delete value;
	}
	value=NULL;
	next=NULL;
}
http_request_info_st::~http_request_info_st()
{
	printf("http_request_info_st %p is being deleted now!\n",this);
	if (post_data!=NULL)
		delete post_data;
	post_data=NULL;
	if (connection!=NULL)
		connection->OwnerRelease();
	connection=NULL;
	http_status_code=0;
	internal_status=0;
	if (gzip_info!=NULL)
		delete gzip_info;
	if (header_buffer!=NULL)
	{
		memset((char*)header_buffer,0,strlen(header_buffer)+1);
		free(header_buffer);
	}
	header_buffer=NULL;
	if (temporary_data_buffer!=NULL)
	{
		memset(temporary_data_buffer,0,temporary_data_size);
		free(temporary_data_buffer);
	}
	temporary_data_buffer=NULL;
	temporary_data_size=0L;
	if (http_status_message!=NULL)
	{
		memset((char*)http_status_message,0,strlen(http_status_message)+1);
		delete http_status_message;
	}
	http_status_message=NULL;
	if (url!=NULL)
	{
		memset((char*)url,0,strlen(url)+1);
		delete url;
	}
	url=NULL;
	if (uri!=NULL)
	{
		memset((char*)uri,0,strlen(uri)+1);
		delete uri;
	}
	uri=NULL;
	if (host!=NULL)
	{
		memset((char*)host,0,strlen(host)+1);
		delete host;
	}
	host=NULL;
	if (referring_url!=NULL)
	{
		memset((char*)referring_url,0,strlen(referring_url)+1);
		delete referring_url;
	}
	if (header_list!=NULL)
	{
		header_info_st *next;
		while (header_list!=NULL)
		{
			next=header_list->next;
			delete header_list;
			header_list=next;
		}
	}
	header_list=NULL;
	referring_url=NULL;
	port=0;
	secure=false;
	cached=false;
	cache_system_type=0;
	cache_object_token=-1;
	url_id=0;
	site_id=0;
	if (auth_info!=NULL)
		delete auth_info;
	next=NULL;
	printf("~http_request_info_st end\n");
}



HTTPv4::HTTPv4(BMessage *info, const char *msg_sys_name)
	:ProtocolPlugClass(info,msg_sys_name)
{
	Init();
	HTTP=this;
	uses_heartbeat=true;
	request_queue=new BList();
	if (info!=NULL)
	{
		info->FindPointer("settings_message_ptr",(void**)&AppSettings_p);
		if (AppSettings_p!=NULL)
			AppSettings=*AppSettings_p;
		info->FindPointer("plug_manager",(void**)&PluginManager);
		if ((CacheSystem=(CachePlug*)PluginManager->FindPlugin(CachePlugin))!=NULL)
			cache_user_token=CacheSystem->Register(Type(),"HTTP Protocol Add-on");
		else
			Debug("Cache System not loaded; will look for it later.");
		if (info->HasMessage("httpv4_settings"))
		{
			info->FindMessage("httpv4_settings",0,HTTPSettings);
		}
		//Should we use a proxy server?
		if (HTTPSettings->HasBool("use_proxy_server"))
		{
			HTTPSettings->FindBool("use_proxy_server",&use_proxy_server);
			if (use_proxy_server)
			{
				if (HTTPSettings->HasString("proxy_server_host"))
					HTTPSettings->FindString("proxy_server_host",&proxy_server_host);
				else
				{
					HTTPSettings->AddString("proxy_server_host",NULL);
					proxy_server_host=NULL;
				}
				if (proxy_server_host==NULL)
				{
					use_proxy_server=false;
					Debug("Use of a proxy server has been requested, however no proxy server address was supplied. Themis will attempt to access servers directly.");
				}
				if (HTTPSettings->HasInt16("proxy_server_port"))
					HTTPSettings->FindInt16("proxy_server_port",(int16*)&proxy_server_port);
				else
					HTTPSettings->AddInt16("proxy_server_port",0);
			}
		}
		else
		{
			HTTPSettings->AddBool("use_proxy_server",false);
		}
		if (HTTPSettings->HasBool("enable_cookies"))
			HTTPSettings->FindBool("enable_cookies",&enable_cookies);
		else
			HTTPSettings->AddBool("enable_cookies",true);	
		if (HTTPSettings->HasInt32("use_useragent"))
		{
			HTTPSettings->FindInt32("use_useragent",(int32*)&use_useragent);
			if (use_useragent==USER_AGENT_CUSTOM)
			{
				HTTPSettings->FindString("custom_useragent",&custom_useragent);
			}
		}
		else
		{
			HTTPSettings->AddInt32("use_useragent",0);
		}
		info->FindPointer("tcp_manager",(void**)&tcp_manager);
		CookieMonster=new CookieManager();
		auth_manager=new AuthManager(this,AuthManager::AUTHENTICATION_TYPE_ALL);
		Debug("Starting layer manager.");
		layer_manager_thid=spawn_thread(start_layer_manager,"HTTP Layer Manager",B_LOW_PRIORITY,this);
		if (resume_thread(layer_manager_thid)!=B_OK)
			Debug("There's been a problem starting the layer_manager.");
	}
	else
		Debug("Information BMessage is NULL; HTTP is essentially dead.");
	
}
HTTPv4::~HTTPv4()
{
	if (http_request_list!=NULL)
	{
		http_request_info_st *next;
		while(http_request_list!=NULL)
		{
			next=http_request_list->next;
			delete http_request_list;
			http_request_list=next;
		}
	}
	if (!request_queue->IsEmpty())
	{
		build_request_st *item;
		while (!request_queue->IsEmpty())
		{
			item=(build_request_st*)request_queue->RemoveItem(0L);
			memset((char*)item->url,0,strlen(item->url)+1);
			delete item->url;
			delete item;
		}
		
	}
	delete request_queue;
	if (CookieMonster!=NULL)
		delete CookieMonster;
	if (auth_manager!=NULL)
		delete auth_manager;
	if (proxy_auth_info!=NULL)
	{
		delete proxy_auth_info->auth_object;
		delete proxy_auth_info;
	}
		
}
void HTTPv4::Init()
{
	enable_cookies=true;
	proxy_auth_info=NULL;
	proxy_auth_win=NULL;
	AppSettings=NULL;
	AppSettings_p=NULL;
	smthead=NULL;
	http_request_list=NULL;
	build_request_thid=0;
	layer_manager_thid=0;
	request_queue=NULL;
	HTTP=NULL;
	cache_user_token=0;
	CookieMonster=NULL;
	auth_manager=NULL;
	use_useragent=0;
	CacheSystem=NULL;
	PluginManager=NULL;
	pipelining=true;
	_terminate_=0;
	use_proxy_server=false;
	proxy_server_host=NULL;
	proxy_server_port=0;//Squid Proxy server's default port is 3128
	send_referral_info=true;
}
char *HTTPv4::PlugName(void)
{
	return PlugNamedef;
}
uint32 HTTPv4::PlugID(void)
{
	return PlugIDdef;
}
float HTTPv4::PlugVersion(void)
{
	return 4.0;
	
}
int32 HTTPv4::Type() 
{
	return HTTP_PROTOCOL;
}
status_t HTTPv4::Go(void)
{
	return B_OK;
}

void HTTPv4::Heartbeat(void)
{
//	Debug("Heartbeat");
//	printf("Heartbeat\n");
	CookieMonster->ClearExpiredCookies();
//	CookieMonster->PrintCookies();
	if (!request_queue->IsEmpty())
		if (build_request_thid==0)
		{
			status_t status;		
			while (!request_queue->IsEmpty())
			{	
				build_request_st *bri=(build_request_st*)request_queue->RemoveItem(0L);
				if (build_request_thid!=0)
					wait_for_thread(build_request_thid,&status);
				build_request_thid=spawn_thread(BuildRequest_th,"http - build request", B_LOW_PRIORITY,bri);
				printf("HTTPv4 (Heartbeat):\tbuild request thread id %ld\n",build_request_thid);
				
				resume_thread(build_request_thid);
				wait_for_thread(build_request_thid,&status);
				build_request_thid=0;
				printf("HTTPv4 (Heartbeat): build request thread completed\n");
			}
			printf("HTTPv4 Heartbeat: No more work.\n");
		}
	thread_info info;
	status_t status=B_OK;
	if ((status=get_thread_info(layer_manager_thid,&info))!=B_OK)
	{
		//something has happened to our thread!
		if (_terminate_==0)
		{//the thread terminated abnormally... hmmm... restart it!
			Debug("HTTPv4: Layer manager thread exited abnormally (0x%x); restarting it.\n",status);
			printf("HTTPv4: Layer manager thread exited abnormally; restarting it.\n");
			if (request_lock.IsLocked())
				request_lock.Unlock();
			
			layer_manager_thid=spawn_thread(start_layer_manager,"HTTP Layer Manager",B_LOW_PRIORITY,this);
			if (resume_thread(layer_manager_thid)!=B_OK)
				Debug("There's been an error restarting the layer manager thread.");
		}
	} //else
	//	printf("HTTPv4 layer manager seems to be running\n");
}
void HTTPv4::ResubmitRequest(http_request_info_st *request,const char *alt_url,int32 request_method)
{
	printf("Resubmit request: alternate URL: %s\n",alt_url);
	build_request_st *bri=new build_request_st();
	bri->redirection_counter=request->redirection_counter;
	bri->url_id=request->url_id;
	bri->site_id=request->site_id;
	if (request_method!=-1)
		bri->request_method=request->request_method;
	else
		bri->request_method=request_method;
	const char *url=NULL;
	if (alt_url!=NULL)
	{
		if ((strncasecmp("http:",alt_url,5)==0) || (strncasecmp("https:",alt_url,6)==0))
			url=alt_url;
		else
		{//url appears to be relative...
			if (alt_url[0]=='/')
			{
				BString new_url="http";
				
				if (request->secure)
					new_url<<"s";
				new_url<<"://"<<request->host;
				if ((request->secure && request->port!=443)|| (!request->secure && request->port!=80))
					new_url<<":"<<(int32)request->port;
				new_url<<alt_url;
				delete request->url;
				request->url=new char[new_url.Length()+1];
				memset((char*)request->url,0,new_url.Length()+1);
				strcpy((char*)request->url,new_url.String());
				url=request->url;
				printf("redirecting to %s\n",url);
			}
			else
			{
				BString new_url="http";
				
				if (request->secure)
					new_url<<"s";
				new_url<<"://"<<request->host<<":"<<(int32)request->port;
				BPath path(request->uri);
				if (request->uri[strlen(request->uri)-1]=='/')
				{
					path.Append(alt_url);
				}
				else
				{
					path.GetParent(&path);
					path.Append(alt_url);
				}
				new_url<<path.Path();
				delete request->url;
				request->url=new char[new_url.Length()+1];
				memset((char*)request->url,0,new_url.Length()+1);
				strcpy((char*)request->url,new_url.String());
				url=request->url;
			}
		}
	}
	else
		url=request->url;
	bri->url=new char[1+strlen(url)];
	memset((char*)bri->url,0,strlen(url)+1);
	strcpy((char*)bri->url,url);
	if (request->referring_url!=NULL)
	{
		bri->referring_url=new char[1+strlen(request->referring_url)];
		memset((char*)bri->referring_url,0,1+strlen(request->referring_url));
		strcpy((char*)bri->referring_url,request->referring_url);
	} else
	{
		bri->referring_url=new char[1+strlen(request->url)];
		memset((char*)bri->referring_url,0,1+strlen(request->url));
		strcpy((char*)bri->referring_url,request->url);
	}
	bri->http=this;
	if (request->auth_info!=NULL)
		bri->auth_info=request->auth_info;
	request_queue->AddItem(bri);
	if ((request->internal_status&STATUS_WAITING_ON_AUTH_INFO)!=0)
	{
		request->internal_status^=STATUS_WAITING_ON_AUTH_INFO;
		request->internal_status|=STATUS_DELETE_REQUEST;
	}
//	DoneWithRequest(request);
}
bool HTTPv4::IsPersistent(void)
{
	return true;
}
void HTTPv4::Stop(void)
{
	if (_terminate_==0)
		atomic_add(&_terminate_,1);
	if (request_lock.IsLocked())
		request_lock.Unlock();
	if (CacheSystem!=NULL)
	 	CacheSystem->Unregister(cache_user_token);
	CacheSystem=NULL;
	if (AppSettings!=NULL)
	{
		AppSettings->PrintToStream();
		if (AppSettings->HasMessage("httpv4_settings"))
			AppSettings->ReplaceMessage("httpv4_settings",HTTPSettings);
		else
			AppSettings->AddMessage("httpv4_settings",HTTPSettings);
		AppSettings->PrintToStream();
	}
	status_t stat;
	wait_for_thread(layer_manager_thid,&stat);
}
int32 HTTPv4::GetURL(BMessage *info)
{
	return 0;
}
void HTTPv4::AddMenuItems(BMenu *menu)
{
}
void HTTPv4::RemoveMenuItems(void)
{
}

uint32 HTTPv4::ConnectionEstablished(Connection *connection)
{
	if (_terminate_)
		return STATUS_NOTIFICATION_FAILED;
	if (connection->NotifiedConnect())
		return STATUS_NOTIFICATION_SUCCESSFUL;
	request_lock.Lock();
	printf("HTTPv4 Connection established! %p\n",connection);
	http_request_info_st *current=http_request_list;
	while(current!=NULL)
	{
		if (current->connection==connection)
		{
			printf("(ConnectionEstablished) request found %p (%s)\n",current,current->host);
			current->internal_status|=STATUS_CONNECTED_TO_SERVER;
			if ((current->internal_status&STATUS_REQUEST_SENT_TO_SERVER)==0)
			{
				off_t bytes=current->connection->Send((char*)current->request_string.String(),current->request_string.Length());
				current->internal_status|=STATUS_REQUEST_SENT_TO_SERVER;
				printf("(ConnectionEstablished) request sent %Ld\n",bytes);
				if (bytes==-1)
				{
					printf("(ConnectionEstablished) Error: %s\n",current->connection->ErrorString(current->connection->Error()));
				}
			}
			break;
		}
		current=current->next;
	}
	request_lock.Unlock();
	return STATUS_NOTIFICATION_SUCCESSFUL;
}

uint32 HTTPv4::ConnectionAlreadyExists(Connection *connection)
{
	if (_terminate_)
		return STATUS_NOTIFICATION_FAILED;
	printf("HTTPv4::ConnectionAlreadyExists() begun %p\n",connection);
//	if (connection->NotifiedConnect())
//		return;
	request_lock.Lock();
	http_request_info_st *current=http_request_list;
	while(current!=NULL)
	{
		printf("Considering: %p - %p\n",current,current->connection);
		if (current->connection==connection)
		{
			printf("(ConnectionAlreadyExists) request found %p (%s)\n",current,current->host);
			current->internal_status|=STATUS_CONNECTED_TO_SERVER;
			if ((current->internal_status&STATUS_REQUEST_SENT_TO_SERVER)==0)
			{
				off_t bytes=current->connection->Send((char*)current->request_string.String(),current->request_string.Length());
				printf("(ConnectionAlreadyExists) request sent %Ld\n",bytes);
				if (bytes==-1)
				{
					printf("[ConnectionAlreadyExists] Error on send attempt; closing current connection, and requesting new one.\n");
					current->internal_status^=STATUS_CONNECTED_TO_SERVER;
					current->connection=NULL;
					tcp_manager->Disconnect(connection);
				} else
					current->internal_status|=STATUS_REQUEST_SENT_TO_SERVER;
			}
			break;
		}
		current=current->next;
	}
	request_lock.Unlock();
	printf("HTTPv4::ConnectionAlreadyExists() done %p\n",current);
	return STATUS_NOTIFICATION_SUCCESSFUL;
}

uint32 HTTPv4::ConnectionTerminated(Connection *connection)
{
	if (_terminate_)
		return STATUS_NOTIFICATION_FAILED;
	printf("HTTPv4: Disconnected from server %p: %s\n",connection,connection->ErrorString(connection->Error()));
	BAutolock autolock(&request_lock);
	if (autolock.IsLocked())
	{
		printf("HTTPv4::ConnectionTerminated() lock acquired\n");
		http_request_info_st *current=http_request_list;
		while(current!=NULL)
		{
			if (current->connection==connection)
			{
				current->internal_status|=STATUS_CONNECTION_CLOSED;
				if ((current->internal_status&STATUS_CONNECTED_TO_SERVER)==0)
				{//we never got connected before we got the terminated notification
					if ((current->internal_status&STATUS_NO_RESPONSE_FROM_SERVER)==0)
					{
						//The original attempt to connect to the server asynchronously failed.
						//Try again synchronously.
						printf("HTTPv4: Switching to synchronous socket for host %s\n",current->host);
						//tcp_manager->Disconnect(connection);//DoneWithSession(connection);
						current->connection->OwnerRelease();
						current->connection=NULL;
						current->internal_status^=(STATUS_CONNECTION_CLOSED|STATUS_NO_RESPONSE_FROM_SERVER);
//						current->connection=tcp_manager->CreateConnection(this,current->host,current->port,current->secure,false);
					} else
					{
						//The synchronous attempt also failed... Maybe the server isn't up or listening...
						BMessage disconnect(SH_LOADING_PROGRESS);
						disconnect.AddInt32("command",COMMAND_INFO);
						disconnect.AddInt32("site_id",current->site_id);
						disconnect.AddInt32("url_id",current->url_id);
						disconnect.AddString("url",current->url);
						disconnect.AddInt64("bytes-received",current->bytes_received);
						disconnect.AddBool("request_done",true);
						disconnect.AddBool("secure",current->secure);
						disconnect.AddBool("aborted",true);
						Broadcast(MS_TARGET_ALL,&disconnect);
						current->connection->OwnerRelease();
						current->connection=NULL;
					//	tcp_manager->DoneWithSession(connection);
						current->internal_status|=(STATUS_NO_RESPONSE_FROM_SERVER|STATUS_NO_DATA_FROM_SERVER|STATUS_CONNECTION_CLOSED);
					}
				} else
				{//connected to server successfully, now disconnecting...
					current->internal_status|=STATUS_CONNECTION_CLOSED;
					BMessage disconnect(SH_LOADING_PROGRESS);
						disconnect.AddInt32("command",COMMAND_INFO);
						disconnect.AddInt32("site_id",current->site_id);
						disconnect.AddInt32("url_id",current->url_id);
						disconnect.AddString("url",current->url);
						disconnect.AddInt64("bytes-received",current->bytes_received);
						disconnect.AddBool("request_done",true);
						disconnect.AddBool("secure",current->secure);
						disconnect.AddBool("aborted",true);
						Broadcast(MS_TARGET_ALL,&disconnect);
						
				//		tcp_manager->DoneWithSession(connection);
						current->internal_status|=STATUS_CONNECTION_CLOSED;
						if ((current->internal_status&STATUS_RECEIVING_DATA)==0)
							DoneWithRequest(current);//we're not receiving data, so clean up a bit.
				}
				break;
			}
			current=current->next;
		}
	}
	return STATUS_NOTIFICATION_SUCCESSFUL;
}

uint32 HTTPv4::DataIsWaiting(Connection *connection)
{
	if (_terminate_)
		return STATUS_NOTIFICATION_FAILED;
	if (request_lock.LockWithTimeout(10000)==B_OK)
	{
		if (connection->DataSize()>0)
		{
			http_request_info_st *current=http_request_list;
			while(current!=NULL)
			{
				if (current->connection==connection)
				{
					printf("Data is waiting for %s (s: %ld u: %ld ; %Ld bytes)\n",current->url,current->site_id,current->url_id,current->connection->DataSize());
					if ((current->internal_status&STATUS_RESPONSE_RECEIVED_FROM_SERVER)==0)
						current->internal_status|=STATUS_RESPONSE_RECEIVED_FROM_SERVER;
					if ((current->internal_status&STATUS_RECEIVING_DATA)==0)
						current->internal_status|=STATUS_RECEIVING_DATA;
					break;
				}
				current=current->next;
			}
		}
		request_lock.Unlock();
	return STATUS_NOTIFICATION_SUCCESSFUL;
	}
	return STATUS_NOTIFICATION_FAILED;
}

uint32 HTTPv4::ConnectionError(Connection *connection)
{
	if (_terminate_)
		return STATUS_NOTIFICATION_FAILED;
	return STATUS_NOTIFICATION_SUCCESSFUL;
}

uint32 HTTPv4::ConnectionFailed(Connection *connection)
{
	if (_terminate_)
		return STATUS_NOTIFICATION_FAILED;
	printf("HTTPv4: Connection failed %p\n",connection);
	return STATUS_NOTIFICATION_SUCCESSFUL;
}

uint32 HTTPv4::DestroyingConnectionObject(Connection *connection)
{
	if (_terminate_)
		return STATUS_NOTIFICATION_FAILED;
	request_lock.Lock();
	http_request_info_st *current=http_request_list;
	while (current!=NULL)
	{
		if (current->connection==connection)
		{
			DoneWithRequest(current);
			current->connection=NULL;
			break;
		}
		current=current->next;
	}
	request_lock.Unlock();
	return STATUS_NOTIFICATION_SUCCESSFUL;
}

status_t HTTPv4::ReceiveBroadcast(BMessage *msg)
{
	int32 command=0;
	status_t status=B_ERROR;
	msg->FindInt32("command",&command);
	switch(command)
	{
		case COMMAND_SEND:
		{
					const char *url;
					bool free_url=false;
					if (msg->HasString("target_url"))
						msg->FindString("target_url",&url);
					else
						msg->FindString("url",&url);
					if (strstr(url,"://")==NULL)
					{
						//Great... the url doesn't have a protocol identifier; default to http.
						printf("HTTPv4: Protocol not specified in URL; defaulting to HTTP.\n");
						char *temp_str=new char[strlen(url)+8];
						memset(temp_str,0,strlen(url)+8);
						strcpy(temp_str,"http://");
						strcat(temp_str,url);
						url=temp_str;
						free_url=true;
						temp_str=NULL;
					}
					
					if (strncasecmp(url,"http",4)!=0)
						return B_ERROR; //We aren't compatible with this url...
					build_request_st *bri=new build_request_st;
					bri->http=this;
					msg->FindInt32("site_id",&bri->site_id);
					msg->FindInt32("url_id",&bri->url_id);
					
					bri->url=new char[strlen(url)+1];
					memset((char*)bri->url,0,strlen(url)+1);
					strcpy((char*)bri->url,url);
					if (build_request_thid==0)
					{
						build_request_thid=spawn_thread(BuildRequest_th,"http - build request", B_LOW_PRIORITY,bri);
						printf("HTTPv4:\tbuild request thread id %ld\n",build_request_thid);
						resume_thread(build_request_thid);
						wait_for_thread(build_request_thid,&status);
						build_request_thid=0;
						printf("HTTPv4 (RB): build request thread completed\n");
					} else {
						//add it to a queue, and check that periodically in the heartbeat to see if we have anything we need to process.
						request_queue->AddItem(bri);
					}
					if (free_url)
					{
						memset((char*)url,0,strlen(url)+1);
						delete url;
					}
			
		}break;
		case COMMAND_RETRIEVE:
		{
		msg->PrintToStream();
			

			switch(msg->what)
			{
				case SH_RETRIEVE_START:
				{

					const char *url;
					bool free_url=false;
					if (msg->HasString("target_url"))
						msg->FindString("target_url",&url);
					else
						msg->FindString("url",&url);
					if (strstr(url,"://")==NULL)
					{
						//Great... the url doesn't have a protocol identifier; default to http.
						printf("HTTPv4: Protocol not specified in URL; defaulting to HTTP.\n");
						char *temp_str=new char[strlen(url)+8];
						memset(temp_str,0,strlen(url)+8);
						strcpy(temp_str,"http://");
						strcat(temp_str,url);
						url=temp_str;
						free_url=true;
						temp_str=NULL;
					}
					
					if (strncasecmp(url,"http",4)!=0)
						return B_ERROR; //We aren't compatible with this url...
					if (msg->HasBool("reload"))
					{
						bool reload=false;
						msg->FindBool("reload",&reload);
						if (reload)
						{
							if (CacheSystem!=NULL)
							{
								int32 cache_object_token=CacheSystem->FindObject(cache_user_token,url);
								if (cache_object_token>-1)
								{
									CacheSystem->AcquireWriteLock(cache_user_token,cache_object_token);
									CacheSystem->ClearContent(cache_user_token,cache_object_token);
									CacheSystem->ReleaseWriteLock(cache_user_token,cache_object_token);
								}
							}
						}
					}	
					build_request_st *bri=new build_request_st;
					bri->http=this;
					msg->FindInt32("site_id",&bri->site_id);
					msg->FindInt32("url_id",&bri->url_id);
					
					bri->url=new char[strlen(url)+1];
					memset((char*)bri->url,0,strlen(url)+1);
					strcpy((char*)bri->url,url);
					if (build_request_thid==0)
					{
						build_request_thid=spawn_thread(BuildRequest_th,"http - build request", B_LOW_PRIORITY,bri);
						printf("HTTPv4:\tbuild request thread id %ld\n",build_request_thid);
						resume_thread(build_request_thid);
						wait_for_thread(build_request_thid,&status);
						build_request_thid=0;
						printf("HTTPv4 (RB): build request thread completed\n");
					} else {
						//add it to a queue, and check that periodically in the heartbeat to see if we have anything we need to process.
						request_queue->AddItem(bri);
					}
					if (free_url)
					{
						memset((char*)url,0,strlen(url)+1);
						delete url;
					}
					

				}break;
				default:
				{
					
				}
			}

		}break;
		case COMMAND_INFO:
		{
			switch(msg->what)
			{
				case SH_RETRIEVE_STOP:
				{
					int32 target_url_id=0;
					msg->FindInt32("url_id",&target_url_id);
					
					request_lock.Lock();
					http_request_info_st *current=http_request_list;
					while (current!=NULL)
					{
						if (current->url_id==target_url_id)
						{
							DoneWithRequest(current);
							break;
						}
						current=current->next;
					}
					
					request_lock.Unlock();
				}break;
				case B_QUIT_REQUESTED:
				{
					Debug("Themis is shutting down; I'm cleaning up...");
					Stop();
				}break;
				case LoadingNewPage:
				{
				}break;
				case PlugInUnLoaded:
				{
					PlugClass *plugin_object=NULL;
					msg->FindPointer("plugin",(void**)&plugin_object);
					if (plugin_object!=NULL)
					{
						if (plugin_object->PlugID()=='cash')
						{
							cache_user_token=0;
							CacheSystem=NULL;
							Debug("Cache System was just unloaded. I'm useless without it!");
							
						}
						
					}
					status=B_OK;
					
				}break;
				case PlugInLoaded:
				{
					PlugClass *plugin_object=NULL;
					msg->FindPointer("plugin",(void**)&plugin_object);
					if (plugin_object!=NULL)
					{
						if (plugin_object->PlugID()=='cash')
						{
							CacheSystem=(CachePlug*)plugin_object;
							cache_user_token=CacheSystem->Register(Type(),"HTTP Protocol Add-on");
							Debug("Cache System was just loaded!");
							
						}
						
					}

//commented out temporarily for development and testing of other parts.
					if (((plugin_object->BroadcastTarget()&TARGET_HANDLER)!=0) || ((plugin_object->BroadcastTarget()&TARGET_PARSER)!=0))
					{
						BMessage mime_query(GetSupportedMIMEType);
						mime_query.AddInt32("ReplyTo",BroadcastTarget());
						mime_query.AddPointer("ReplyToPointer",this);
						mime_query.AddInt32("command",COMMAND_INFO_REQUEST);
						mime_query.AddBool("supportedmimetypes",true);
						Broadcast(plugin_object->BroadcastTarget(),&mime_query);
						
					}
					
					status=B_OK;
				}break;
				
			}
			
		}break;
		
	}
	return status;
}

status_t HTTPv4::BroadcastReply(BMessage *msg)
{
	printf("HTTPv4: BroadcastReply()\n\b");
	
	msg->PrintToStream();
	int32 command=msg->FindInt32("command");
	switch (command)
	{
		case COMMAND_INFO:
		{
			switch(msg->what)
			{
				case SupportedMIMEType:
				{
					printf("Supported MIME Types returned\n");
#warning Rewrite this section if at all possible...
					BString str;
					int32 count=0;
					type_code type;
					msg->GetInfo("mimetype",&type,&count);
					smt_st *cur,*cur2,*cur3;
					for (int32 i=0;i<count;i++) {
						str="nothing";
						msg->FindString("mimetype",i,&str);
						cur3=NULL;
						cur3=smthead;
						while (cur3!=NULL) {
							if (strcasecmp(cur3->type,str.String())==0)
								break;
							cur3=cur3->next;
						}
						if (cur3!=NULL)
							continue;
						/*
						The above code should prevent a duplicate type from being added.
						*/
						cur3=NULL;
						cur=new smt_st;
						cur->type=new char[str.Length()+1];
						memset(cur->type,0,str.Length()+1);
						strcpy(cur->type,str.String());
						if (smthead==NULL) {
							smthead=cur;
						} else {
							cur2=smthead;
							while (cur2->next!=NULL)
								cur2=cur2->next;
							cur2->next=cur;
						}
						
					}
				}break;
				default:
				break;
			}
			
		}break;
		default:
		break;
		
	}
	
return B_OK;
	
}

uint32 HTTPv4::BroadcastTarget()
{
	return MS_TARGET_HTTP_PROTOCOL;
}

status_t HTTPv4::BuildRequest_th(void *arg)
{
	build_request_st *bri=(build_request_st*)arg;
	status_t status=B_ERROR;
	BAutolock autolock(bri->http->request_build_lock);
	if (autolock.IsLocked())
	{
		bri->http->BuildRequest(bri);
	}
		
	delete bri;
	return status;
}

void HTTPv4::BuildRequest(build_request_st *bri)

{
	//bri = build request info
	HTTPv4 *http=bri->http;
//	status_t status=B_ERROR;
	printf("HTTPv4:\tBuild request for url: %s\n",bri->url);
	http->request_lock.Lock();
	http_request_info_st *current_item=NULL,*new_item=new http_request_info_st;
	http->request_lock.Unlock();
	new_item->url=bri->url;
	new_item->site_id=bri->site_id;
	new_item->url_id=bri->url_id;
	new_item->redirection_counter=bri->redirection_counter;
	new_item->referring_url=bri->referring_url;
	new_item->request_method=bri->request_method;
	if (http->ParseURL(bri->url,new_item)!=B_OK)
	{
		printf("HTTPv4:\tProblem parsing the URL (%s). Abandoning attempt.\n",bri->url);
		delete new_item;
		return;
	}
	//check the cache
	if (http->CacheSystem!=NULL)
	{
		new_item->cache_object_token=http->CacheSystem->FindObject(http->cache_user_token,new_item->url);
		if (new_item->cache_object_token!=B_ENTRY_NOT_FOUND)
		{	
			new_item->cached=true;
		}
		
	}
	
	
	//build request string
	const char *method=NULL;
	switch(new_item->request_method)
	{
		case HTTP_REQUEST_TYPE_HEAD:
		{
			method="HEAD";
		}break;
		case HTTP_REQUEST_TYPE_DELETE:
		{
			method="DELETE";
		}break;
		case HTTP_REQUEST_TYPE_POST:
		{
			method="POST";
		}break;
		case HTTP_REQUEST_TYPE_PUT:
		{
			method="PUT";
		}break;
		case HTTP_REQUEST_TYPE_GET:
		default:
		{
			method="GET";
			if (new_item->request_method==0)
				new_item->request_method=HTTP_REQUEST_TYPE_GET;
		}
	}
	new_item->request_string<<method<<" ";
	if (http->use_proxy_server)
	{
		new_item->request_string<<new_item->url;
	}
	else
	{
		new_item->request_string<<new_item->uri;
	}
	new_item->request_string<<" HTTP/1.1\r\n";
	if (http->use_proxy_server)
	{
		if (http->proxy_auth_win!=NULL)
		{
			status_t status=B_OK;
			thread_id paw=http->proxy_auth_win->Thread();
			http->proxy_auth_win->Activate(true);
			wait_for_thread(paw,&status);
			
		}
		if (http->proxy_auth_info!=NULL)
		{
			new_item->request_string<<"Proxy-Authorization: ";
			if ((http->proxy_auth_info->auth_type&AuthManager::AUTHENTICATION_TYPE_BASIC)!=0)
				new_item->request_string<<"Basic";
			else
				if ((http->proxy_auth_info->auth_type&AuthManager::AUTHENTICATION_TYPE_DIGEST)!=0)
					new_item->request_string<<"Digest";
				else
					new_item->request_string<<"Basic";//default to basic.
			new_item->request_string<<" "<<http->proxy_auth_info->auth_object->Credentials()<<"\r\n";
		}
	}
	//check for authentication
	if (auth_manager->HasAuthInfo(new_item))
		printf("Authentication info is available\n");
	if (new_item->auth_info!=NULL)
	{
		new_item->request_string<<"Authorization: ";
		if ((new_item->auth_info->auth_type&AuthManager::AUTHENTICATION_TYPE_BASIC)!=0)
				new_item->request_string<<"Basic";
			else
				if ((new_item->auth_info->auth_type&AuthManager::AUTHENTICATION_TYPE_DIGEST)!=0)
					new_item->request_string<<"Digest";
			new_item->request_string<<" "<<new_item->auth_info->auth_object->Credentials()<<"\r\n";
		
	}
	new_item->request_string<<"Host: "<<new_item->host;
	if (((new_item->port!=80) && (!new_item->secure)) || ((new_item->port!=443) && (new_item->secure)))
		new_item->request_string<<":"<<(int32)new_item->port;
	new_item->request_string<<"\r\n";
	//language; temporarily hardcoded to english
	new_item->request_string<<"Accept-Language: en-US,*\r\n";
	new_item->request_string<<"Accept-Charset: *,utf-8\r\n";
	//temporarily hardcoded user agent...
	use_useragent=USER_AGENT_THEMIS;
	
	switch (use_useragent)
	{
		case USER_AGENT_THEMIS:
		{
			new_item->request_string<<"User-Agent: Themis (http://themis.sourceforge.net)\r\n";
			
		}break;
		case USER_AGENT_MOZILLA:
		{
			new_item->request_string<<"User-Agent: Mozilla/5.0 (BeOS; BePC; rv: 4.0) Themis http://themis.sourceforge.net)\r\n";
		}break;
		case USER_AGENT_INTERNETEXPLORER:
		{
			new_item->request_string<<"User-Agent: Mozilla/5.0 (compatible; MSIE 6.0; BeOS ; Themis http://themis.sourceforge.net)\r\n";
		}break;
		case USER_AGENT_CUSTOM:
		{
			new_item->request_string<<"User-Agent: " <<custom_useragent<<"\r\n";
		}break;
		default:
		{//add no user agent
		}
	}
	if (bri->referring_url!=NULL)
		new_item->request_string<<"Referer: "<<bri->referring_url<<"\r\n";
	if (new_item->cached)
	{ 
		BMessage *cache_info=http->CacheSystem->GetInfo(http->cache_user_token,new_item->cache_object_token);
		if (cache_info->HasString("last-modified"))
		{
			const char *last_modified=NULL;
			cache_info->FindString("last-modified",&last_modified);		
			new_item->request_string<<"If-Modified-Since: "<<last_modified<<"\r\n";
		}
		if (cache_info->HasString("etag"))
		{
			const char *etag=NULL;
			cache_info->FindString("etag",&etag);
			new_item->request_string<<"ETag: "<<etag<<"\r\n";
		}
		
		delete cache_info;
	}
	if (http->smthead!=NULL)
	{//add supported types to header
		new_item->request_string<<"Accept: ";
		smt_st *current=http->smthead;
		while (current!=NULL)
		{
			new_item->request_string<<current->type;
			if (current->next!=NULL)
				new_item->request_string<<", ";
			current=current->next;
		}
		new_item->request_string <<", text/*, */*\r\n";
	}//add supported types to header
	else
		new_item->request_string << "Accept: text/*, */*\r\n";
	if (enable_cookies)
	{
		//get cookies...
		const char *cookies=NULL;
		cookies=http->CookieMonster->GetCookie(new_item->host,new_item->uri,new_item->port,new_item->secure);
		if (cookies!=NULL)
		{
			new_item->request_string<<cookies;
			delete cookies;
		}
	}
	//let them know we can handle gzipped responses...
	//new_item->request_string<<"Accept-Encoding:  gzip, compress, deflate\r\n";

	if (pipelining)
	{
		new_item->request_string<<"Connection: Keep-Alive\r\nKeep-Alive: 300\r\n";
		if (http->use_proxy_server)
		{
			new_item->request_string<<"Proxy-Connection: Keep-Alive\r\n";
		}
	}
	else
	{
		new_item->request_string<<"Connection: close\r\n";
		if (http->use_proxy_server)
		{
			new_item->request_string<<"Proxy-Connection: close\r\n";
		}
	}
	printf("HTTPv4: Request String\n%s\nHTTPv4: End Request String\n",new_item->request_string.String());
	new_item->internal_status|=STATUS_REQUEST_BUILT;
	new_item->request_string<<"\r\n";//the final CRLF pair necessary for the request.
	//add new item to the list...
	http->request_lock.Lock();
	current_item=http->http_request_list;
	if (current_item==NULL)
	{
		http->http_request_list=new_item;
	} else {
		while(current_item->next!=NULL)
			current_item=current_item->next;
		current_item->next=new_item;
	}
	http->request_lock.Unlock();
	printf("HTTPv4: Build request all done.\n");
//	exit_thread(status);
}
status_t HTTPv4::ParseURL(const char *url,http_request_info_st *request)
{
	status_t status=B_ERROR;
	BAutolock autolock(parse_url_lock);
	if (autolock.IsLocked())
	{
		char *colon2slash=NULL,*firstslash=NULL,*portcolon=NULL;
		colon2slash=strstr(url,"://");
		if (colon2slash==NULL)
		{//check protocol information
			//no protocol information provided; we might be able to recover from this.
			//assume that we're not secure...
			//just for the record; this code *SHOULD NEVER EXECUTE*. We should always
			//receive a proper URL from the window.
			request->secure=false;
			request->port=80;
			firstslash=strchr(url,'/');
			int32 len=0;
			if (firstslash==NULL)
			{ //find beginning of uri
				request->uri=new char[2];
				strcpy((char*)request->uri,"/\0");
				portcolon=strchr(url,':');
				if (portcolon==NULL)
				{//find port number
					//keep default port numbers based on protocol.
					if (request->secure)
						request->port=443;
					else
						request->port=80;
					request->host=new char[strlen(url)+1];
					strcpy((char*)request->host,url);
				}else//find port number
				{//find port number
					len=strlen(url)-((portcolon+1)-url);
					char *port=new char[len+1];
					strncpy(port,portcolon+1,len);
					request->port=atoi(port);
					delete port;
					len=portcolon-(colon2slash+3);
					request->host=new char[len+1];
					memset((char*)request->host,0,len+1);
					strncpy((char*)request->host,url,len);
				}//find port number
				//update the url to at include "/" at the end of the host[:port]
				const char *temp_url=new char[strlen(request->url)+2];
				memset((char*)temp_url,0,strlen(request->url)+2);
				strcpy((char*)temp_url,request->url);
				strcat((char*)temp_url,"/");
				delete request->url;
				request->url=temp_url;
				temp_url=NULL;
			} else//find beginning of uri
			{//find beginning of uri
				request->uri=new char[1+strlen(firstslash)];
				strcpy((char*)request->uri,firstslash);
				portcolon=strchr(url,':');
				if ((portcolon==NULL) || (portcolon>firstslash))
				{//find port number
					//keep default port numbers based on protocol.
					if (request->secure)
						request->port=443;
					else
						request->port=80;
					len=firstslash-url;
					request->host=new char[len+1];
					memset((char*)request->host,0,len+1);
					strncpy((char*)request->host,url,len);
				}else//find port number
				{//find port number
					char *port=new char[firstslash-portcolon+1];
					strncpy(port,portcolon+1,firstslash-portcolon);
					request->port=atoi(port);
					delete port;
					len=portcolon-url;
					request->host=new char[len+1];
					memset((char*)request->host,0,len+1);
					strncpy((char*)request->host,url,len);
				}//find port number
			}//find beginning of uri
		} else {//check protocol information
			int32 len=colon2slash-url;
			char *protocol=new char[1+len];
			memset(protocol,0,len+1);
			strncpy(protocol,url,len);
			printf("HTTPv4: length %ld %s\n",len, protocol);
			if ((len==5) && (strcasecmp(protocol,"https")==0))
			{
				request->secure=true;
				request->port=443;
			} else {
				if ((len==4) && (strcasecmp(protocol,"http")==0))
				{
					request->secure=false;
					request->port=80;
				} else
				{
					//incorrect protocol information has been provided.
					delete protocol;
					return B_ERROR;
				}
			}
			firstslash=strchr(colon2slash+3,'/');
			if (firstslash==NULL)
			{ //find beginning of uri
				request->uri=new char[2];
				strcpy((char*)request->uri,"/\0");
				portcolon=strchr(colon2slash+3,':');
				if (portcolon==NULL)
				{//find port number
					//keep default port numbers based on protocol.
					if (request->secure)
						request->port=443;
					else
						request->port=80;
					request->host=new char[strlen(colon2slash)+1];
					strcpy((char*)request->host,colon2slash+3);
				}else//find port number
				{//find port number
					len=strlen(url)-((portcolon+1)-url);
					char *port=new char[len+1];
					strncpy(port,portcolon+1,len);
					request->port=atoi(port);
					delete port;
					len=portcolon-(colon2slash+3);
					request->host=new char[len+1];
					memset((char*)request->host,0,len+1);
					strncpy((char*)request->host,colon2slash+3,len);
				}//find port number
				//update the url to at include "/" at the end of the host[:port]
				const char *temp_url=new char[strlen(request->url)+2];
				memset((char*)temp_url,0,strlen(request->url)+2);
				strcpy((char*)temp_url,request->url);
				strcat((char*)temp_url,"/");
				delete request->url;
				request->url=temp_url;
				temp_url=NULL;
			} else//find beginning of uri
			{//find beginning of uri
				request->uri=new char[1+strlen(firstslash)];
				strcpy((char*)request->uri,firstslash);
				portcolon=strchr(colon2slash+3,':');
				if ((portcolon==NULL) || (portcolon>firstslash))
				{//find port number
					//keep default port numbers based on protocol.
					if (request->secure)
						request->port=443;
					else
						request->port=80;
					len=firstslash-(colon2slash+3);
					request->host=new char[len+1];
					memset((char*)request->host,0,len+1);
					strncpy((char*)request->host,colon2slash+3,len);
				}else//find port number
				{//find port number
					char *port=new char[firstslash-portcolon+1];
					strncpy(port,portcolon+1,firstslash-portcolon);
					request->port=atoi(port);
					delete port;
					len=portcolon-(colon2slash+3);
					request->host=new char[len+1];
					memset((char*)request->host,0,len+1);
					strncpy((char*)request->host,colon2slash+3,len);
				}//find port number
			}//find beginning of uri
		}//check protocol information
		status=B_OK;
		printf("host: %s\nuri: %s\nport: %u\nsecure: %d\n",request->host,request->uri,request->port,request->secure);
		
	}
	return status;
}
status_t HTTPv4::start_layer_manager(void *arg)
{
	status_t status=B_ERROR;
	HTTPv4 *http=(HTTPv4*)arg;
	if (http!=NULL)
	{
		http->layer_manager(http);
		status=B_NO_ERROR;
	}
	return status;
}

void HTTPv4::layer_manager(HTTPv4 *http)
{
	http_request_info_st *current=NULL,*previous=NULL, *next=NULL;
	unsigned char *buffer=(unsigned char*)malloc(BUFFER_SIZE);
	off_t bytes_received=0L,data_size=0L;
	off_t buffer_size=BUFFER_SIZE;
	memset(buffer,0,BUFFER_SIZE);//we do an initial wipe of memory just to start things off fresh
	while(http->_terminate_==0)
	{
		if (http->request_lock.LockWithTimeout(10000)==B_OK)
		{
//			printf("current: %p\n",current);
			if ((current==NULL) && (http->http_request_list!=NULL))
			{
				current=http->http_request_list;
				if (previous!=NULL)
					previous=NULL;
				http->request_lock.Unlock();
				snooze(25000);//yield the lock for a short time for another process.
				continue;
			} else
			{
				if ((current!=NULL) && (current->internal_status&STATUS_DELETE_REQUEST)!=0)
				{
//					printf("request %p has been marked for deletion.\n",current);
					if ((current->internal_status&STATUS_WAITING_ON_AUTH_INFO)!=0)
					{
//						printf("request %p is waiting on auth info, and will not be deleted until it has been received.\n",current);
						current=current->next;
						http->request_lock.Unlock();
						snooze(5000);
						continue;
					}
						next=current->next;
					if (http->http_request_list==current)
						http->http_request_list=next;
					else
					{
						previous=http->http_request_list;
						while(previous->next!=NULL)
						{
							if (previous->next==current)
								break;
							previous=previous->next;
						}
						previous->next=next;
					}
//					if (previous==NULL)
//						http->http_request_list=next;
//					else
//					{
//						previous->next=next;
//					}
					delete current;
					current=next;
					http->request_lock.Unlock();
					printf("HTTPv4: A request was just deleted.\n");
					snooze(10000);
					continue;
				}
				if ((current!=NULL) && (http->http_request_list==NULL))
				{
					//what the hell?? all http request structures should be in the list unless being deleted
					printf("http request list is null, resetting...\n");
					http->http_request_list=current;
				}
				if (current!=NULL)
				{
//				printf("current: %p\n",current);
					//process current http request begin
					if (
						(//Make sure the request has been built
							(current->internal_status&STATUS_REQUEST_BUILT)!=0)
						 && ((current->internal_status&STATUS_CONNECTED_TO_SERVER)==0))
					{//Request has been built but server connection hasn't been established begin
						if (current->connection==NULL)
						{
							printf("layer_manager: making connection\n");
							const char *host=current->host;
							uint16 port=current->port;
							if (http->use_proxy_server)
							{
								host=http->proxy_server_host;
								port=http->proxy_server_port;
								current->secure=false;
							}
							if ((current->internal_status&STATUS_NO_RESPONSE_FROM_SERVER)!=0)
								current->connection=http->tcp_manager->CreateConnection(http,host,port,current->secure,true);
							else
								current->connection=http->tcp_manager->CreateConnection(http,host,port,current->secure,false);
//							printf("HTTPv4: Connection object at %p\n",current->connection);
						}
					//	else
					//		printf("connection: %p\n",current->connection);
					}//Request has been built but server connection hasn't been established begin
					if ((current->internal_status&STATUS_CONNECTED_TO_SERVER)!=0)
					{//we are connected to the server, so process here begin
						if ((current->internal_status&STATUS_RECEIVING_DATA)!=0)
						{//we have received data that needs to be processed begin
							//we don't have to make sure the memory is clear here
							//because we do so in the receive function itself.
							if (current->connection!=NULL)
							{
							//	printf("HTTPv4: Getting data size before receive.\n");
								data_size=current->connection->DataSize();
								if (data_size>0)
								{
									if (data_size>buffer_size)
									{
										buffer=(unsigned char*)realloc(buffer,data_size);
										memset(buffer,0,data_size);
										buffer_size=data_size;
									}
									else
									{
										if ((data_size<BUFFER_SIZE) && (buffer_size>BUFFER_SIZE))
										{
											buffer=(unsigned char*)realloc(buffer,BUFFER_SIZE);
											memset(buffer,0,BUFFER_SIZE);
											buffer_size=BUFFER_SIZE;
										}
										else
										{
											memset(buffer,0,buffer_size);
										}
									}
									bytes_received=current->connection->Receive(buffer,buffer_size);
//									printf("layer_manager: received %Ld bytes:\n",bytes_received);
									ProcessData(current,buffer,bytes_received);
//									printf("layer manager is done processing data\n");
								//	if (bytes_received>=data_size)
										current->internal_status^=STATUS_RECEIVING_DATA;
									/*
										special case; we want to receive data as quickly as possible.
									*/
									previous=current;
									current=current->next;
									http->request_lock.Unlock();
									snooze(10000);
									continue;
								} else
								{
									if (buffer_size>BUFFER_SIZE)
									{
										memset(buffer,0,buffer_size);
										buffer=(unsigned char*)realloc(buffer,BUFFER_SIZE);
										buffer_size=BUFFER_SIZE;
									}
					//				if (data_size!=B_TIMED_OUT)
					//					printf("DataSize() returned size %Ld\n",data_size);
	
								}
							}
						}//we have received data that needs to be processed end
						else
						{
							if ((current->internal_status&STATUS_PROCESSING_TIMED_OUT)!=0)
									ProcessChunkedData(current);
						}
					}//we are connected to the server, so process here end
					//process current http request end
					current=current->next;
					http->request_lock.Unlock();
					snooze(20000);//yield the lock for a short time for another process.
					continue;
				}
				//if current is null and the request list is also null, then make processor time
				//available for something else for 1/10 seconds.
				http->request_lock.Unlock();
				snooze(25000);
				continue;
			}
			http->request_lock.Unlock();
			snooze(100000);
		} else
			snooze(5000);
	}
	memset(buffer,0,BUFFER_SIZE);
	free(buffer);
	buffer=NULL;
//	printf("layer_manager: shutting down\n");
	exit_thread(0);
}

void HTTPv4::ProcessData(http_request_info_st *request, unsigned char *buffer, int32 length)
{
	if (((request->internal_status&STATUS_HEADERS_STARTED)==0) && ((request->internal_status&STATUS_HEADERS_COMPLETED)==0))
	{
		char *data=(char*)buffer;
		char *end_of_header=strstr(data,"\r\n\r\n");
		bool end_of_headers_present=(end_of_header==NULL)?false:true;
//		char *status_line=NULL;
		int32 Length;
		char *eol=strstr(data,"\r\n");
		if (eol!=NULL)
		{
			char temp[200];
			memset(temp,0,200);
			char *slash=strchr(data,'/');
			char *space=strchr(slash,0x20),*space2;
			space2=strchr(space+1,0x20);
			if (space2>eol)
				space2=eol;
			Length=space-(slash+1);
			strncpy(temp,slash+1,Length);
			float http_version=atof(temp);
			request->http_major_version=(uint8)http_version/1;
			request->http_minor_version=(uint8)http_version%1;
			memset(temp,0,Length);
			Length=space2-(space+1);
			strncpy(temp,space+1,Length);
			request->http_status_code=atol(temp);
			memset(temp,0,Length);
			Length=eol-(space2+1);
			if (Length>0)
			{
				request->http_status_message=new char[Length+1];
				memset((char*)request->http_status_message,0,Length+1);
				strncpy((char*)request->http_status_message,space2+1,Length);
			}
			printf("HTTPv4: Server HTTP Version: %u.%u\nHTTPv4: Status Code: %d\nHTTPv4: Status Message: %s\n",request->http_major_version,request->http_minor_version,request->http_status_code,request->http_status_message);
		} else
		{
			request->header_buffer=(char*)malloc(length+1);
			memset(request->header_buffer,0,length+1);
			strcpy(request->header_buffer,data);
		}
		if (end_of_headers_present)
		{
			ProcessHeadersGeneral(request,eol+2,strlen(eol+2));
			request->internal_status|=(STATUS_HEADERS_STARTED|STATUS_HEADERS_COMPLETED);//once this is set, we start processing received data.
				switch((request->http_status_code/100))
				{
					case 1:
					{
						ProcessHeaderLevel100(request,eol+2,strlen(eol+2));
					}break;
					case 2:
					{
						ProcessHeaderLevel200(request,eol+2,strlen(eol+2));
					}break;
					case 3:
					{
						ProcessHeaderLevel300(request,eol+2,strlen(eol+2));
					}break;
					case 4:
					{
						ProcessHeaderLevel400(request,eol+2,strlen(eol+2));
					}break;
					case 5:
					{
						ProcessHeaderLevel500(request,eol+2,strlen(eol+2));
					}break;
					default:
					printf("HTTPv4: Unknown HTTP response code: %d - %s\n",request->http_status_code,request->http_status_message);
					
				}
			ProcessData2(request,(unsigned char*)(end_of_header+4),length-((end_of_header+4)-(char*)buffer));
		} else
		{
			request->internal_status|=STATUS_HEADERS_STARTED;//we have more headers to receive and process
			if (request->header_buffer==NULL)
			{
				request->header_buffer=(char*)malloc(length+1);
				memset(request->header_buffer,0,length+1);
				strcpy(request->header_buffer,data);
			} else
			{
				int32 Length=strlen(eol)-2;
				request->header_buffer=(char*)malloc(Length+1);
				memset(request->header_buffer,0,Length+1);
				strcpy(request->header_buffer,eol+2);
			}
		}
	} else
	{
		//if headers have been started but not completed... continue headers and process any left over data.
		if (((request->internal_status&STATUS_HEADERS_STARTED)!=0) && ((request->internal_status&STATUS_HEADERS_COMPLETED)==0))
		{//process additional headers
			char *data=NULL;
				int32 Length=strlen(request->header_buffer)+length;
			int32 orig_head_len=strlen(request->header_buffer);
				request->header_buffer=(char*)realloc(request->header_buffer,Length+1);
				memset(request->header_buffer+(Length-length),0,length+1);
				strncat(request->header_buffer,(char*)buffer,length);
				data=request->header_buffer;
				char *eol=strstr(data,"\r\n");
			if (request->http_status_code==0)
			{
				if (eol!=NULL)
				{
					char temp[200];
					memset(temp,0,200);
					char *slash=strchr(data,'/');
					char *space=strchr(slash,0x20),*space2;
					space2=strchr(space+1,0x20);
					Length=space-(slash+1);
					strncpy(temp,slash+1,Length);
					float http_version=atof(temp);
					request->http_major_version=(uint8)http_version/1;
					request->http_minor_version=(uint8)http_version%1;
					memset(temp,0,Length);
					Length=space2-(space+1);
					strncpy(temp,space+1,Length);
					request->http_status_code=atol(temp);
					memset(temp,0,Length);
					Length=eol-(space2+1);
					request->http_status_message=new char[Length+1];
					memset((char*)request->http_status_message,0,Length+1);
					strncpy((char*)request->http_status_message,space2+1,Length);
					printf("HTTPv4: Server HTTP Version: %u.%u\nHTTPv4: Status Code: %d\nHTTPv4: Status Message: %s\n",request->http_major_version,request->http_minor_version,request->http_status_code,request->http_status_message);
					data=eol+2;
				}
			}
			char *end_of_header=strstr(data,"\r\n\r\n");
			bool end_of_headers_present=(end_of_header==NULL)?false:true;
			printf("the end of the headers was present: %d\n",end_of_headers_present);
			if (end_of_headers_present)
			{
				request->internal_status|=STATUS_HEADERS_COMPLETED;//once this is set, we start processing received data.
				ProcessHeadersGeneral(request,data,strlen(data));
				switch((request->http_status_code/100))
				{
					case 1:
					{
						ProcessHeaderLevel100(request,data,strlen(data));
					}break;
					case 2:
					{
						ProcessHeaderLevel200(request,data,strlen(data));
					}break;
					case 3:
					{
						ProcessHeaderLevel300(request,data,strlen(data));
					}break;
					case 4:
					{
						ProcessHeaderLevel400(request,data,strlen(data));
					}break;
					case 5:
					{
						ProcessHeaderLevel500(request,data,strlen(data));
					}break;
					default:
					printf("HTTPv4: Unknown HTTP response code: %d - %s\n",request->http_status_code,request->http_status_message);
					
				}
				printf("header length: %ld\n" ,((end_of_header+4)-(char*)data)+orig_head_len);
				printf("length received: %ld\n",length);
				printf("difference: %ld\n",length-((end_of_header+4)-(char*)data)+orig_head_len);
				ProcessData2(request,(unsigned char*)(end_of_header+4),length-((end_of_header+4)-(char*)data)+orig_head_len);
			}

		} else
		{
		//if headers have been completed, process data
			if (((request->internal_status&STATUS_HEADERS_STARTED)!=0) && ((request->internal_status&STATUS_HEADERS_COMPLETED)!=0))
			{
				ProcessData2(request,buffer,length);
			}
		}		
	}
	printf("Exiting ProcessData\n");
}
void HTTPv4::ProcessData2(http_request_info_st *request,unsigned char *buffer, int32 length)
{
	if (length==0 || buffer==NULL || request==NULL)
		return;
	if ((request->transfer_state&TRANSFER_STATE_CHUNKED)==0)
	{//not chunked file transfer
		if ((request->transfer_state&TRANSFER_STATE_ENCODED_GZIP)==0)
		{//not GZIP content encoded
			if ((request->transfer_state&TRANSFER_STATE_ENCODED_COMPRESS)==0)
			{//normal file transfer; no content encoding
//				const char *value=NULL;
				if (!Compressed(request))
					request->bytes_received+=length;
				request->size_delta=length;
				if (length>0)
					StoreData(request,buffer,length);
				/*
					If we didn't receive a content_length header, just receive until
					the connection is closed...
				*/
				if (request->content_length!=-1)
				{
					if (request->bytes_received>=request->content_length)
					{
						DoneWithRequest(request);
					}
				}
			}//normal file transfer; no content encoding
			else
			{//COMPRESS content encoded
			StoreData(request,buffer,length);//this will need to change to request,data,data_length
				if (request->bytes_received>=request->content_length)
				{
					DoneWithRequest(request);
				}
			}//COMPRESS content encoded
		}//not GZIP content encoded
		else
		{//GZIP content encoding
			StoreData(request,buffer,length);//this will need to change to request,data,data_length
				if (request->bytes_received>=request->content_length)
				{
					DoneWithRequest(request);
				}
			
//			unsigned char *data=NULL;
//			int32 data_length=0;
//			printf("Calling GUnzip\n");
//			data=GUnzip(buffer,0,length,&data_length);
//			printf("GUnzip is done\n");

		}//GZIP content encoding
	}//not chunked file transfer
	else
	{//chunked file transfer
		printf("============\nChunked Transfer Encoding\nBytes Remaining: %ld\n============\n",length);
		ProcessChunk(request,buffer,length);
	}//chunked file transfer
	printf("Exiting ProcessData2\n");
}
void HTTPv4::StoreData(http_request_info_st *request,unsigned char *buffer, int32 length)
{
	printf("HTTPv4: Store Data: %ld bytes buffer: %p request: %p\n",length,buffer,request);
	if ((length>0) && (buffer!=NULL) && (request!=NULL))
	{
		BMessage *broadcast=new BMessage(SH_LOADING_PROGRESS);
//		const char *value=NULL;
		broadcast->AddInt32("command",COMMAND_INFO);
		broadcast->AddInt32("url_id",request->url_id);
		broadcast->AddInt32("site_id",request->site_id);
		if (request->connection!=NULL)
		{
			broadcast->AddDouble("bytes-per-second",request->connection->BytesPerSecond());
			broadcast->AddBool("secure",request->connection->IsSecure());
			if (request->connection->IsSecure())
				broadcast->AddInt16("ssl_bits",request->connection->EncryptionBits());
		}
		broadcast->AddString("url",request->url);
		if (request->content_type!=NULL)
		{
			broadcast->AddString("content-type",request->content_type);
			broadcast->AddString("mime-type",request->content_type);
		}
		unsigned char *dbuffer=buffer;
		int32 dlength=length;
		bool compressed=false;
		if ((request->transfer_state&TRANSFER_STATE_ENCODED_GZIP)!=0)
		{
			int32 len=0;
			if (request->chunk_size!=0)
				len=request->chunk_size;
			else
					len=length;
			dbuffer=GUnzip(request,buffer,0,len,&dlength);
//			dbuffer=NULL;
			if (dbuffer==NULL)
			{
				dlength=length;
				dbuffer=buffer;
			} else
			{
//				printf("uncompressed data to be stored: %s\n",(char*)dbuffer);
				compressed=true;
			}
		}
		broadcast->AddInt64("bytes-received",request->bytes_received);
		if (FindHeader(request,"content-length")!=NULL)
			broadcast->AddInt64("content-length",request->content_length);
		broadcast->AddInt64("size-delta",request->size_delta);
		if ((CacheSystem==NULL) || ((request->request_method==HTTP_REQUEST_TYPE_POST) &&
			((FindHeader(request,"cache-control")!=NULL)|| (FindHeader(request,"expires")!=NULL)))
			|| ((request->cache_state&CACHE_STATE_DONT_CACHE)!=0))
		{
			if (((request->transfer_state&TRANSFER_STATE_CHUNKED)!=0 && request->chunk_size>0) || ((request->transfer_state&TRANSFER_STATE_CHUNKED)==0 && dlength>0))
				broadcast->AddData("raw_data",B_RAW_TYPE,dbuffer,dlength);
		} else
		{
			if (!request->cached)
			{
				request->cached=true;
				request->cache_object_token=CacheSystem->CreateObject(cache_user_token,request->url);
			} else
				CacheSystem->AcquireWriteLock(cache_user_token,request->cache_object_token);
			broadcast->AddInt32("cache_object_token",request->cache_object_token);
			header_info_st *current=request->header_list;
			if ((request->cache_state&CACHE_STATE_ATTRIBUTES_UPDATED)==0)
			{
				BMessage *headers=new BMessage();
				headers->AddString("url",request->url);
				headers->AddString("host",request->host);
				headers->AddString("path",request->uri);
				while(current!=NULL)
				{
					if ((current->attribute!=NULL) && (current->value!=NULL))
						headers->AddString(current->attribute,current->value);
					current=current->next;
				}
				CacheSystem->SetObjectAttr(cache_user_token,request->cache_object_token,headers);
				delete headers;
				request->cache_state|=CACHE_STATE_ATTRIBUTES_UPDATED;
			}
			printf("HTTPv4::StoreData() about to write.\n");
			CacheSystem->Write(cache_user_token,request->cache_object_token,dbuffer,dlength);
			printf("HTTPv4::StoreData() done writing.\n");
		}
		broadcast->PrintToStream();
		Broadcast(MS_TARGET_ALL,broadcast);
		printf("Broadcast call completed.\n");
		delete broadcast;
		if (compressed)
			delete dbuffer;
	}
	printf("Exiting StoreData()\n");
}
void HTTPv4::ProcessChunk(http_request_info_st *request,unsigned char *buffer, int32 length)
{
	printf("ProcessChunk: %ld bytes\n",length);
	if (length<=0)
		return;
//	uint32 start_time=0,current_time=0;
//	uint8 max_seconds=2;
	//The first chunk is a little different from the rest... It does not have a CRLF pair before
	//the chunk size other than that which determines the end of the header.
//	char *cbuffer=(char*)buffer;
//	char *crlf1=NULL,*crlf2=NULL;
//	char *start=NULL;
//	char *end=NULL;
//	char *temp=NULL;
//	int32 len=0l;
//	int32 bytes_remaining=0l;
	if (request->temporary_data_buffer==NULL)
	{
		request->temporary_data_buffer=(unsigned char*)malloc(length);
		memset(request->temporary_data_buffer,0,length);
		request->temporary_data_size=length;
		memcpy(request->temporary_data_buffer,buffer,length);
	} else
	{
		request->temporary_data_buffer=(unsigned char *)realloc(request->temporary_data_buffer,request->temporary_data_size+length);
		memset(request->temporary_data_buffer+request->temporary_data_size,0,length);
		memcpy(request->temporary_data_buffer+request->temporary_data_size,buffer,length);
		request->temporary_data_size+=length;
	}
	ProcessChunkedData(request);
	printf("End of ProcessChunk\n");
}
void HTTPv4::ProcessChunkedData(http_request_info_st *request)
{
	if ((request->transfer_state&TRANSFER_STATE_CHUNKED)!=0)
	{
		if ((request->internal_status&STATUS_PROCESSING_TIMED_OUT)!=0)
			request->internal_status^=STATUS_PROCESSING_TIMED_OUT;

	uint32 start_time=0,current_time=0;
	uint8 max_seconds=2;
	//The first chunk is a little different from the rest... It does not have a CRLF pair before
	//the chunk size other than that which determines the end of the header.
	char *cbuffer=NULL;
	char *crlf1=NULL,*crlf2=NULL;
	char *start=NULL;
	char *end=NULL;
	char *temp=NULL;
	int32 len=0l;
	int32 bytes_remaining=0l;
	cbuffer=(char*)request->temporary_data_buffer;
	bytes_remaining=request->temporary_data_size;
	if (request->chunk_id==0)
	{
		crlf1=strstr(cbuffer,"\r\n");
		len=crlf1-cbuffer;
		temp=new char[len+1];
		memset(temp,0,len+1);
		strncpy(temp,cbuffer,len);
		request->chunk_size=strtol(temp,&end,16);
		memset(temp,0,len+1);
		delete temp;
		printf("First chunk size: %lu bytes (0x%02lx)\n",request->chunk_size,(uint32)request->chunk_size);
		temp=NULL;
		if (request->chunk_size==0)
		{
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=NULL;
					request->temporary_data_size=0l;
					printf("Chunk Size==0\n");
					end=crlf2+2;
					DoneWithRequest(request);
//					request->connection->OwnerRelease();
//					request->connection=NULL;
					return;
		}
		start=crlf1+2;
//				temp=new char[request->chunk_size+1];
//				memset(temp,0,request->chunk_size+1);
//				strncpy(temp,start,request->chunk_size);
//				printf("Chunk:\n%s\n===========\n",temp);
				request->chunk_id++;
		end=start+request->chunk_size;
		bytes_remaining-=(len+2);
		char *end2=NULL;
		if (bytes_remaining>=request->chunk_size)
		{//we can at least finish this chunk
			bytes_remaining-=request->chunk_size;
			if (!Compressed(request))
				request->bytes_received+=request->chunk_size;
			//write chunk
			request->size_delta=request->chunk_size;
			StoreData(request,(unsigned char*)start,request->chunk_size);
			start_time=current_time=real_time_clock();
			do {
				crlf1=strstr(end,"\r\n");//find the end of the chunk
				if (crlf1==NULL)
				{//we seem to have run out of data...
					//there isn't a CRLF pair at the current position... that means that
					//we have data here...
					printf("End data at loop start: %s\n",end);
					unsigned char *temp2=(unsigned char*)malloc(bytes_remaining);
					memset(temp2,0,bytes_remaining);
					memcpy(temp2,end,bytes_remaining);
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=temp2;
					request->temporary_data_size=bytes_remaining;
					temp2=NULL;
					break;
				}
				crlf2=strstr(crlf1+2,"\r\n");//find the end of the chunk size line
				if (crlf2==NULL)
				{
					printf("we have the start of a chunk header, but not the whole thing.\n");
					unsigned char *temp2=(unsigned char *)malloc(bytes_remaining);
					memset(temp2,0,bytes_remaining);
					memcpy(temp2,crlf1,bytes_remaining);
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=temp2;
					request->temporary_data_size=bytes_remaining;
					request->chunk_size=0;
					temp2=NULL;
					break;
				}
				start=crlf2+2;
				len=crlf2-(crlf1+2);//number of bytes the chunk size is written in
				temp=new char[len+1];
				memset(temp,0,len+1);
				strncpy(temp,crlf1+2,len);
				end2=NULL;
				request->chunk_size=strtol(temp,&end2,16);
				memset(temp,0,len+1);
				delete temp;
				printf("chunk size: %lu bytes (0x%lx)\n",request->chunk_size,(uint32)request->chunk_size);
				bytes_remaining-=(len+4);
				current_time=real_time_clock();
				if ((current_time-start_time)>=max_seconds)
				{
					printf("HTTPv4: ProcessChunk() ran out of allotted time to process. (%u seconds)\n",max_seconds);
					int32 size=(request->temporary_data_buffer+request->temporary_data_size)-(unsigned char*)end;
					printf("size: %ld\n",size);
					//size should == bytes_remaining
					unsigned char *temp2=(unsigned char*)malloc(size);
					memset(temp2,0,size);
					memcpy(temp2,(unsigned char*)crlf1/*end*/,size);
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=temp2;
					request->temporary_data_size=size;
					request->internal_status|=STATUS_PROCESSING_TIMED_OUT;
					request->chunk_size=0L;
//		printf("[out of time]Temporary Data Buffer:\n%s\n",(char*)request->temporary_data_buffer);
					break;
					
				}
				if (request->chunk_size>bytes_remaining)
				{
					printf("Not enough data left in buffer: chunk size %ld - bytes remaining %ld\n",request->chunk_size,bytes_remaining);
					int32 size=(request->temporary_data_buffer+request->temporary_data_size)-(unsigned char*)end;
					printf("size: %ld\n",size);
					//size should == bytes_remaining
					unsigned char *temp2=(unsigned char*)malloc(size);
					memset(temp2,0,size);
					memcpy(temp2,(unsigned char*)crlf1/*end*/,size);
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=temp2;
					request->temporary_data_size=size;
					request->chunk_size=0L;
					break;
				}
				if ((request->chunk_size==0) && (len==1))
				{
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=NULL;
					request->temporary_data_size=0l;
					printf("Chunk Size==0\n");
					end=crlf2+2;
					printf("Remainder: %ld\n",strlen(end));
					DoneWithRequest(request);
//					request->connection->OwnerRelease();
//					request->connection=NULL;
					break;
				}
				bytes_remaining-=request->chunk_size;
				//write the known chunk
				if (!Compressed(request))
					request->bytes_received+=request->chunk_size;
				request->size_delta=request->chunk_size;
				StoreData(request,(unsigned char*)start,request->chunk_size);
				request->chunk_id++;
//				temp=new char[request->chunk_size+1];
//				memset(temp,0,request->chunk_size+1);
//				strncpy(temp,start,request->chunk_size);
//				printf("Chunk:\n%s\n===========\n",temp);
//				memset(temp,0,request->chunk_size+1);
//				delete temp;
				temp=NULL;
				end=start+request->chunk_size;
				request->chunk_size=-1;
			}while((bytes_remaining>0) &&(bytes_remaining>request->chunk_size));
			printf("bytes remaining: %ld\tchunk size: %ld\n",bytes_remaining,request->chunk_size);
		} else
		{//this chunk isn't big enough to be complete.
			printf("incomplete chunk\n");
			request->chunk_id=0;
			return;
		}
	} else
	{
		printf("Chunk!=0\n");
		end=(char*)request->temporary_data_buffer;
		if ((request->chunk_size>0) && (request->chunk_size<=request->temporary_data_size))
		{
			printf("completing earlier chunk\n");
			bytes_remaining=request->temporary_data_size-=request->chunk_size;
			end+=request->chunk_size;
			if (!Compressed(request))
				request->bytes_received+=request->chunk_size;
			request->size_delta=request->chunk_size;
			StoreData(request,(unsigned char *)cbuffer,request->chunk_size);
			request->chunk_size=0;
		}
		char *end2=NULL;
		start_time=current_time=real_time_clock();
		
			do {
				crlf1=strstr(end,"\r\n");//find the end of the chunk
				if (crlf1==NULL)
				{//we seem to have run out of data...
					//there isn't a CRLF pair at the current position... that means that
					//we have data here...
					printf("End data at loop start\n");
					for (int i=0; i<10; i++)
						printf("\t%d:\t0x%x\n",i,end[i]);
					unsigned char *temp2=(unsigned char*)malloc(bytes_remaining);
					memset(temp2,0,bytes_remaining);
					memcpy(temp2,end,bytes_remaining);
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=temp2;
					request->temporary_data_size=bytes_remaining;
					temp2=NULL;
					break;
				}
				crlf2=strstr(crlf1+2,"\r\n");//find the end of the chunk size line
				if (crlf2==NULL)
				{
					printf("we have the start of a chunk header, but not the whole thing.\n");
					for (int i=0; i<10; i++)
						printf("\t%d:\t0x%x\n",i,crlf1[i]);
					unsigned char *temp2=(unsigned char *)malloc(bytes_remaining);
					memset(temp2,0,bytes_remaining);
					memcpy(temp2,crlf1,bytes_remaining);
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=temp2;
					request->temporary_data_size=bytes_remaining;
					request->chunk_size=0;
					temp2=NULL;
					break;
				}
				start=crlf2+2;
				len=crlf2-(crlf1+2);//number of bytes the chunk size is written in
				temp=new char[len+1];
				memset(temp,0,len+1);
				strncpy(temp,crlf1+2,len);
				printf("[temp] chunk size: %s (%ld)\n",temp,strlen(temp));
				end2=NULL;
				request->chunk_size=strtol(temp,&end2,16);
				memset(temp,0,len+1);
				delete temp;
				printf("chunk size: %ld bytes (0x%lx)\n",request->chunk_size,(uint32)request->chunk_size);
				bytes_remaining-=(len+4);
				printf("bytes remaining: %ld (0x%lx)\n",bytes_remaining,(uint32)bytes_remaining);
				
				current_time=real_time_clock();
				printf("processing time: %ld\n",(current_time-start_time));
				if ((current_time-start_time)>=max_seconds)
				{
					printf("HTTPv4: ProcessChunk() ran out of allotted time to process. (%u seconds)\n",max_seconds);
					int32 size=(request->temporary_data_buffer+request->temporary_data_size)-(unsigned char*)end;
					printf("size: %ld\n",size);
					//size should == bytes_remaining
					unsigned char *temp2=(unsigned char*)malloc(size);
					memset(temp2,0,size);
					memcpy(temp2,(unsigned char*)crlf1/*end*/,size);
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=temp2;
					request->temporary_data_size=size;
					request->internal_status|=STATUS_PROCESSING_TIMED_OUT;
					request->chunk_size=0L;
	//	printf("[out of time]Temporary Data Buffer:\n%s\n",(char*)request->temporary_data_buffer);
					break;
					
				}
				if (request->chunk_size>bytes_remaining)
				{
					printf("Not enough data left in buffer: chunk size %ld - bytes remaining %ld\n",request->chunk_size,bytes_remaining);
					int32 size=(request->temporary_data_buffer+request->temporary_data_size)-(unsigned char*)end;//bytes_remaining;
					printf("size: %ld\n",size);
					//size should == bytes_remaining
					unsigned char *temp2=(unsigned char*)malloc(size);
					memset(temp2,0,size);
					memcpy(temp2,(unsigned char*)crlf1/*end*/,size);
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=temp2;
					request->temporary_data_size=size;
					request->chunk_size=0L;
					break;
				}
				if ((request->chunk_size==0) && (len==1))
				{
					memset(request->temporary_data_buffer,0,request->temporary_data_size);
					free(request->temporary_data_buffer);
					request->temporary_data_buffer=NULL;
					request->temporary_data_size=0l;
					printf("Chunk Size==0\n");
					end=crlf2+2;
					printf("Remainder: %ld\n",strlen(end));
					if (((request->internal_status&STATUS_RECEIVING_DATA)==0) &&
						(request->connection->DataSize()<=0))
					{
					}
						DoneWithRequest(request);
					break;
				}
					bytes_remaining-=request->chunk_size;
				request->size_delta=request->chunk_size;
				StoreData(request,(unsigned char*)start,request->chunk_size);
				if (!Compressed(request))
				request->bytes_received+=request->chunk_size;
				request->chunk_id++;
//				temp=new char[request->chunk_size+1];
//				memset(temp,0,request->chunk_size+1);
//				strncpy(temp,start,request->chunk_size);
//				printf("Chunk:\n%s\n===========\n",temp);
//				memset(temp,0,request->chunk_size+1);
//				delete temp;
				temp=NULL;
				end=start+request->chunk_size;
				request->chunk_size=0;
			}while((bytes_remaining>0) &&(bytes_remaining>request->chunk_size));
			printf("bytes remaining: %ld\tchunk size: %ld\n",bytes_remaining,request->chunk_size);
	}
	}
}
void HTTPv4::ProcessHeadersGeneral(http_request_info_st *request, char *buffer, int32 length)
{
	header_info_st *new_list=NULL,*current=NULL,*last=NULL;
	char *colon=buffer,*eol=buffer,*eoh,*start=buffer;
	eoh=strstr(buffer,"\r\n\r\n");
	int32 len;
	do
	{
		colon=strchr(start,':');
		eol=strstr(start,"\r\n");
//		printf("start:\t%d\tcolon:\t%d\teol:\t%d\n",start-buffer,colon-buffer,eol-buffer);
		len=colon-start;
		current=new header_info_st;
		current->attribute=new char[len+1];
		memset((char*)current->attribute,0,len+1);
		strncpy((char*)current->attribute,start,len);
		colon+=1;
		while (colon<eol)
		{
			if (!isspace(colon[0]))
			{
				len=eol-colon;
				current->value=new char[len+1];
				memset((char*)current->value,0,len+1);
				strncpy((char*)current->value,colon,len);
				break;
			}
			colon+=1;
		}
		start=eol+2;
		if (new_list==NULL)
		{
			new_list=current;
		}
		if (last==NULL)
		{
			last=current;
		} else
		{
			last->next=current;
			last=last->next;
		}
	}while ((start<eoh) && (eol!=NULL));
	request->header_list=new_list;
	current=request->header_list;
	printf("Headers:\n");
	BString temp;//available for anything that might need it.
	while (current!=NULL)
	{
		printf("\t%s:\t%s\n",current->attribute,current->value);
		if (strcasecmp("warning",current->attribute)==0)
		{
			BString warning;
			warning <<"HTTPv4: Received this warning from server: \""<<current->value<<"\".";
			Debug(warning.String());
		}
		if (strcasecmp("WWW-Authenicate",current->attribute)==0)
		{
			//send it to the authentication manager
		}
		if (strcasecmp("Proxy-Authenticate",current->attribute)==0)
		{
			//also send it to the authentication manager
		}
		if (strcasecmp("Pragma",current->attribute)==0)
		{
			//for pragma statements other than no-cache
		}
		if (strcasecmp("Transfer-encoding",current->attribute)==0)
		{
			//uh-oh
			if ((current->value!=NULL)&& (strcasecmp("chunked",current->value)==0))
			{
			//fuck!
				request->transfer_state|=TRANSFER_STATE_CHUNKED;
			}
		}
		if (strcasecmp("Content-encoding",current->attribute)==0)
		{
			//great...
			if ((current->value!=NULL)&& (strcasecmp("gzip",current->value)==0 || strcasecmp("x-gzip",current->value)==0))
			{
				//sigh...
				request->transfer_state|=TRANSFER_STATE_ENCODED_GZIP;
			} else
			{
				if ((current->value!=NULL)&& (strcasecmp("compress",current->value)==0 || strcasecmp("x-compress",current->value)==0))
				{
					//double sigh...
					request->transfer_state|=TRANSFER_STATE_ENCODED_COMPRESS;
				}
			}
		}
		if (strcasecmp("Content-Length",current->attribute)==0)
		{
			request->content_length=atol(current->value);
			printf("HTTPv4 Expected Content Length: %Ld bytes\n",request->content_length);
		}
		if ((strcasecmp("Set-cookie",current->attribute)==0) || (strcasecmp("Set-Cookie2",current->attribute)==0))
		{
			if (current->value!=NULL)
			{
				if (enable_cookies)
				{
					CookieMonster->SetCookie(current->value,request->host, request->uri,request->port,request->secure);
				}
			}
		}
		if ((strcasecmp("content-type",current->attribute)==0) && (current->value!=NULL))
			request->content_type=current->value;
		/*
			Apparently some sites such as online.wellsfargo.com may return NULL
			values for either cache-control or pragma. In Wells Fargo's case it's
			done when the user agent string isn't one that it likes. The same may
			be happening elsewhere too; so treat it as if it's not cachable.
		*/
		if (((strcasecmp("cache-control",current->attribute)==0) ||
			(strcasecmp("pragma",current->attribute)==0)) &&
			( (current->value==NULL) ||((current->value!=NULL) && (strcasecmp("no-cache",current->value)==0))))
		{//don't cache the data
			request->cache_state|=CACHE_STATE_DONT_CACHE;
		}
		current=current->next;
	}
}
void HTTPv4::ProcessHeaderLevel100(http_request_info_st *request, char *buffer, int32 length)
{
	uint8 subcode=request->http_status_code%100;
	printf("Header Level %d Status Sub-code: %d\n",100,subcode);
	switch(subcode)
	{
		case 0://100 Continue
		{
		}break;
		case 1://101 Switching protocols
		{
		}break;
		default:
			printf("HTTPv4: Unknown subcode %d (%d): %s\n",subcode,request->http_status_code,request->http_status_message);
	}
}
void HTTPv4::ProcessHeaderLevel200(http_request_info_st *request, char *buffer, int32 length)
{
	uint8 subcode=request->http_status_code%100;
	printf("Header Level %d Status Sub-code: %d\n",200,subcode);
	switch(subcode)
	{
		case 0://200 Ok
		{
		}break;
		case 1://201 Created
		{
		}break;
		case 2://202 Accepted
		{
		}break;
		case 3://203 Non-Autoritative Information
		{
		}break;
		case 4://204 No Content
		{
			//There is no content beyond the headers; the browser should display the current page as is.
			DoneWithRequest(request);
		}break;
		case 5://205 Reset Content
		{
			//There is no content beyond the headers; the browser should clear any forms back to original status.
			DoneWithRequest(request);
		}break;
		case 6://206 Partial Content
		{
		}break;
		default:
			printf("HTTPv4: Unknown subcode %d (%d): %s\n",subcode,request->http_status_code,request->http_status_message);
	}
}
void HTTPv4::ProcessHeaderLevel300(http_request_info_st *request, char *buffer, int32 length)
{
	uint8 subcode=request->http_status_code%100;
	printf("Header Level %d Status Sub-code: %d\n",300,subcode);
	switch(subcode)
	{
		case 0://300 Multiple Choices
		{
		}break;
		case 1://301 Moved Permanently
		{
			request->cache_state|=CACHE_STATE_DONT_CACHE;
			if (request->redirection_counter<5)
				ResubmitRequest(request,FindHeader(request,"location"));
			else
			{
				printf("Redirection limit exceeded!\n");
				Debug("Redirection limit exceeded!\n");
				DoneWithRequest(request);
			}
		}break;
		case 2://302 Moved Temporarily
		{
			//send the request again to the URL provided in the Location header.
			//per RFC 2068, do not cache responses with HTTP status code 302
			request->cache_state|=CACHE_STATE_DONT_CACHE;
			if (request->redirection_counter<5)
				ResubmitRequest(request,FindHeader(request,"location"));
			else
			{
				printf("Redirection limit exceeded!\n");
				Debug("Redirection limit exceeded!\n");
				DoneWithRequest(request);
			}
		}break;
		case 3://303 See Other
		{
			//use GET method to retrieve the data at this other location
		}break;
		case 4://304 Not Modified
		{
			//no content is returned
			/*
				No content is to be returned, meaning we can terminate this request
				right here, right now. We should already have the some cache information,
				but we need to load up the rest.
			*/
			if (CacheSystem!=NULL)
			{
				request->content_length=request->bytes_received=CacheSystem->GetObjectSize(cache_user_token,request->cache_object_token);
			}
			DoneWithRequest(request);
		}break;
		case 5://305 Use Proxy
		{
			use_proxy_server=true;
			http_request_info_st *proxy_url=new http_request_info_st;
			ParseURL(FindHeader(request,"Location"),proxy_url);
			proxy_server_host=proxy_url->host;
			proxy_server_port=proxy_url->port;
			proxy_url->host=NULL;//prevent the http_request_info_st destructor from deleting the allocated host string.
			delete proxy_url;
		}break;

		default:
			printf("HTTPv4: Unknown subcode %d (%d): %s\n",subcode,request->http_status_code,request->http_status_message);
	}
}
void HTTPv4::ProcessHeaderLevel400(http_request_info_st *request, char *buffer, int32 length)
{
	uint8 subcode=request->http_status_code%100;
	printf("Header Level %d Status Sub-code: %d\n",400,subcode);
	request->cache_state|=CACHE_STATE_DONT_CACHE;
	switch(subcode)
	{
		case 0://400 Bad Request
		{
		}break;
		case 1://401 Unauthorized
		{
			if (auth_manager->HasAuthInfo(request))
				auth_manager->InvalidateAuthInfo(request);
			request->internal_status|=STATUS_WAITING_ON_AUTH_INFO;
			auth_manager->GetAuth(request);
		}break;
		case 2://402 Payment Required
		{
		}break;
		case 3://403 Forbidden
		{
			DoneWithRequest(request);
		}break;
		case 4://404 Not Found
		{
			DoneWithRequest(request);
		}break;
		case 5://405 Method Not Allowed
		{
			//Expect an Allow: header listing allowed request methods.
		}break;
		case 6://406 Not Acceptable
		{
		}break;
		case 7://407 Proxy Authentication Required
		{
			if (proxy_auth_info!=NULL)
			{
				printf("HTTPv4: Proxy auth info is invalid.\n");
				SetProxyAuthInfo(NULL);
			}
			request->internal_status|=STATUS_WAITING_ON_AUTH_INFO;
			auth_manager->GetAuth(request);
		}break;
		case 8://408 Request Timeout
		{
		}break;
		case 9://409 Conflict
		{
		}break;
		case 10://410 Gone
		{
		}break;
		case 11://411 Length Required
		{
		}break;
		case 12://412 Precondition Failed
		{
		}break;
		case 13://413 Request Entity Too Large
		{
		}break;
		case 14://414 Request-URI Too Long
		{
		}break;
		case 15://415 Unsupported Media Type
		{
		}break;
		default:
			printf("HTTPv4: Unknown subcode %d (%d): %s\n",subcode,request->http_status_code,request->http_status_message);
	}
}
void HTTPv4::ProcessHeaderLevel500(http_request_info_st *request, char *buffer, int32 length)
{
	uint8 subcode=request->http_status_code%100;
	printf("Header Level %d Status Sub-code: %d\n",500,subcode);
	request->cache_state|=CACHE_STATE_DONT_CACHE;
	switch(subcode)
	{
		case 0://500 Internal Server Error
		{
		}break;
		case 1://501 Not Implemented
		{
		}break;
		case 2://502 Bad Gateway
		{
		}break;
		case 3://503 Service Unavailable
		{
		}break;
		case 4://504 Gateway Timeout
		{
		}break;
		case 5://505 HTTP Version Not Supported
		{
			DoneWithRequest(request);
		}break;
		default:
			printf("HTTPv4: Unknown subcode %d (%d): %s\n",subcode,request->http_status_code,request->http_status_message);
	}
}

void HTTPv4::DoneWithRequest(http_request_info_st *request)
{
	if ((request->internal_status&STATUS_DELETE_REQUEST)!=0)
		return;
	request_lock.Lock();
	printf("HTTPv4: DoneWithRequest()\n");
	printf("Bytes Received: %Ld\n",request->bytes_received);
	if (request->content_length>0)
		printf("Expected Content Length: %Ld\n",request->content_length);
	if (Compressed(request))
		printf("Data was compressed\n");
		http_request_info_st *current=http_request_list;
		while (current!=NULL)
		{
			if (current==request)
			{
				current->internal_status|=STATUS_DELETE_REQUEST;
				if (pipelining && (request->http_major_version==1 && request->http_minor_version!=0))
				current->connection->OwnerRelease();
				else
					tcp_manager->Disconnect(current->connection);
				current->connection=NULL;
				break;
			}
			current=current->next;
		}
	
		BMessage *broadcast=new BMessage(SH_LOADING_PROGRESS);
//		const char *value=NULL;
		broadcast->AddInt32("command",COMMAND_INFO);
		broadcast->AddInt32("url_id",request->url_id);
		broadcast->AddInt32("site_id",request->site_id);
		if (request->connection!=NULL)
		{
			broadcast->AddDouble("bytes-per-second",request->connection->BytesPerSecond());
			broadcast->AddBool("secure",request->connection->IsSecure());
			if (request->connection->IsSecure())
				broadcast->AddInt16("ssl_bits",request->connection->EncryptionBits());
		}
		broadcast->AddString("url",request->url);
		if (request->content_type!=NULL)
		{
			broadcast->AddString("content-type",request->content_type);
			broadcast->AddString("mime-type",request->content_type);
		}
		broadcast->AddInt64("bytes-received",request->bytes_received);
		if (FindHeader(request,"content-length")!=NULL)
			broadcast->AddInt64("content-length",request->content_length);
		if (request->cached)
			broadcast->AddInt32("cache_object_token",request->cache_object_token);
		broadcast->AddBool("request_done",true);
		broadcast->PrintToStream();
		Broadcast(MS_TARGET_ALL,broadcast);
		delete broadcast;
	request_lock.Unlock();
}
const char *HTTPv4::FindHeader(http_request_info_st *request,const char *attribute,bool case_sensitive)
{
	const char *value=NULL;
	header_info_st *current=request->header_list;
	while (current!=NULL)
	{
		if (case_sensitive)
		{
			if (strcmp(attribute,current->attribute)==0)
			{
				value=current->value;
				break;
			}
		} else
		{
			if (strcasecmp(attribute,current->attribute)==0)
			{
				value=current->value;
				break;
			}
		}
		current=current->next;
	}
	return value;
}
void HTTPv4::SetProxyAuthInfo(auth_info_st *proxy_auth)
{
	request_lock.Lock();
	if (proxy_auth_info!=NULL)
	{
		delete proxy_auth_info->auth_object;
		delete proxy_auth_info;
	}
	proxy_auth_info=proxy_auth;
	request_lock.Unlock();
}
unsigned char *HTTPv4::GUnzip(http_request_info_st *request,unsigned char *buffer, int32 offset,int32 length,int32 *out_length)
{
	printf("zlib version: %s\n",zlibVersion());
	printf("GUnzip: buffer: %p\toffset: %ld\tlength: %ld\n",buffer,offset,length);
	if ((request==NULL)||(buffer==NULL) || (length<=0) || (offset<0))
		return NULL;
		{
			uint32 uncompressed_size=((length*2)>BUFFER_SIZE)?(length*2):BUFFER_SIZE;
			uint32 bytes_received=0;
			unsigned char *uncompressed_buffer=(unsigned char*) malloc(uncompressed_size+1);
			memset(uncompressed_buffer,0,uncompressed_size+1);
			bool init=false;
			if (request->gzip_info==NULL)
			{
				request->gzip_info=new zlib_info_st;
				request->gzip_info->stream=new z_stream;
				request->gzip_info->stream->zalloc=Z_NULL;
				request->gzip_info->stream->zfree=Z_NULL;
				request->gzip_info->stream->opaque=Z_NULL;
				request->gzip_info->error=Z_OK;
				init=true;
			} else
				bytes_received=request->gzip_info->stream->total_out;
				request->gzip_info->stream->next_in=(buffer+offset);
				request->gzip_info->stream->avail_in=length;
			request->gzip_info->stream->next_out=uncompressed_buffer;
			request->gzip_info->stream->avail_out=uncompressed_size;
			int32 window_bits=47;//15 for normal size, +32 for automatic deflate and gzip header detection.
			if (init)
			{
				printf("initializing zlib library.\n");
				request->gzip_info->error=inflateInit2(request->gzip_info->stream,window_bits);
				printf("zlib init status: %ld\n",request->gzip_info->error);
			}
//			bool good=false;
//			int32 error=Z_OK;
//			z_streamp stream=new z_stream;
//			stream->zalloc=Z_NULL;
//			stream->zfree=Z_NULL;
//			stream->opaque=Z_NULL;
//			stream->next_in=(buffer+offset);
//			stream->avail_in=length;
//			error=Z_OK;
				bool cont=true;
				printf("available in: %d\n",request->gzip_info->stream->avail_in);
				int32 session_bytes_received=0,start_total=request->gzip_info->stream->total_out;//,current_total=0;
			while (cont && request->gzip_info->stream->avail_in>0)
			{
				if (uncompressed_buffer==NULL)
				{
					uncompressed_buffer=(unsigned char*)malloc(512);
					memset(uncompressed_buffer,0,512);
					uncompressed_size=512;
					request->gzip_info->stream->next_out=uncompressed_buffer;
					request->gzip_info->stream->avail_out=uncompressed_size;
				} else
				{
					request->gzip_info->stream->next_out=(uncompressed_buffer+session_bytes_received);
					request->gzip_info->stream->avail_out=(uncompressed_size-session_bytes_received);
				}
				request->gzip_info->error=inflate(request->gzip_info->stream,Z_SYNC_FLUSH);
				printf("inflate status: %ld - %s\n",request->gzip_info->error,request->gzip_info->stream->msg);
				printf("total bytes expanded: %ld\n",request->gzip_info->stream->total_out);
				session_bytes_received=request->gzip_info->stream->total_out-start_total;
				printf("expanded %ld bytes this session\n",session_bytes_received);
				switch(request->gzip_info->error)
				{
					case Z_BUF_ERROR:
					{
						int32 offset2=uncompressed_size;
						uncompressed_size+=(int32)(uncompressed_size*0.10);
						printf("HTTPv4: [Inflate] Increasing uncompressed buffer size to %ld bytes.\n",uncompressed_size);
						uncompressed_buffer=(unsigned char*)realloc(uncompressed_buffer,uncompressed_size+1);
						memset(uncompressed_buffer+offset2,0,uncompressed_size-offset2+1);
					}break;
					case Z_STREAM_END:
					{
						printf("decompression successful\n");
						inflate(request->gzip_info->stream,Z_FINISH);
						inflateEnd(request->gzip_info->stream);
						request->size_delta=session_bytes_received;
//						session_bytes_received=request->gzip_info->stream->total_out-start_total;
						*out_length=session_bytes_received;//uncompressed_size-request->gzip_info->stream->avail_out;//request->gzip_info->stream->total_out;
//						printf("decompressed data (%ld): %s\n",*out_length,(char*)uncompressed_buffer);
//						for (int i=0; i<16; i++)
//							printf("\t%d\t%c\t0x%x\n",i,uncompressed_buffer[i],uncompressed_buffer[i]);
						//The actual uncompressed file length is at the end of the stream of compressed bytes
						//so reset the previous known content-length (which referred to the compressed file length)
						//to the real value.
						memcpy(&request->content_length,buffer+offset+length-4,4);
						request->bytes_received=request->gzip_info->stream->total_out;
						printf("Uncompressed File Size: %Ld/%Ld\n",request->bytes_received,request->content_length);
						unsigned char *temp=new unsigned char[*out_length+1];
						memcpy(temp,uncompressed_buffer,*out_length);
						memset(uncompressed_buffer,0,uncompressed_size+1);
						free( uncompressed_buffer);
						uncompressed_buffer=temp;
						delete request->gzip_info->stream;
						delete request->gzip_info;
						request->gzip_info=NULL;
						printf("uncompressed: %s\n",uncompressed_buffer);
						return uncompressed_buffer;
					}break;
					case Z_OK:
					{
						printf("HTTPv4: [inflate] Z_OK total bytes expanded: %ld\n",request->gzip_info->stream->total_out);
						session_bytes_received=request->gzip_info->stream->total_out-start_total;
						
					}break;
					default:
					{
						printf("error %ld: %s\n",request->gzip_info->error,request->gzip_info->stream->msg);
						cont=false;
					}
				}
			}
			printf("expanded %ld bytes this session\n",session_bytes_received);
			if (cont==true)
			{
				printf("available in must have been 0\n");
//				bytes_received=request->gzip_info->stream->total_out-bytes_received;//calculation may be wrong
						session_bytes_received=request->gzip_info->stream->total_out-start_total;
				bytes_received=session_bytes_received;//uncompressed_size-request->gzip_info->stream->avail_out;
//				printf("Bytes received A: %ld\tB: %ld\n",bytes_received,uncompressed_size-request->gzip_info->stream->avail_out);
				if (bytes_received>0)
				{
				printf(">0\n");
				printf("creating new buffer\n");
				unsigned char *temp=new unsigned char [bytes_received+1];
				printf("nulling buffer\n");
				memset(temp,0,bytes_received+1);
				printf("copying data\n");
				memcpy(temp,uncompressed_buffer,bytes_received);
				printf("nulling old buffer\n");
				memset(uncompressed_buffer,0,uncompressed_size+1);
				printf("freeing old buffer\n");
				free( uncompressed_buffer);
				printf("assigning variables\n");
				uncompressed_buffer=temp;
				*out_length=bytes_received;
				request->size_delta=session_bytes_received;
								request->bytes_received+=session_bytes_received;
								if ((request->transfer_state&TRANSFER_STATE_CHUNKED)==0)
									request->content_length=request->bytes_received+1;//we don't know how big the damned file is until we receive all of it and decompress it...

				} else
				{
					printf("<=0\n");
					*out_length=0;
				memset(uncompressed_buffer,0,uncompressed_size+1);
					free(uncompressed_buffer);
					uncompressed_buffer=NULL;
				}
				printf("GUnzip returning\n");
				return uncompressed_buffer;
			}
			delete uncompressed_buffer;
			delete request->gzip_info->stream;
			delete request->gzip_info;
			return NULL;
		}
}
bool HTTPv4::Compressed(http_request_info_st *request)
{
	if ((request->transfer_state&TRANSFER_STATE_ENCODED_GZIP)!=0 || (request->transfer_state&TRANSFER_STATE_ENCODED_COMPRESS)!=0 ||(request->transfer_state&TRANSFER_STATE_ENCODED_DEFLATE)!=0)
		return true;
	else
		return false;
}
