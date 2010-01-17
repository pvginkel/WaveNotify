

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Sun Jan 17 12:11:43 2010
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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __chrome_tab_h_h__
#define __chrome_tab_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IChromeFrame_FWD_DEFINED__
#define __IChromeFrame_FWD_DEFINED__
typedef interface IChromeFrame IChromeFrame;
#endif 	/* __IChromeFrame_FWD_DEFINED__ */


#ifndef __IChromeFramePrivileged_FWD_DEFINED__
#define __IChromeFramePrivileged_FWD_DEFINED__
typedef interface IChromeFramePrivileged IChromeFramePrivileged;
#endif 	/* __IChromeFramePrivileged_FWD_DEFINED__ */


#ifndef __DIChromeFrameEvents_FWD_DEFINED__
#define __DIChromeFrameEvents_FWD_DEFINED__
typedef interface DIChromeFrameEvents DIChromeFrameEvents;
#endif 	/* __DIChromeFrameEvents_FWD_DEFINED__ */


#ifndef __HtmlFilter_FWD_DEFINED__
#define __HtmlFilter_FWD_DEFINED__

#ifdef __cplusplus
typedef class HtmlFilter HtmlFilter;
#else
typedef struct HtmlFilter HtmlFilter;
#endif /* __cplusplus */

#endif 	/* __HtmlFilter_FWD_DEFINED__ */


#ifndef __ChromeProtocol_FWD_DEFINED__
#define __ChromeProtocol_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChromeProtocol ChromeProtocol;
#else
typedef struct ChromeProtocol ChromeProtocol;
#endif /* __cplusplus */

#endif 	/* __ChromeProtocol_FWD_DEFINED__ */


#ifndef __ChromeActiveDocument_FWD_DEFINED__
#define __ChromeActiveDocument_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChromeActiveDocument ChromeActiveDocument;
#else
typedef struct ChromeActiveDocument ChromeActiveDocument;
#endif /* __cplusplus */

#endif 	/* __ChromeActiveDocument_FWD_DEFINED__ */


#ifndef __ChromeFrame_FWD_DEFINED__
#define __ChromeFrame_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChromeFrame ChromeFrame;
#else
typedef struct ChromeFrame ChromeFrame;
#endif /* __cplusplus */

#endif 	/* __ChromeFrame_FWD_DEFINED__ */


#ifndef __ChromeFrameBHO_FWD_DEFINED__
#define __ChromeFrameBHO_FWD_DEFINED__

#ifdef __cplusplus
typedef class ChromeFrameBHO ChromeFrameBHO;
#else
typedef struct ChromeFrameBHO ChromeFrameBHO;
#endif /* __cplusplus */

#endif 	/* __ChromeFrameBHO_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IChromeFrame_INTERFACE_DEFINED__
#define __IChromeFrame_INTERFACE_DEFINED__

