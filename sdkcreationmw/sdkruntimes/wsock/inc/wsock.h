/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* which accompanies this distribution, and is available
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
*
*/


#ifndef _WSOCK_WSOCK_H_
#define _WSOCK_WSOCK_H_

#ifdef _DEBUG
#  include <e32svr.h>  // defines RDebug
#endif // _DEBUG

#include <cdbcols.h>

// files that redefine ASSERT macro
#include <e32def.h>
#include <vwsdef.h>
#include <mda/common/base.h>

#define N(x) ((TInt)(sizeof(x)/sizeof((x)[0])))

#ifndef _REALLY_DEBUG
#  if defined(_DEBUG) && defined(WSOCK_ENABLE_DEBUG_TRACE)
#    define _REALLY_DEBUG
#  endif // _DEBUG && WSOCK_ENABLE_DEBUG_TRACE
#endif // _REALLY_DEBUG

//===========================================================================
// Debug trace
//===========================================================================

#ifdef _UNICODE
#  define __T(a,b) a L##b
#else  // _UNICODE
#  define __T(a,b) a b
#endif // _UNICODE

#ifdef _REALLY_DEBUG

#  ifdef TRACE_PREFIX
#    define ___L(z) _L(z)
#    define __L(z) ___L(__T(TRACE_PREFIX,z))
#  else
#    define __L(z) _L(z)
#  endif // TRACE_PREFIX

#  define TRACE(s)                  RDebug::Print(__L("%s"),_S(s))
#  define TRACE0(s)                 RDebug::Print(__L("%s"),_S(s))
#  define TRACE1(s,p1)              RDebug::Print(__L(s),p1)
#  define TRACE2(s,p1,p2)           RDebug::Print(__L(s),p1,p2)
#  define TRACE3(s,p1,p2,p3)        RDebug::Print(__L(s),p1,p2,p3)
#  define TRACE4(s,p1,p2,p3,p4)     RDebug::Print(__L(s),p1,p2,p3,p4)
#  define TRACE5(s,p1,p2,p3,p4,p5)  RDebug::Print(__L(s),p1,p2,p3,p4,p5)

#  define DEBUG_ONLY(x)             x

#else  /* !_DEBUG */

#  define TRACE(s)                  ((void)0)
#  define TRACE0(s)                 ((void)0)
#  define TRACE1(s,p1)              ((void)0)
#  define TRACE2(s,p1,p2)           ((void)0)
#  define TRACE3(s,p1,p2,p3)        ((void)0)
#  define TRACE4(s,p1,p2,p3,p4)     ((void)0)
#  define TRACE5(s,p1,p2,p3,p4,p5)  ((void)0)

#  define DEBUG_ONLY(x)

#endif /* !_DEBUG */


#define WTRACE(s)               	TRACE1(__T("[%08X] ",s),this)
#define WTRACE0(s)              	TRACE1(__T("[%08X] ",s),this)
#define WTRACE1(s,p1)           	TRACE2(__T("[%08X] ",s),this,p1)
#define WTRACE2(s,p1,p2)        	TRACE3(__T("[%08X] ",s),this,p1,p2)
#define WTRACE3(s,p1,p2,p3)     	TRACE4(__T("[%08X] ",s),this,p1,p2,p3)
#define WTRACE4(s,p1,p2,p3,p4)  	TRACE5(__T("[%08X] ",s),this,p1,p2,p3,p4)

//===========================================================================
// Debug assert
//===========================================================================

#undef ASSERT
#ifdef _REALLY_DEBUG
void WsockAssert(const TText* s,const TText* file,TInt line);
TInt WsockAssertSuccess(TInt err,const TText* s,const TText* file,TInt line);
void WsockLeave(TInt err,const TText* s,const TText* file,TInt line);
void WsockLeaveIfError(TInt err,const TText* s,const TText* file,TInt line);
#  define __S(x) _S(x)
#  define ASSERT(x) ((void)((x)||(WsockAssert(_S(#x),__S(__FILE__),__LINE__),0),0))
#  define LEAVE(x) WsockLeave(x,_S(#x),__S(__FILE__),__LINE__)
#  define LEAVE_IF_ERROR(x) WsockLeaveIfError(x,_S(#x),__S(__FILE__),__LINE__)
#  define ASSERT_SUCCESS(x) WsockAssertSuccess(x,_S(#x),__S(__FILE__),__LINE__)
#  define VERIFY_SUCCESS(x) ASSERT_SUCCESS(x)
#else  // _DEBUG
#  define ASSERT(x)         ((void)0)
#  define LEAVE(x)          User::Leave(x)
#  define LEAVE_IF_ERROR(x) User::LeaveIfError(x)
#  define ASSERT_SUCCESS(x) ((void)0)
#  define VERIFY_SUCCESS(x) (x)
#endif // _DEBUG

//===========================================================================
// Constants
//===========================================================================

#define WSOCK_LAN_SERVICE 1

// Names of the modules
_LIT(KWinsockAgent,"wsocka");
_LIT(KWinsockInterface,"wsocki");
_LIT(KWinsockProtocol,"wsock");

// CommDb constants
_LIT(KWinsockConnectionName,"Winsock");
_LIT(KWinsockAPName,"Winsock");
_LIT(KWinsockBearerName,"Winsock");
_LIT(KWinsockBearerType,"LANBearer");
_LIT(KWinsockWapBearer,"WAPIPBearer");
_LIT(KWinsockGatewayAddress,"0.0.0.0");

#ifdef WSOCK_LAN_SERVICE
_LIT(KWinsockServiceType,"LANService");
_LIT(KWinsockServiceName,"Winsock");
#else // ! WSOCK_LAN_SERVICE
_LIT(KWinsockServiceType,"OutgoingGPRS");
_LIT(KWinsockServiceName,"Winsock Service");
#endif // !WSOCK_LAN_SERVICE

const TUint32 KWinsockSocketActivityTimeout = (TUint32)-1;
const TUint32 KWinsockSessionClosedTimeout = (TUint32)-1;
const TUint32 KWinsockSocketClosedTimeout = (TUint32)-1;
const TUint32 KWinsockAPType = EIspTypeInternetAndWAP;
#define KWinsockPdpType RPacketContext::EPdpTypeIPv6 // defined in etelpckt.h

//===========================================================================
// Internal API
//===========================================================================
void WsockCheckCommDbL(TUint32& aIapId,
                       TUint32& aNetworkId,
                       TUint32& aServiceId,
                       TUint32& aBearerId);

//===========================================================================
// Functions exported from wsock.dll
//===========================================================================

class CProtocolFamilyBase;
class CNifAgentFactory;
class CNifIfFactory;
class CNetworkControllerBase;

extern "C" {
    IMPORT_C CProtocolFamilyBase* WsockCreateProtocolFamily(void);
    IMPORT_C CNifAgentFactory* WsockCreateAgentFactory(void);
    IMPORT_C CNifIfFactory* WsockCreateInterfaceFactory(void);
    IMPORT_C TInt WsockQueryConfiguration(TBool&, HBufC*&, TUint&);
    IMPORT_C TInt WsockSetConfiguration(TBool, TBool, const TDesC*, TUint);
    IMPORT_C CProtocolFamilyBase* WsockCreateProtocolFamily6(void);
}

#endif // _WSOCK_WSOCK_H_
