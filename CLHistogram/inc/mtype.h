/****************************************************************************
* File: mtype.h                                                             *
* Description: Global type definition                                       *
*   Please don't add new definition arbitrarily.                            *
*   Only system-member is allowed to modify this file.                      *
****************************************************************************/

#ifndef _MTYPE_H_
#define _MTYPE_H_

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

// Global constant

#ifndef TRUE
#define TRUE                    1
#endif

#ifndef FALSE
#define FALSE                   0
#endif

#ifndef NULL
#define NULL                    ((void *)0)
#endif


// Global data type

typedef float                   FLOAT32;
typedef double                  FLOAT64;

typedef char                    CHAR;
typedef char             SINT8; // The INT8 has bug. In fact it is "unsigned char". Use SINT8 to indicate "signed char".
typedef char                    INT8;
typedef unsigned char           UINT8;
typedef short                   INT16;
typedef unsigned short          UINT16;
typedef int                     INT32;
typedef unsigned int            UINT32;

#ifdef WIN32
typedef __int64                 INT64;
typedef unsigned __int64        UINT64;
#else
typedef long long               INT64;
typedef unsigned long long      UINT64;
#endif

typedef UINT8                   BOOL; 
typedef UINT32                  ERRCODE;
typedef void                    *PVOID;
typedef char                    *PSTR;

typedef PVOID                   HWND;
typedef UINT16                  WPARAM;
typedef UINT32                  LPARAM;
typedef UINT32                  HANDLE;

typedef void (*CALLBACK)();


#endif //_MTYPE_H_