/* interface IChromeFrame */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IChromeFrame;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FB243E4B-8AC2-4840-95F2-91B9AF9CFF10")
    IChromeFrame : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_src( 
            /* [retval][out] */ BSTR *src) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_src( 
            /* [in] */ BSTR src) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE postMessage( 
            /* [in] */ BSTR message,
            /* [optional][in] */ VARIANT target) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_onload( 
            /* [retval][out] */ VARIANT *onload_handler) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_onload( 
            /* [in] */ VARIANT onload_handler) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_onloaderror( 
            /* [retval][out] */ VARIANT *onerror_handler) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_onloaderror( 
            /* [in] */ VARIANT onerror_handler) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_onmessage( 
            /* [retval][out] */ VARIANT *onmessage_handler) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_onmessage( 
            /* [in] */ VARIANT onmessage_handler) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_readyState( 
            /* [retval][out] */ long *ready_state) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE addEventListener( 
            /* [in] */ BSTR event_type,
            /* [in] */ IDispatch *listener,
            /* [optional][in] */ VARIANT use_capture) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE removeEventListener( 
            /* [in] */ BSTR event_type,
            /* [in] */ IDispatch *listener,
            /* [optional][in] */ VARIANT use_capture) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_version( 
            /* [retval][out] */ BSTR *version) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE postPrivateMessage( 
            /* [in] */ BSTR message,
            /* [in] */ BSTR origin,
            /* [in] */ BSTR target) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_useChromeNetwork( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_useChromeNetwork( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE installExtension( 
            /* [in] */ BSTR crx_path) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE loadExtension( 
            /* [in] */ BSTR extension_path) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IChromeFrameVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IChromeFrame * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IChromeFrame * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IChromeFrame * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IChromeFrame * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IChromeFrame * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IChromeFrame * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IChromeFrame * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_src )( 
            IChromeFrame * This,
            /* [retval][out] */ BSTR *src);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_src )( 
            IChromeFrame * This,
            /* [in] */ BSTR src);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *postMessage )( 
            IChromeFrame * This,
            /* [in] */ BSTR message,
            /* [optional][in] */ VARIANT target);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_onload )( 
            IChromeFrame * This,
            /* [retval][out] */ VARIANT *onload_handler);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_onload )( 
            IChromeFrame * This,
            /* [in] */ VARIANT onload_handler);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_onloaderror )( 
            IChromeFrame * This,
            /* [retval][out] */ VARIANT *onerror_handler);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_onloaderror )( 
            IChromeFrame * This,
            /* [in] */ VARIANT onerror_handler);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_onmessage )( 
            IChromeFrame * This,
            /* [retval][out] */ VARIANT *onmessage_handler);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_onmessage )( 
            IChromeFrame * This,
            /* [in] */ VARIANT onmessage_handler);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IChromeFrame * This,
            /* [retval][out] */ long *ready_state);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *addEventListener )( 
            IChromeFrame * This,
            /* [in] */ BSTR event_type,
            /* [in] */ IDispatch *listener,
            /* [optional][in] */ VARIANT use_capture);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *removeEventListener )( 
            IChromeFrame * This,
            /* [in] */ BSTR event_type,
            /* [in] */ IDispatch *listener,
            /* [optional][in] */ VARIANT use_capture);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_version )( 
            IChromeFrame * This,
            /* [retval][out] */ BSTR *version);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE *postPrivateMessage )( 
            IChromeFrame * This,
            /* [in] */ BSTR message,
            /* [in] */ BSTR origin,
            /* [in] */ BSTR target);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_useChromeNetwork )( 
            IChromeFrame * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_useChromeNetwork )( 
            IChromeFrame * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE *installExtension )( 
            IChromeFrame * This,
            /* [in] */ BSTR crx_path);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE *loadExtension )( 
            IChromeFrame * This,
            /* [in] */ BSTR extension_path);
        
        END_INTERFACE
    } IChromeFrameVtbl;

    interface IChromeFrame
    {
        CONST_VTBL struct IChromeFrameVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChromeFrame_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IChromeFrame_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IChromeFrame_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IChromeFrame_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IChromeFrame_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IChromeFrame_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IChromeFrame_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IChromeFrame_get_src(This,src)	\
    ( (This)->lpVtbl -> get_src(This,src) ) 

#define IChromeFrame_put_src(This,src)	\
    ( (This)->lpVtbl -> put_src(This,src) ) 

#define IChromeFrame_postMessage(This,message,target)	\
    ( (This)->lpVtbl -> postMessage(This,message,target) ) 

#define IChromeFrame_get_onload(This,onload_handler)	\
    ( (This)->lpVtbl -> get_onload(This,onload_handler) ) 

#define IChromeFrame_put_onload(This,onload_handler)	\
    ( (This)->lpVtbl -> put_onload(This,onload_handler) ) 

#define IChromeFrame_get_onloaderror(This,onerror_handler)	\
    ( (This)->lpVtbl -> get_onloaderror(This,onerror_handler) ) 

#define IChromeFrame_put_onloaderror(This,onerror_handler)	\
    ( (This)->lpVtbl -> put_onloaderror(This,onerror_handler) ) 

#define IChromeFrame_get_onmessage(This,onmessage_handler)	\
    ( (This)->lpVtbl -> get_onmessage(This,onmessage_handler) ) 

#define IChromeFrame_put_onmessage(This,onmessage_handler)	\
    ( (This)->lpVtbl -> put_onmessage(This,onmessage_handler) ) 

#define IChromeFrame_get_readyState(This,ready_state)	\
    ( (This)->lpVtbl -> get_readyState(This,ready_state) ) 

#define IChromeFrame_addEventListener(This,event_type,listener,use_capture)	\
    ( (This)->lpVtbl -> addEventListener(This,event_type,listener,use_capture) ) 

#define IChromeFrame_removeEventListener(This,event_type,listener,use_capture)	\
    ( (This)->lpVtbl -> removeEventListener(This,event_type,listener,use_capture) ) 

#define IChromeFrame_get_version(This,version)	\
    ( (This)->lpVtbl -> get_version(This,version) ) 

#define IChromeFrame_postPrivateMessage(This,message,origin,target)	\
    ( (This)->lpVtbl -> postPrivateMessage(This,message,origin,target) ) 

#define IChromeFrame_get_useChromeNetwork(This,pVal)	\
    ( (This)->lpVtbl -> get_useChromeNetwork(This,pVal) ) 

#define IChromeFrame_put_useChromeNetwork(This,newVal)	\
    ( (This)->lpVtbl -> put_useChromeNetwork(This,newVal) ) 

#define IChromeFrame_installExtension(This,crx_path)	\
    ( (This)->lpVtbl -> installExtension(This,crx_path) ) 

#define IChromeFrame_loadExtension(This,extension_path)	\
    ( (This)->lpVtbl -> loadExtension(This,extension_path) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IChromeFrame_INTERFACE_DEFINED__ */


#ifndef __IChromeFramePrivileged_INTERFACE_DEFINED__
#define __IChromeFramePrivileged_INTERFACE_DEFINED__

/* interface IChromeFramePrivileged */
/* [hidden][nonextensible][oleautomation][uuid][object] */ 


EXTERN_C const IID IID_IChromeFramePrivileged;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("655A11E0-EF63-4fbe-9DF6-C182D2FCD6DC")
    IChromeFramePrivileged : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetWantsPrivileged( 
            /* [out] */ boolean *wants_privileged) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetChromeExtraArguments( 
            /* [out] */ BSTR *args) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetChromeProfileName( 
            /* [out] */ BSTR *profile_name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExtensionApisToAutomate( 
            /* [out] */ BSTR *extension_apis) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IChromeFramePrivilegedVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IChromeFramePrivileged * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IChromeFramePrivileged * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IChromeFramePrivileged * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetWantsPrivileged )( 
            IChromeFramePrivileged * This,
            /* [out] */ boolean *wants_privileged);
        
        HRESULT ( STDMETHODCALLTYPE *GetChromeExtraArguments )( 
            IChromeFramePrivileged * This,
            /* [out] */ BSTR *args);
        
        HRESULT ( STDMETHODCALLTYPE *GetChromeProfileName )( 
            IChromeFramePrivileged * This,
            /* [out] */ BSTR *profile_name);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtensionApisToAutomate )( 
            IChromeFramePrivileged * This,
            /* [out] */ BSTR *extension_apis);
        
        END_INTERFACE
    } IChromeFramePrivilegedVtbl;

    interface IChromeFramePrivileged
    {
        CONST_VTBL struct IChromeFramePrivilegedVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IChromeFramePrivileged_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IChromeFramePrivileged_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IChromeFramePrivileged_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IChromeFramePrivileged_GetWantsPrivileged(This,wants_privileged)	\
    ( (This)->lpVtbl -> GetWantsPrivileged(This,wants_privileged) ) 

#define IChromeFramePrivileged_GetChromeExtraArguments(This,args)	\
    ( (This)->lpVtbl -> GetChromeExtraArguments(This,args) ) 

#define IChromeFramePrivileged_GetChromeProfileName(This,profile_name)	\
    ( (This)->lpVtbl -> GetChromeProfileName(This,profile_name) ) 

#define IChromeFramePrivileged_GetExtensionApisToAutomate(This,extension_apis)	\
    ( (This)->lpVtbl -> GetExtensionApisToAutomate(This,extension_apis) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IChromeFramePrivileged_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_chrome_tab_0000_0002 */
/* [local] */ 

#define SID_ChromeFramePrivileged __uuidof(IChromeFramePrivileged)
typedef /* [public] */ 
enum __MIDL___MIDL_itf_chrome_tab_0000_0002_0001
    {	CF_EVENT_DISPID_ONLOAD	= 1,
	CF_EVENT_DISPID_ONLOADERROR	= ( CF_EVENT_DISPID_ONLOAD + 1 ) ,
	CF_EVENT_DISPID_ONMESSAGE	= ( CF_EVENT_DISPID_ONLOADERROR + 1 ) ,
	CF_EVENT_DISPID_ONPRIVATEMESSAGE	= ( CF_EVENT_DISPID_ONMESSAGE + 1 ) ,
	CF_EVENT_DISPID_ONEXTENSIONREADY	= ( CF_EVENT_DISPID_ONPRIVATEMESSAGE + 1 ) ,
	CF_EVENT_DISPID_ONREADYSTATECHANGED	= -609
    } 	ChromeFrameEventDispId;



extern RPC_IF_HANDLE __MIDL_itf_chrome_tab_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_chrome_tab_0000_0002_v0_0_s_ifspec;


#ifndef __ChromeTabLib_LIBRARY_DEFINED__
#define __ChromeTabLib_LIBRARY_DEFINED__

/* library ChromeTabLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ChromeTabLib;

#ifndef __DIChromeFrameEvents_DISPINTERFACE_DEFINED__
#define __DIChromeFrameEvents_DISPINTERFACE_DEFINED__

/* dispinterface DIChromeFrameEvents */
/* [uuid] */ 


EXTERN_C const IID DIID_DIChromeFrameEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("388B5D64-CE67-415b-9B0F-745C56E868E7")
    DIChromeFrameEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DIChromeFrameEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DIChromeFrameEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DIChromeFrameEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DIChromeFrameEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DIChromeFrameEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DIChromeFrameEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DIChromeFrameEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DIChromeFrameEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DIChromeFrameEventsVtbl;

    interface DIChromeFrameEvents
    {
        CONST_VTBL struct DIChromeFrameEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DIChromeFrameEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define DIChromeFrameEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define DIChromeFrameEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define DIChromeFrameEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define DIChromeFrameEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define DIChromeFrameEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define DIChromeFrameEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DIChromeFrameEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_HtmlFilter;

#ifdef __cplusplus

class DECLSPEC_UUID("BB1176EE-20DD-41DC-9D1E-AC1335C7BBB0")
HtmlFilter;
#endif

EXTERN_C const CLSID CLSID_ChromeProtocol;

#ifdef __cplusplus

class DECLSPEC_UUID("9875BFAF-B04D-445E-8A69-BE36838CDE3E")
ChromeProtocol;
#endif

EXTERN_C const CLSID CLSID_ChromeActiveDocument;

#ifdef __cplusplus

class DECLSPEC_UUID("3E1D0E7F-F5E3-44CC-AA6A-C0A637619AB8")
ChromeActiveDocument;
#endif

EXTERN_C const CLSID CLSID_ChromeFrame;

#ifdef __cplusplus

class DECLSPEC_UUID("E0A900DF-9611-4446-86BD-4B1D47E7DB2A")
ChromeFrame;
#endif

EXTERN_C const CLSID CLSID_ChromeFrameBHO;

#ifdef __cplusplus

class DECLSPEC_UUID("ECB3C477-1A0A-44bd-BB57-78F9EFE34FA7")
ChromeFrameBHO;
#endif
#endif /* __ChromeTabLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


