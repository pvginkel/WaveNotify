

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Sun Jan 17 16:50:48 2010
 */
/* Compiler settings for .\chrome_tab.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IChromeFrame,0xFB243E4B,0x8AC2,0x4840,0x95,0xF2,0x91,0xB9,0xAF,0x9C,0xFF,0x10);


MIDL_DEFINE_GUID(IID, IID_IChromeFramePrivileged,0x655A11E0,0xEF63,0x4fbe,0x9D,0xF6,0xC1,0x82,0xD2,0xFC,0xD6,0xDC);


MIDL_DEFINE_GUID(IID, LIBID_ChromeTabLib,0x6F2664E1,0xFF6E,0x488A,0xBC,0xD1,0xF4,0xCA,0x60,0x01,0xDF,0xCC);


MIDL_DEFINE_GUID(IID, DIID_DIChromeFrameEvents,0x388B5D64,0xCE67,0x415b,0x9B,0x0F,0x74,0x5C,0x56,0xE8,0x68,0xE7);


MIDL_DEFINE_GUID(CLSID, CLSID_HtmlFilter,0xBB1176EE,0x20DD,0x41DC,0x9D,0x1E,0xAC,0x13,0x35,0xC7,0xBB,0xB0);


MIDL_DEFINE_GUID(CLSID, CLSID_ChromeProtocol,0x9875BFAF,0xB04D,0x445E,0x8A,0x69,0xBE,0x36,0x83,0x8C,0xDE,0x3E);


MIDL_DEFINE_GUID(CLSID, CLSID_ChromeActiveDocument,0x3E1D0E7F,0xF5E3,0x44CC,0xAA,0x6A,0xC0,0xA6,0x37,0x61,0x9A,0xB8);


MIDL_DEFINE_GUID(CLSID, CLSID_ChromeFrame,0xE0A900DF,0x9611,0x4446,0x86,0xBD,0x4B,0x1D,0x47,0xE7,0xDB,0x2A);


MIDL_DEFINE_GUID(CLSID, CLSID_ChromeFrameBHO,0xECB3C477,0x1A0A,0x44bd,0xBB,0x57,0x78,0xF9,0xEF,0xE3,0x4F,0xA7);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



