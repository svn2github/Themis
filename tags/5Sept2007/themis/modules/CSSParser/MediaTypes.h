#ifndef MEDIA_TYPES_H
#define MEDIA_TYPES_H
/* MediaTypes.h - at rule media types
**
** Copyright 2001, pinc Software. All Rights Reserved.
*/

enum media_types
{
	MEDIA_TYPE_AURAL	= 0x0001,
	MEDIA_TYPE_BRAILLE	= 0x0002,
	MEDIA_TYPE_EMBOSSED	= 0x0004,
	MEDIA_TYPE_HANDHELD	= 0x0008,
	MEDIA_TYPE_PRINT	= 0x0010,
	MEDIA_TYPE_PROJECTION = 0x0020,
	MEDIA_TYPE_SCREEN	= 0x0040,
	MEDIA_TYPE_TTY		= 0x0080,
	MEDIA_TYPE_TV		= 0x0100,
	
	MEDIA_ALL_TYPES		= 0xffff
};

#endif	/* MEDIA_TYPES_H */
