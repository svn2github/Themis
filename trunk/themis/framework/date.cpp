#include "date.h"
#include <kernel/OS.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
/*
From RFC 2822:
3.3. Date and Time Specification

   Date and time occur in several header fields.  This section specifies
   the syntax for a full date and time specification.  Though folding
   white space is permitted throughout the date-time specification, it
   is RECOMMENDED that a single space be used in each place that FWS
   appears (whether it is required or optional); some older
   implementations may not interpret other occurrences of folding white
   space correctly.

date-time       =       [ day-of-week "," ] date FWS time [CFWS]

day-of-week     =       ([FWS] day-name) / obs-day-of-week

day-name        =       "Mon" / "Tue" / "Wed" / "Thu" /
                        "Fri" / "Sat" / "Sun"

date            =       day month year

year            =       4*DIGIT / obs-year

month           =       (FWS month-name FWS) / obs-month

month-name      =       "Jan" / "Feb" / "Mar" / "Apr" /
                        "May" / "Jun" / "Jul" / "Aug" /
                        "Sep" / "Oct" / "Nov" / "Dec"

day             =       ([FWS] 1*2DIGIT) / obs-day

time            =       time-of-day FWS zone

time-of-day     =       hour ":" minute [ ":" second ]

hour            =       2DIGIT / obs-hour

minute          =       2DIGIT / obs-minute

second          =       2DIGIT / obs-second

zone            =       (( "+" / "-" ) 4DIGIT) / obs-zone





Resnick                     Standards Track                    [Page 14]

RFC 2822                Internet Message Format               April 2001


   The day is the numeric day of the month.  The year is any numeric
   year 1900 or later.

   The time-of-day specifies the number of hours, minutes, and
   optionally seconds since midnight of the date indicated.

   The date and time-of-day SHOULD express local time.

   The zone specifies the offset from Coordinated Universal Time (UTC,
   formerly referred to as "Greenwich Mean Time") that the date and
   time-of-day represent.  The "+" or "-" indicates whether the
   time-of-day is ahead of (i.e., east of) or behind (i.e., west of)
   Universal Time.  The first two digits indicate the number of hours
   difference from Universal Time, and the last two digits indicate the
   number of minutes difference from Universal Time.  (Hence, +hhmm
   means +(hh * 60 + mm) minutes, and -hhmm means -(hh * 60 + mm)
   minutes).  The form "+0000" SHOULD be used to indicate a time zone at
   Universal Time.  Though "-0000" also indicates Universal Time, it is
   used to indicate that the time was generated on a system that may be
   in a local time zone other than Universal Time and therefore
   indicates that the date-time contains no information about the local
   time zone.

   A date-time specification MUST be semantically valid.  That is, the
   day-of-the-week (if included) MUST be the day implied by the date,
   the numeric day-of-month MUST be between 1 and the number of days
   allowed for the specified month (in the specified year), the
   time-of-day MUST be in the range 00:00:00 through 23:59:60 (the
   number of seconds allowing for a leap second; see [STD12]), and the
   zone MUST be within the range -9959 through +9959.


*/

