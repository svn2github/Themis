/*
 * UrlEntry.h
 */

#ifndef URLENTRY_H
#define URLENTRY_H

#include "BaseEntry.hpp"

class UrlEntry : public BaseEntry {

	public:
		UrlEntry(int32 id,
				 const char * url);

		int GetLoadingProgress();
		bool GetSecureConnection();
		const char * GetTitle();
		
		const char * GetUrl();
		void Print();
		void SetLoadingProgress(int loadingprogress);
		void SetSecureConnection(bool value);
		void SetTitle(const char * title);
									
};

#endif
