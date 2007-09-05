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
#ifndef _date_class_
#define _date_class_
#include <SupportDefs.h>
/*!
\brief Date parsing and interpretation class.

This class takes a unix timestamp (in <b>local time</b>)  or a RFC 822/2822 compliant
date string and converts it internally into a typical int32 unix style timestamp.
From there, you may do some age comparisons, get the timestamp, or a const char * string
that you may use elsewhere.
*/
class Date
{
	private:
		int32 unix_timestamp;
		void Init(void);
		int32 ParseDate(const char *date);
	public:
		/*!
			\brief Unix timestamp version of the constructor.
		*/
		Date(int32 timestamp);
		Date(void);
		Date(const char *date);
		Date(Date *date);
		~Date(void);
		int32 Age(void);
		/*!
			\brief Returns the difference in time between the timestamp provided and the one utilized by this object.
		*/
		int32 Age(int32 timestamp);
		/*!
			\brief Returns the difference in time between this object and the other one.
		*/
		int32 Age(Date *date);
		/*!
			\brief Set the time using an RFC 822 or 2822 compliant string.
		*/
		int32 SetDate(const char *date);
		/*!
			\brief Set the time using another Date object.
		*/
		int32 SetDate(Date *date);
		/*!
			\brief Set the time with a localized timestamp value.
		*/
		int32 SetDate(int32 timestamp);
		/*!
			\brief This function returns an int32 representation of the localized time.
			
			\retval int32 This return value is a localized time value. To get the GMT time,
			you must convert it with gmtime().
		*/
		int32 Timestamp(void);
		/*!
			\brief This function generates a RFC 2822 compliant string.
			
			Like the brief note says, this function will generate an RFC 2822 compliant
			date string.
			
			\retval "const char *" Either NULL or a string that you must delete when you're done with it.
			The value returned is always GMT/UT relative.
		*/
		const char *RFC2822(void);
		/*!
			\brief This function generates a RFC 822 compliant string.
			
			Like the brief note says, this function will generate an RFC 822 compliant
			date string.
			
			\retval "const char *" Either NULL or a string that you must delete when you're done with it.
			The value returned is always GMT/UT relative.
		*/
		const char *RFC822(void);
};

#endif