Date::Date(int32 timestamp)
{
	Init();
	unix_timestamp=timestamp;
}
Date::Date(void)
{
	Init();
}
Date::Date(const char *date)
{
	Init();
	SetDate(date);
}
Date::Date(Date *date)
{
	Init();
	SetDate(date);
}
Date::~Date(void)
{
	unix_timestamp=0l;
}
void Date::Init(void)
{
	unix_timestamp=real_time_clock();
}
int32 Date::Age(void)
{
	return (real_time_clock()-unix_timestamp);
}
int32 Date::Age(int32 timestamp)
{
	return (timestamp-unix_timestamp);
}
int32 Date::Age(Date *date)
{
	return (date->unix_timestamp-unix_timestamp);
}
int32 Date::SetDate(const char *date)
{
	return (unix_timestamp=ParseDate(date));
}
int32 Date::ParseDate(const char *date)
{
	if (date==NULL)
		return B_ERROR;
//		printf("Date as received: %s\n",date);
	/*
		We need to handle folding whitespace as defined in RFC 2822, so lets take
		care of that first.
	*/
	int32 length=strlen(date);
	char *copy=new char[length+1];
	memset(copy,0,length+1);
	strcpy(copy,date);
	int32 fws_count=0;
	char *fws_p=NULL;
	fws_p=strstr(copy,"\r\n");
	while(fws_p!=NULL)
	{
		if (isspace(*(fws_p+2)))
		{
			fws_count++;
			memmove(fws_p,fws_p+2,strlen(fws_p+2)+1);//replace the fws with three spaces instead
		}
		fws_p=strstr(fws_p+2,"\r\n");
	}
//	printf("%ld instances of folding whitespace encountered\n",fws_count);
//	printf("modified copy of date string: %s\n",copy);
	/*
		We also need to handle comments as defined in RFC 2822.
	*/
	char *open_p=strchr(copy,'('),*close_p=NULL;
	int32 comment_count=0;
	while (open_p!=NULL)
	{
		close_p=strchr(open_p,')');
		if (close_p!=NULL)
		{
			comment_count++;
			int32 remainder=strlen(close_p+1);
			memmove(open_p,close_p+1,remainder+1);
		}
		open_p=strchr(open_p,'(');
	}
	/*
		Some date strings used in cookies have the nerve to have hyphens between
		the day, month, and year... The audacity! Convert them to space characters!
	*/
	char *hyphen=strchr(copy,'-');
	while (hyphen!=NULL)
	{
		*hyphen=0x20;
		hyphen=strchr(hyphen,'-');
	}
//	printf("%ld comments encountered\n",comment_count);
//	printf("modified copy of date string: %s\n",copy);
	/*
		Now proceed to analyze the date...
	*/
	char *start_p=copy,*comma=strchr(copy,',');
	//If we have a comma, that means there's a day of the week expression starting things off. Skip it.
	if (comma!=NULL)
		start_p=(comma+1);
	/*
		There are five key sections that will need to be isolated and analyzed:
		1	Day of the month.
		2	Month of the year.
		3	Year.
		4	Time of day.
		5	Time zone.
		So divide the string up into it's sections.
	*/
	char *sections[5]={NULL,NULL,NULL,NULL,NULL};
	int32 lengths[5]={0,0,0,0,0};
	int32 section_counter=0;
	for (;section_counter<5; )
	{
		if ((*start_p)==(char)NULL)
			break;
		if (isspace(*start_p))
		{
			start_p++;
			continue;
		}
		sections[section_counter]=start_p;
		while(!isspace(*start_p))
		{
			if ((*start_p)==(char)NULL)
				break;
			start_p++;
			lengths[section_counter]++;
		}
//		printf("section %d is %d bytes long\n",section_counter+1,lengths[section_counter]);
		section_counter++;
	}
//	char *sn[5]={"Day","Month","Year","Time","Time Zone"};
	char *value;
	/*
		Move the data into a time structure.
	*/
	struct tm tstruct;
	for (int i=0; i<5; i++)
	{
		value=new char[lengths[i]+1];
		memset(value,0,lengths[i]+1);
		strncpy(value,sections[i],lengths[i]);
//		printf("\t%s:\t%s\n",sn[i],value);
		switch(i)
		{
			case 0:
			{
				tstruct.tm_mday=atoi(value);
			}break;
			case 1:
			{
				if (strcasecmp(value,"jan")==0)
					tstruct.tm_mon=0;
				if (strcasecmp(value,"feb")==0)
					tstruct.tm_mon=1;
				if (strcasecmp(value,"mar")==0)
					tstruct.tm_mon=2;
				if (strcasecmp(value,"apr")==0)
					tstruct.tm_mon=3;
				if (strcasecmp(value,"may")==0)
					tstruct.tm_mon=4;
				if (strcasecmp(value,"jun")==0)
					tstruct.tm_mon=5;
				if (strcasecmp(value,"jul")==0)
					tstruct.tm_mon=6;
				if (strcasecmp(value,"aug")==0)
					tstruct.tm_mon=7;
				if (strcasecmp(value,"sep")==0)
					tstruct.tm_mon=8;
				if (strcasecmp(value,"oct")==0)
					tstruct.tm_mon=9;
				if (strcasecmp(value,"nov")==0)
					tstruct.tm_mon=10;
				if (strcasecmp(value,"dec")==0)
					tstruct.tm_mon=11;
			}break;
			case 2:
			{
				if (lengths[i]==4)
				{
					tstruct.tm_year=atol(value)-1900;
				} else
				{
				
					int16 year=atol(value);
					/*
						The following is more technically correct, but in truth
						it is no different than just doing tstruct.tm_year=atol(value)
						provided that the length of the year is less than 4 but greater
						than 0.
					*/
					if ((lengths[i]==2) && (year<50) && (year>=0))
						year+=2000;
					else
						if (((lengths[i]==2) && (year>=50) && (year<100)) || (lengths[i]==3))
							year+=1900;
					tstruct.tm_year=year-1900;
				}
			}break;
			case 3:
			{
				char *time_array[3];
				time_array[0]=strtok(value,":");
				time_array[1]=strtok(NULL,":");
				time_array[2]=strtok(NULL,":");
//				printf("Hour: %s\nMinute: %s\nSeconds: %s\n",time_array[0],time_array[1],time_array[2]);
				tstruct.tm_hour=atoi(time_array[0]);
				tstruct.tm_min=atoi(time_array[1]);
				if (time_array[2]!=NULL)
					tstruct.tm_sec=atoi(time_array[2]);
			}break;
			case 4:
			{
				/*
					Adjust the time in the structure based on the time zone information to
					get GMT time...
				*/
				if ((ispunct(value[0]) && (strpbrk(value,"-+")==value)) && (strlen(value)==5))
				{
//					printf("Numeric time zone!\n");
					bool positive=false;
					int32 minutes=0,hours=0, zone=0;
					if (value[0]=='+')
						positive=true;
					zone=atoi((value+1));
					hours=zone/100;
					minutes=zone%100;
//					printf("This time is GMT %c %d hours %d minutes\n",value[0],hours,minutes);
					
					if (positive)
					{//if the time zone is positive, we need to subtract to get gmt
						tstruct.tm_gmtoff=-(3600*hours+60*minutes);
					} else
					{//if the time zone is negative, we need to add to get gmt
						tstruct.tm_gmtoff=(3600*hours+60*minutes);
					}
					
				} else
				{
//					printf("String time zone!\n");
					int32 offset=0,local_offset=0;
					time_t current_time=real_time_clock();
					struct tm *gmt=localtime(&current_time);
					if (gmt->tm_gmtoff<0)
						local_offset=-gmt->tm_gmtoff;
					else
						local_offset=gmt->tm_gmtoff;
					if (gmt->tm_isdst)
						local_offset+=3600;
//					printf("GMT offset of local machine: %d\n",gmt->tm_gmtoff);
					if ((strcasecmp("GMT",value)==0) || (strcasecmp("UT",value)==0))
					{//Then we need to find the GMT offset of the local machine
						printf("(GMT) local offset: %d\toffset: %d\n",local_offset,offset);
						local_offset-=3600;
					}
					/*
						Add more time zone strings and values as time and sanity permit.
					*/
					if (strcasecmp("edt",value)==0)//US Eastern Daylight Time
						offset=0x3840;
					if (strcasecmp("est",value)==0)//US Eastern Standard Time
						offset=0x4650;
					if (strcasecmp("cdt",value)==0)//US Central Daylight Time
						offset=0x4650;
					if (strcasecmp("cst",value)==0)//US Central Standard Time
						offset=0x5460;
					if (strcasecmp("mdt",value)==0)//US Mountain Daylight Time
						offset=0x5460;
					if (strcasecmp("mst",value)==0)//US Mountain Standard Time
						offset=0x6270;
					if (strcasecmp("pdt",value)==0)//US Pacific Daylight Time
						offset=0x6270;
					if (strcasecmp("pst",value)==0)//US Pacific Standard Time
						offset=0x7080;
//						printf("local offset: %d\nother offset: %d\n",local_offset,offset);
//					if (offset==0)
//						offset=local_offset;
					offset=local_offset-offset;
//					printf("difference: %d\n",offset);
						int32 hours=offset/3600, minutes=(offset%3600)/60;
						tstruct.tm_hour-=hours;
						tstruct.tm_min-=minutes;
					
				}
			}break;
		}
		memset(value,0,lengths[i]+1);
		delete value;	
	}
	time_t ttime=mktime(&tstruct);
	ttime=mktime(localtime(&ttime));
//	printf("ctime() (localtime): %s\n",ctime(&ttime));
	char dt[100];
	memset(dt,0,100);
	strftime(dt,99,"%a %B %d, %Y %H:%M:%S %Z",&tstruct);
	printf("strftime (localtime): %s (%s)\n",dt,asctime(&tstruct));
	memset(dt,0,100);
//	struct tm *gmt=gmtime(&ttime);
//	strftime(dt,99,"%a %B %d, %Y %H:%M:%S %Z",gmt);
//	printf("strftime (GMT): %s\n",dt);
	memset(copy,0,length+1);
	delete copy;
	return ttime;
}
int32 Date::SetDate(Date *date)
{
	if (date==NULL)
		return B_ERROR;
	return (unix_timestamp=date->unix_timestamp);
}
int32 Date::SetDate(int32 timestamp)
{unix_timestamp=timestamp;
	printf("SetDate(timestamp): %s",ctime(&unix_timestamp));
	return unix_timestamp;
}
int32 Date::Timestamp(void)
{
	return unix_timestamp;
}
const char *Date::RFC2822(void)
{
	const char *date=new char[100];
	memset((char*)date,0,100);
					struct tm *gm=gmtime(&unix_timestamp);
	strftime((char*)date,99,"%a, %d %b %Y %H:%M:%S +0000",gm );
	return date;
}
const char *Date::RFC822(void)
{
	const char *date=new char[100];
	memset((char*)date,0,100);
					struct tm *gm=gmtime(&unix_timestamp);
	strftime((char*)date,99,"%a, %d %b %Y %H:%M:%S GMT",gm );
	return date;
}
