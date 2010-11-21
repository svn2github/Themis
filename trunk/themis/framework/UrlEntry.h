/*
 * UrlEntry.h
 */

#ifndef URLENTRY_H
#define URLENTRY_H

// Declarations used
class BString;

class UrlEntry {

	private:
		int32 fID;
		int8 fLoadingProgress;
		BString * fUrl;
		BString * fTitle;
		bool fSecureConnection;

	public:
		UrlEntry(int32 id,
				 const char* url);
		~UrlEntry();

		int32 GetID();
		int8 GetLoadingProgress();
		bool GetSecureConnection();
		const char * GetTitle();
		
		const char * GetUrl();
		void Print();
		void SetLoadingProgress(int8 loadingprogress);
		void SetSecureConnection(bool value);
		void SetTitle(const char * title);
									
};

#endif
