/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  HTTP traffic plugin for diagnostics
*
*/


#ifndef __ECMT_HTTP_PLUGIN_H__
#define __ECMT_HTTP_PLUGIN_H__ 

#include <http/mhttpfilter.h>
#include <http/rhttpsession.h>
#include <http/cecomfilter.h>
#include <http/rhttptransaction.h>
#include <http/rhttpheaders.h>
#include <http/mhttpdatasupplier.h>

#include "EcmtClient.h"


#define TRACE_PREFIX   "HTTPPLUGIN:"

#ifdef _DEBUG
#  include <e32svr.h>  // defines RDebug
#endif // _DEBUG

#ifdef _DEBUG

#  ifdef TRACE_PREFIX
#    if defined(_UNICODE)
#      define ___L(a,b) _L(a L##b)
#    else  // _UNICODE
#      define ___L(a,b) _L(a##b)
#    endif // _UNICODE
#    define __L(z) ___L(TRACE_PREFIX,z)
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

#  define DEBUG_ONLY(x)            x

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

class RHTTPResponse;
class RHTTPTransaction;

class CHttpPluginFilter : public CEComFilter, public MHTTPFilter
{
public:

    static CEComFilter* InstallFilterL(TAny* aSession);	
    virtual ~CHttpPluginFilter();

    // Methods from MHTTPFilterBase
    virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
    virtual void MHFSessionRunL(const THTTPSessionEvent& aEvent);
    virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction,
                             const THTTPEvent& aEvent);
    virtual TInt MHFSessionRunError(TInt aError, const THTTPSessionEvent& aEvent);

    // Methods from MHTTPFilter
    virtual void MHFUnload(RHTTPSession aSession, THTTPFilterHandle aHandle);
    virtual void MHFLoad(RHTTPSession aSession, THTTPFilterHandle aHandle);

private:
	
    CHttpPluginFilter(RHTTPSession aSession);

    ///ContructL also Installs the filter.
    void ConstructL(RHTTPSession aSession);
    void ProcessResponseL (RHTTPTransaction& aTransaction, TInt aEvent);
    void ProcessRequestL (RHTTPTransaction& aTransaction, TInt aEvent);
    void ReportHTTPEventL (RHTTPTransaction& aTransaction, TInt aEvent);

private:
    /// A count to make sure we delete at the right time.
    TInt iLoadCount;
    RStringPool iStringPool;
    RHTTPSession iSession;

    // Session to the Ecmt server
    REcmt iEcmtClient;
};


//========================================================================
/* CHttpPluginData represent HTTP data as a set of records, 
 * where the each record has 3 fileds:
 * <Type><Length><Data>, 
 * <Type> - 2 bytes,
 * <Length> - 4 bytes,
 * <Data> - <Length> bytes
 */

class CHttpPluginData : public CBase
{
public:
    static CHttpPluginData* NewL (TInt aType, TInt aId);  
    static CHttpPluginData* NewL (TInt aType, TInt aId,
                                  RHTTPRequest& aRequest,
                                  RStringPool strPool);
    static CHttpPluginData* NewL (TInt aType, TInt aId,
                                  RHTTPResponse& aResponse,
                                  RStringPool strPool);
    virtual ~CHttpPluginData ();
    HBufC8* GetData ();

private:
    CHttpPluginData ();
    CHttpPluginData (RStringPool  strPool);
    void ConstructL (TInt aType, TInt aId);
    void ConstructL (TInt aType, TInt aId, RHTTPRequest& aRequest); 
    void ConstructL (TInt aType, TInt aId, RHTTPResponse& aResponse);
    
    void AllocDataHeaderL (TInt aType, TUint32 aId);
    void AllocDataHeaderL (TInt aType, TUint32 aId, TUint aDataSize);
    void CollectHeadersL (RHTTPHeaders& aHeaders, HBufC8*& aDataPtr);
    void SetStatusL (RHTTPResponse& aResponse); 
    void SetMethodL (RHTTPRequest& aRequest);
    void SetHeadersL (RHTTPMessage& aHttpMsg);
    void SetURIL (RHTTPRequest& aRequest);
    void SetBodyL (RHTTPMessage& aHttpMsg);
    void SetDataL (const TDesC8& aNewData, TUint16 aDataType);
    void SetDataL (const TUint32 aNewData, TUint16 aDataType);
    void SetDataL (const TUint16 aNewData, TUint16 aDataType);
    void SetDataL (const TUint8 aNewData, TUint16 aDataType);

private:
    HBufC8*  iDataPtr;
    RStringPool  iStrPool;
};

#endif /* __ECMT_HTTP_PLUGIN_H__ */
