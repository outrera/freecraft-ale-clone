/*
 * This source code is public domain.
 *
 * Authors: Rani Assaf <rani@magic.metawire.com>,
 *          Olivier Lapicque <olivierl@jps.net>,
 *          Adam Goode       <adam@evdebs.org> (endian and char fixes for PPC)
 *
 *	$Id: stdafx.h,v 1.18 2002/12/22 15:19:42 ariclone Exp $
*/


#ifndef _STDAFX_H_
#define _STDAFX_H_

///////////////////////////////////////////////////////////////////////////////
//	CONFIG - PART
///////////////////////////////////////////////////////////////////////////////

    /**
    **	FIXME: whats this?
    */
#define noMODPLUG_TRACKER
    /**
    **	Only include the basic mod support .xm, .it, .s3m, .wav, .mod
    */
#define noMODPLUG_BASIC_SUPPORT
    /**
    **	FIXME: whats this?
    */
#define noMODPLUG_FASTSOUNDLIB
    /**
    **	FIXME: whats this?
    */
#define MODPLUG_MMCMP_SUPPORT
    /**
    **	FIXME: whats this?
    */
#define MODPLUG_NO_IMIXPLUGIN
    /**
    **	FIXME: whats this?
    */
#define MODPLUG_NO_PACKING
    /**
    **	FIXME: whats this?
    */
#define MODPLUG_NO_FILESAVE

#if defined(_MSC_VER) // || defined(__MINGW32__) // {

#ifndef _WIN32_WCE
#include "freecraft.h"
#endif

#ifndef __MINGW32__
#pragma warning (disable:4201)
#pragma warning (disable:4514)
#endif
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __MINGW32__
#define MODPLUG_NO_AGC 1
//    typedef unsigned char uint8_t; 	typedef signed char int8_t;
//    typedef unsigned short uint16_t;	typedef signed short int16_t;
//    typedef unsigned int uint32_t; 	typedef signed int int32_t;
#endif

#ifndef __MINGW32__
inline void ProcessPlugins(int n) {}
#endif

#else	// }{ _MSC_VER

#include <stdlib.h>
#include <stdio.h>

#ifdef BSD
#include <inttypes.h>
#else
#include <stdint.h>
#endif // BSD

#include <string.h>

extern inline long MulDiv (long a, long b, long c)
{
  // if (!c) return 0;
  return ((unsigned long long) a * (unsigned long long) b ) / c;
}

#ifdef __GNUC__

#ifndef __cdecl
#define __cdecl
#endif
#ifndef __declspec
#define __declspec(BLAH)
#endif

#if __GNUC__>=3 

#if __GNUC__==3 && __GNUC_MINOR__>=2 || __GNUC__>3
#define __GCC32__MAYBE_OK__ 1
#endif

#if !defined(__GCC32__MAYBE_OK__) && !defined(__I_KNOW_THAT_GNUC_3_IS_UNSUPPORTED__)
#warning "GNU GCC 3.xx is not supported"
#endif

//	It looks that GCC 3.xx is becoming nutty:
//	__FUNCTION__	can't be concated in the future.
//	__func__	Is defined by ISO C99 as
//		static const char __func__[] = "function-name";
#define __FUNCTION__ "Wrong compiler:"__FILE__

#endif
#endif

#ifdef _MSC_VER	// { m$ auto detection
#define PrintFunction() \
    do { fprintf(stdout,"%s:%d: ",__FILE__,__LINE__); } while(0)

#else // } m$ detection {

#if defined(__GCC32__MAYBE_OK__)
#define PrintFunction() do { fprintf(stdout,"%s: ", __func__); } while(0)
#else
#ifndef PrintFunction
    /// Print function in debug macros
#define PrintFunction() do { fprintf(stdout,__FUNCTION__": "); } while(0)
#endif
#endif
#endif	// } m$

#define MODPLUG_NO_AGC
#define WAVE_FORMAT_PCM 1
//#define ENABLE_EQ

#define  GHND   0

#ifdef __cplusplus
static inline signed char * GlobalAllocPtr(unsigned int, size_t size)
#else
static inline signed char * GlobalAllocPtr(unsigned int dummy __attribute__((unused)), size_t size)
#endif
{
  signed char * p = (signed char *) malloc(size);

  if (p != NULL) memset(p, 0, size);
  return p;
}

#ifdef __cplusplus
#define __attribute__(dummy)	/* dummy */
static inline void ProcessPlugins(int) {}
#else
static inline void ProcessPlugins(int dummy __attribute__((unused))) {}
#endif

#define GlobalFreePtr(p) free((void *)(p))

#define strnicmp(a,b,c)		strncasecmp(a,b,c)

#endif // } !_MSC_VER
#endif
