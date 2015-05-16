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
#ifndef _http_object_v4_
#define _http_object_v4_

#include "protocol_plugin.h"
#include <Locker.h>
#include "tcpmanager.h"
#include "connection.h"
#include "smt.h"
#include <string.h>
#include <kernel/OS.h>
#include <List.h>
#include <String.h>
#include "authmanager.h"
#include "authvw.h"
#include "zlib.h"

class CachePlug;
class CookieManager;
using namespace _Themis_Networking_;

/*!
	\brief Linked list structure for holding header information.
*/
struct header_info_st
{
	const char *attribute;//!< Name of a header field.
	const char *value;//!< Value of a header field.
	header_info_st *next;//!< Next header field.
	header_info_st()
	{
		attribute=NULL;
		value=NULL;
		next=NULL;
	}
	~header_info_st();
};
/*!
	\brief Informational structure for zlib (gzip, deflate,compress) compressed file transfers.
	
	Most of the fields in this structure will likely be removed in the near future. As a result
	none will be documented at this time. (October 3, 2004 10:44 am PDT)
*/
struct zlib_info_st
{
	z_streamp stream;
	int32 error;
	unsigned char *uncompressed_data;
	int32 uncompressed_length;
	int8 window_bits;
	int32 state;
	bool initialized;
	zlib_info_st()
	{
		stream=NULL;
		error=0;
		uncompressed_data=NULL;
		uncompressed_length=0;
		window_bits=15;
		state=0;
		initialized=false;
	}
	~zlib_info_st()
	{
		stream=NULL;
		error=0;
		uncompressed_data=NULL;
		uncompressed_length=0;
		window_bits=15;
		state=0;
	}
};
/*!
	\brief HTTP request information structure.
	
	This structure make HTTPv4 work.
*/
struct http_request_info_st
{
	zlib_info_st *gzip_info; //!< If compressed, file transfer's zlib information.
	uint16 http_status_code; //!< HTTP status code as returned by server.
	const char *http_status_message; //!< HTTP status message if returned by server.
	uint32 internal_status; //!< Bitmap field containing various states as used by the HTTP class.
	const char *url; //!< The URL as sent by the calling application part, possibly different from originally sent depending on redirections. 
	const char *uri; //!< The location of the data on the server.
	const char *host; //!< The server name/address.
	const char *content_type; //!< The mime type of the data as returned by the server.
	uint16 port; //!< The port number to which the HTTP object will connect.
	bool secure; //!< Whether or not the HTTP object should request SSL encryption.
	const char *referring_url; //!< The URL that led Themis to request the current URL.
	bool cached; //!< Whether or not the URL is cached.
	authwin *auth_win; //!< A pointer to an authorization request window.
	uint32 cache_system_type; //!< The cache system (RAM or Disk) that is being used if cached.
	int32 cache_object_token; //!< If cached, the cache object token identification number.
	int32 url_id; //!< The URL ID as provided by another part of the application. Theoretically the SiteHandler.
	int32 site_id;	//!< The site ID as provided by another part of the application. 
	int32 request_method; //!< The HTTP request method in use by this object.
	off_t bytes_received; //!< Bytes received by this request.
	off_t content_length; //!< Expected content length of this request; may be zero if the transfer encoding is chunked, the content-encoding is compressed, or if the web server is stupid.
	http_request_info_st *next; //!< Next http_request_info_st structure in the linked list.
	Connection *connection; //!< The Connection object which we are using to communicate with the server.
	auth_info_st *auth_info; //!< The authorization information structure if needed.
	BString request_string; //!< The entire request sent to the server.
	uint8 http_major_version; //!< Major HTTP version number of the server.
	uint8 http_minor_version; //!< Minor HTTP version number of the server.
	char *header_buffer; //!< A buffer used to cache received header information; it's used to make sure that all of the header is received before we start processing the actual content of the request.
	unsigned char *temporary_data_buffer; //!< A buffer used to cache data when an incomplete chunk has been received while in chunked transfer mode. 
	int32 temporary_data_size; //!< Size of the temporary_data_buffer.
	header_info_st *header_list; //!< Pointer to the first header_info_st in the header linked list.
	uint8 redirection_counter; //!< A counter for the number of times the original URL request has been redirected. More than 5 redirects is considered an infinite loop by the HTTP specifications.
	int32 transfer_state; //!< A bitmap field that records the transmission type of the request: chunked, gzip, compress, deflate.
	int32 cache_state; //!< A bitmap field that records the cache state.
	volatile int32 chunk_size; //!< Size of the current chunk (in chunked transfer encoding) being received.
	volatile int32 chunk_bytes_remaining; //!< Bytes remaining in the current chunk.
	volatile int32 chunk_id; //!< Arbitrary chunk identifier.
	int64 size_delta; //!< Amount of data has been received post-processing after a call to Connection::Receive()
	BMessage *post_data; //!< A BMessage containing data to be posted to the server.
	http_request_info_st()
	{
		post_data=NULL;
		gzip_info=NULL;
		auth_win=NULL;
		size_delta=0L;
		cache_state=0l;
		content_type=NULL;
		chunk_id=0l;
		chunk_size=0l;
		chunk_bytes_remaining=0l;
		transfer_state=0l;
		redirection_counter=0;
		http_major_version=0;
		http_minor_version=0;
		header_list=NULL;
		bytes_received=0L;
		content_length=-1L;
		http_status_code=0;
		http_status_message=NULL;
		request_method=0;
		internal_status=0;
		url=NULL;
		uri=NULL;
		host=NULL;
		referring_url=NULL;
		port=0;
		secure=false;
		cached=false;
		cache_system_type=0;
		cache_object_token=-1;
		url_id=0;
		site_id=0;
		connection=NULL;
		next=NULL;
		auth_info=NULL;
		header_buffer=NULL;
		temporary_data_buffer=NULL;
		temporary_data_size=0L;
	}
	~http_request_info_st();
};
/*!
	\brief The HTTP protocol class, version 4.
	
	This is the fourth major rewrite of the HTTP support for Themis, and hopefully
	the last major rewrite. This class receives requests for transfer of data either
	to or from a remote server, and completes that transfer. By working with other classes,
	such as the CookieManager, AuthManager, TCPManager, and it's parent classes, HTTPv4
	provides the backbone functionality of any web browser capable of communicating over
	a network.
	
	@todo I need to finish implementing the POST/PUT support.
*/
class HTTPv4: public ProtocolPlugClass
{
	private:
		CookieManager *CookieMonster;//!< Pointer to the cookie management object.
		BMessage *AppSettings;//!< Application settings object.
		BMessage **AppSettings_p;//!< Pointer to the application settings pointer as set by the framework application.
		TCPManager *tcp_manager; //!< The TCPManager object through which the HTTPv4 class will obtain Connection objects, and communicate indirectly.
		uint32 cache_user_token;//!< The cache system user identification token. Required to communicate with the cache system properly.
		CachePlug *CacheSystem;//!< Pointer to the cache system or NULL if not loaded.
		plugman *PluginManager;//!< Pointer to the plug-in manager.
		BLocker request_build_lock;//!< BLocker object to prevent more than one HTTP request from being built simultaneously.
		BLocker request_lock;//!< BLocker object to prevent simultaneous access to sensitive data.
		BLocker parse_url_lock;//!< BLocker object to prevent more than one URL from being parsed simultaneously.
		bool use_proxy_server;//!< Variable determining whether or not a proxy server should be used.
		const char *proxy_server_host;//!< The address of the proxy server.
		uint16 proxy_server_port;//!< The port number to which we should attempt to connect if using a proxy server.
		auth_info_st *proxy_auth_info;//!< Proxy server authentication information.
		authwin *proxy_auth_win;//!< Proxy server authentication request window.
		bool send_referral_info;//!< Determines whether or not the HTTP_REFERER header should be sent.
		/*!
			\brief Parse the passed in URL, and place the data into the http_request_info_st for use.
		*/
		status_t ParseURL(const char *url,http_request_info_st *request);
		bool enable_cookies;//!< Determines whether cookies should be utilized or not.
		http_request_info_st *http_request_list;//!< Pointer to the first http_request_info_st item in the linked list for HTTP requests.
		/*!
			\brief Private structure for building HTTP requests.
			
			This is a private structure containing information received from other application
			parts so that it may be used in the BuildRequest() function to send/receive data
			with the remote server. This structure is of very limited use, so is not accessible
			outside of the HTTPv4 class.
		*/
		struct build_request_st
		{
			HTTPv4 *http;
			const char *url;
			int32 request_method;
			const char *referring_url;
			uint8 redirection_counter;
			int32 site_id;
			int32 url_id;
			auth_info_st *auth_info;
			BMessage *post_data;
			build_request_st()
			{
				post_data=NULL;
				auth_info=NULL;
				http=NULL;
				request_method=0;
				referring_url=NULL;
				redirection_counter=0;
				site_id=0;
				url_id=0;
				url=NULL;
			}
		};
	
