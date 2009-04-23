/*
 * PrefsViews.cpp
 *
 * author: Michael Weirauch
 */

#ifndef _PREFSDEFS_H_
#define _PREFSDEFS_H_

extern BMessage* AppSettings;

/*
 * local prefs win message constants
 */

const uint32 HOMEPAGE_CHANGED			= 'hpch';
const uint32 LIST_SELECTION_CHANGED		= 'lsch';
const uint32 SET_BLANK_PAGE				= 'sblp';
const uint32 IZ_CHECKBOX				= 'imcb';
const uint32 SHOWTYPEAHEAD_CHECKBOX		= 'stcb';
const uint32 NEW_WINDOW_START_PAGE		= 'nwsp';
const uint32 SHOWTABS_CHECKBOX			= 'shtb';
const uint32 TABSBACKGROUND_CHECKBOX	= 'tabb';
const uint32 TABSBLANK_CHECKBOX			= 'tabl';
const uint32 HISTORY_CHANGED			= 'hich';
const uint32 HISTORY_CLEAR				= 'ghcl';
const uint32 DTD_SELECTED				= 'dtds';
const uint32 CSS_SELECTED				= 'csss';


/*
 * global string constants for the AppSettings message
 */

/* General */
const char* const kPrefsSettingsDirectory			= "PrefsSettingsDirectory";
const char* const kPrefsSettingsFilePath			= "PrefsSettingsFile";
const char* const kPrefsMainWindowRect				= "PrefsMainWindowRect";

/* Global History */
const char* const kPrefsGlobalHistoryData			= "PrefsGlobalHistoryData";
const char* const kPrefsGlobalHistoryItemMessage	= "PrefsGlobalHistoryItemMessage";

/* Prefs General */
const char* const kPrefsLastSelectedItem			= "PrefsLastSelectedItem";
const char* const kPrefsPrefWindowPoint				= "PrefsPrefWindowPoint";

/* Prefs Window */
const char* const kPrefsHomePage					= "PrefsHomePage";
const char* const kPrefsIntelligentZoom				= "PrefsIntelligentZoom";
const char* const kPrefsShowTypeAheadWindow			= "PrefsShowTypeAheadWindow";
const char* const kPrefsShowTabsAtStartup			= "PrefsShowTabsAtStartup";
const char* const kPrefsOpenTabsInBackground		= "PrefsOpenTabsInBackground";
const char* const kPrefsNewWindowStartPage			= "PrefsNewWindowStartPage";
const char* const kPrefsOpenBlankTargetInTab		= "PrefsOpenBlankTargetInTab";

/* Prefs Privacy */
const char* const kPrefsGlobalHistoryDepthInDays	= "PrefsGlobalHistoryDepthInDays";
const char* const kPrefsGlobalHistoryFreeURLCount	= "PrefsGlobalHistoryFreeURLCount";
const char* const kPrefsTabHistoryDepth				= "PrefsTabHistoryDepth";

/* Prefs HTML Parser */
const char* const kPrefsDTDDirectory				= "PrefsDTDDirectory";
const char* const kPrefsActiveDTDPath				= "PrefsActiveDTDPath";

/* Prefs CSS Parser */
const char* const kPrefsCSSDirectory				= "PrefsCSSDirectory";
const char* const kPrefsActiveCSSPath				= "PrefsActiveCSSPath";

/*
 * helper struct
 */

enum prefs_view_const
{
	PREFSVIEW_WINDOW = 0,
	PREFSVIEW_NETWORK,
//	PREFSVIEW_CACHE,
	PREFSVIEW_HTMLPARSER,
	PREFSVIEW_RENDERER,
	PREFSVIEW_PRIVACY,
	PREFSVIEW_CSSPARSER
};

struct prefs_item_info
{
	const char* name;
	const uint8* bitmap;
	enum prefs_view_const pvconst;
};

#endif