		/*!
			\brief BuildRequest thread launch function.
		*/
		static status_t BuildRequest_th(void *arg);
		/*!
			\brief This function builds the transaction requests before adding them to the http_request_info_st linked list.
		*/
		void BuildRequest(build_request_st *build_request_info);
		thread_id build_request_thid;//!< The thread ID of the current build request thread.
		
		/*!
			\brief The launch function for the HTTP protocol layer manager.
		*/
		static status_t start_layer_manager(void *arg);
		/*!
			\brief The HTTP protocol layer manager; the heart of HTTP.
			
			This function analyzes the state of any given http_request_info_st and orders the
			necessary actions to be taken. For instance, if the request is marked for deletion,
			the layer_manager is the function that actually deletes the request.
		*/
		void layer_manager(HTTPv4 *http);
		thread_id layer_manager_thid;//!< Layer manager thread ID.
		
		BList *request_queue;//!< BList object containing build_request_st records to be converted into http_request_info_st records by the BuildRequest() function.
		AuthManager *auth_manager;//!< Pointer to the authorization/authentication manager.
		int32 _terminate_;//!< A flag to terminate all processing in HTTP, used to prepare for shutdown.
		/*!
			\brief Sets the various variables and flags to a known state.
		*/
		void Init();
		bool pipelining;//!< Determines if more than one transaction should be attempted per server connection.
		/*!
		\brief Process received data.
		
		This function does the general processing of the data received from the server.
		It identifies the HTTP header block, separates it for processing in ProcessHeadersGeneral(),
		and then passes control on to ProcessData2(), which handles transfer and content
		encoding.
		*/
		void ProcessData(http_request_info_st *request,unsigned char *buffer, int32 length);
		/*!
		\brief Processed received data phase 2.
		
		This function interprets data received based on transfer and content encoding.
		*/
		void ProcessData2(http_request_info_st *request,unsigned char *buffer, int32 length);
		/*!
		\brief Processes individual chunks of a chunk transfer encoded file.
		
		This function places newly received data in the temporary_data_buffer variable,
		resizing the allocated memory as necessary for further processing in ProcessChunkedData.
		In short, this is phase 1 of a 2 phase process for processing chunked transfer encoded
		files.
		*/
		void ProcessChunk(http_request_info_st *request, unsigned char *buffer, int32 length);
		/*!
			\brief Process chunked transfer encoded files into normalized data.
			
			This function does the majority of the work necessary to store data sent to
			the client in the chunked transfer encoding in its original format.
		*/
		void ProcessChunkedData(http_request_info_st *request);
		/*!
			\brief This function processes HTTP headers into a linked list of header fields.
			
			This function processes the HTTP headers returned by the server into fields
			that are easily interpreted. It also is responsible for calling the CookieManager::SetCookie()
			function, and setting some of the more important variables needed for handling
			the connection.
		*/
		void ProcessHeadersGeneral(http_request_info_st *request, char *buffer, int32 length);
		/*!
		\brief Process received data with HTTP Status 100-199.
		
		HTTP 100 level response codes generally mean that the request was successful, but
		that there may be more information that is required or on its way. It may also
		be used in cases where the server needs more time to process the request.
		*/
		void ProcessHeaderLevel100(http_request_info_st *request, char *buffer, int32 length);
		/*!
		\brief Process received data with HTTP Status 200-299.
		
		HTTP 200 level response codes mean that the request was successful; but may have
		some specific actions that need to occur afterwards.
		*/
		void ProcessHeaderLevel200(http_request_info_st *request, char *buffer, int32 length);
		/*!
		\brief Process received data with HTTP Status 300-399.
		
		HTTP 300 level response codes generally mean that the document in question has
		been relocated.
		*/
		void ProcessHeaderLevel300(http_request_info_st *request, char *buffer, int32 length);
		/*!
		\brief Process received data with HTTP Status 400-499.
		
		HTTP 400 level response codes are usually browser or user errors, however they also
		include authentication requests.
		*/
		void ProcessHeaderLevel400(http_request_info_st *request, char *buffer, int32 length);
		/*!
		\brief Process received data with HTTP Status 500-599.
		
		HTTP 500 level response codes are usually server errors.
		*/
		void ProcessHeaderLevel500(http_request_info_st *request, char *buffer, int32 length);
		/*!
			\brief Finalizes http_request_info_st structures and HTTP requests.
			
			This function broadcasts a final summary of any given URL/HTTP request when
			the request has been completed. It also signals the TCPManager to terminate the connection
			if appropriate.
		*/
		void DoneWithRequest(http_request_info_st *request);
		/*!
			\brief Sends received data to either disk, or other parts of Themis.
			
			Depending on whether or not the cache system add-on is loaded or whether the
			server states that the data can be cached or not, the StoreData() function
			either saves the data to disk, or broadcasts it along with status information
			to other parts of the application. If the data is broadcast, it is listed
			as part of the BMessage as a B_RAW_TYPE field by the name of "raw_data". Otherwise
			the BMessage will have an int32 cache_object_token field containing the
			cache system reference number.
		*/
		void StoreData(http_request_info_st *request,unsigned char *buffer, int32 length);
		/*!
			\brief Decompresses data in the gzip, deflate, or compress content encodings.
			
			This function utilizes zlib 1.2.1 to automatically detect and decompress data
			received in the gzip, deflate, or compress content encodings.
		*/
		unsigned char *GUnzip(http_request_info_st *request,unsigned char *buffer, int32 offset,int32 length,int32 *out_length);
		friend class AuthManager;
		/*!
			\brief Is the data from the server compressed?
		*/
		bool Compressed(http_request_info_st *request);
	public:
		char *SettingsViewLabel();
		BView *SettingsView(BRect frame);
		/*!
			\brief Find the specified header for the given HTTP request.
			
			This function finds and returns the value for the specified header attribute.
			
			\note This function should really be expanded a bit to include support for
			finding the nth attribute header for those occasions where there is more than
			one. But for now, it works as is.
		*/
		const char *FindHeader(http_request_info_st *request,const char *attribute,bool case_sensitive=false);
		/*!
			\brief This variable selects the user agent string to use.
			
			USER_AGENT_THEMIS  "Themis (http://themis.sourceforge.net)"
			USER_AGENT_MOZILLA  "Mozilla/5.0 (BeOS; BePC; rv: 4.0) Themis http://themis.sourceforge.net)"
			USER_AGENT_INTERNETEXPLORER  "Mozilla/5.0 (compatible; MSIE 6.0; BeOS ; Themis http://themis.sourceforge.net)"
			USER_AGENT_CUSTOM  User provided string.
			USER_AGENT_NONE don't use a user agent string.
		*/
		uint32 use_useragent;
		const char *custom_useragent;
		/*!
			\brief This function is called by the AuthManager to set specific proxy authentication information.
		*/
		void SetProxyAuthInfo(auth_info_st *proxy_auth);
		/*!
			\brief Resubmits a request to the server.
			
			This function is primarily used to set up for another attempt to retrieve data
			from the server. This is useful for the HTTP status 301 and 302 redirection
			requests to a different URL, and the HTTP status 401 and 407 requests for
			authentication information.
		*/
		void ResubmitRequest(http_request_info_st *request,const char *alt_url=NULL,int32 request_method=-1);
		/*!
			\brief The constructor.
		*/
		HTTPv4(BMessage *info=NULL, const char *msg_sys_name="HTTP Protocol");
		/*!
			\brief The destructor.
		*/
		~HTTPv4();
		/*!
			\brief Returns the plug-in's name.
		*/
		char *PlugName(void);
		/*!
			\brief Returns the plug-in's ID code.
		*/
		uint32 PlugID(void);
		/*!
			\brief Returns the plug-in version number.
		*/
		float PlugVersion(void);
		/*!
			\brief Returns the plug-in type information.
		*/
		int32 Type(void);
		/*!
			\brief Legacy function; does nothing.
		*/
		status_t Go(void);
		/*!
			\brief Receives broadcast messages from the MessageSystem.
		*/
		status_t ReceiveBroadcast(BMessage *msg);
		/*!
			\brief Handles replies to broadcasts from the HTTP protocol.
		*/
		status_t BroadcastReply(BMessage *msg);
		/*!
			\brief The broadcast ID code for the MessageSystem.
		*/
		uint32 BroadcastTarget();
		/*!
			\brief The heartbeat function.
			
			This function processes build_request_st items that couldn't be processed
			at the time that the original request was made. This function also checks
			to make sure that the HTTP layer_manager is still running. If it isn't, and
			_terminate_ is not set to non-zero, then it launches a new layer_manager thread.
		*/
		void Heartbeat();
		
		bool IsPersistent();
		void Stop();
		/*!
			\brief Legacy function; does nothing.
		*/
		int32 GetURL(BMessage *info);
		/*!
			\brief Legacy function; does nothing. At the moment.
		*/
		void AddMenuItems(BMenu *menu);
		/*!
			\brief Legacy function; does nothing. At the moment.
		*/
		void RemoveMenuItems();
		/*!
			\brief Supported mime types structure link list.
		*/
		struct smt_st *smthead;
		/*!
			\brief See NetworkableObject::ConnectionEstablished.
		*/
		uint32 ConnectionEstablished(Connection *connection);
		/*!
			\brief See NetworkableObject::ConnectionAlreadyExists.
		*/
		uint32 ConnectionAlreadyExists(Connection *connection);
		/*!
			\brief See NetworkableObject::ConnectionTerminated.
		*/
		uint32 ConnectionTerminated(Connection *connection);
		/*!
			\brief See NetworkableObject::DataIsWaiting.
		*/
		uint32 DataIsWaiting(Connection *connection);
		/*!
			\brief See NetworkableObject::ConnectionError.
		*/
		uint32 ConnectionError(Connection *connection);
		/*!
			\brief See NetworkableObject::ConnectionFailed.
		*/
		uint32 ConnectionFailed(Connection *connection);
		/*!
			\brief See NetworkableObject::DestroyingConnectionObject.
		*/
		uint32 DestroyingConnectionObject(Connection *connection);
		friend class CookieManager;
		friend struct http_request_info_st;
		const static int32 HTTP_REQUEST_TYPE_GET='htgt';
		const static int32 HTTP_REQUEST_TYPE_HEAD='hthd';
		const static int32 HTTP_REQUEST_TYPE_DELETE='htdl';
		const static int32 HTTP_REQUEST_TYPE_POST='htpo';
		const static int32 HTTP_REQUEST_TYPE_PUT='htpu';
		const static uint32 STATUS_REQUEST_BUILT=0x01;
		const static uint32 STATUS_CONNECTED_TO_SERVER=0x10;
		const static uint32 STATUS_REQUEST_SENT_TO_SERVER=0x20;
		const static uint32 STATUS_RESPONSE_RECEIVED_FROM_SERVER=0x40;
		const static uint32 STATUS_NO_DATA_FROM_SERVER=0x80;
		const static uint32 STATUS_CONNECTION_CLOSED=0x0100;
		const static uint32 STATUS_CONNECTION_BEING_REDIRECTED=0x0200;
		const static uint32 STATUS_CONNECTION_REQUIRES_AUTHENTICATION=0x0400;
		const static uint32 STATUS_AUTHENTICATION_RECEIVED=0x0800;
		const static uint32 STATUS_SERVER_NOT_FOUND=0x1000;
		const static uint32 STATUS_RECEIVING_DATA=0x2000;
		const static uint32 STATUS_REQUEST_COMPLETE=0x4000;
		const static uint32 STATUS_REQUEST_INTERRUPTED=0x8000;
		const static uint32 STATUS_NO_RESPONSE_FROM_SERVER=0x10000;
		const static uint32 STATUS_HEADERS_STARTED=0x20000;
		const static uint32 STATUS_HEADERS_COMPLETED=0x40000;
		const static uint32 STATUS_DELETE_REQUEST=0x80000;
		const static uint32 STATUS_PROCESSING_TIMED_OUT=0x02;
		const static uint32 STATUS_WAITING_ON_AUTH_INFO=0x04;
		
		const static uint32 USER_AGENT_NONE=0x0;
		const static uint32 USER_AGENT_THEMIS=0x1;
		const static uint32 USER_AGENT_MOZILLA=0x2;
		const static uint32 USER_AGENT_INTERNETEXPLORER=0x3;
		const static uint32 USER_AGENT_CUSTOM=0x4;
	
		const static uint32 TRANSFER_STATE_CHUNKED=0x01;
		const static uint32 TRANSFER_STATE_ENCODED_GZIP=0x02;
		const static uint32 TRANSFER_STATE_ENCODED_COMPRESS=0x04;
		const static uint32 TRANSFER_STATE_ENCODED_DEFLATE=0x04;
		
		const static uint32 CACHE_STATE_NOT_IN_CACHE=0x01;
		const static uint32 CACHE_STATE_ITEM_IS_CACHED=0x02;
		const static uint32 CACHE_STATE_CACHE_ITEM_UPDATED=0x04;
		const static uint32 CACHE_STATE_DONT_CACHE=0x08;
		const static uint32 CACHE_STATE_ATTRIBUTES_UPDATED=0x10;
};

#endif
