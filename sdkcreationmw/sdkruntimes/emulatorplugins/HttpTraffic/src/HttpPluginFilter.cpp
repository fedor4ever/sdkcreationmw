/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ecom/implementationproxy.h>

#include "HTTPPluginFilter.h"
#include "HttpPluginUids.h"

//------------------------------------------------------------------------
_LIT8( KHttpFilterName, "EcmtHttpTraffic");
const TInt KFilterPosition = MHTTPFilter::ECache-10;  

//------------------------------------------------------------------------

_LIT8(KHEADER_FORMAT, "%S: %S\n");

inline void BIG_ENDIAN_PACK_INT(TUint8* dst, const TUint32 val)
{
   dst[0] = (val >> 24) & 0xFF;
   dst[1] = (val >> 16) & 0xFF;
   dst[2] = (val >> 8) & 0xFF;
   dst[3] = (val & 0xFF);
}

inline void BIG_ENDIAN_PACK_SHORT(TUint8* dst, const TUint16 val)
{
   dst[0] = (val >> 8) & 0xFF;
   dst[1] = (val & 0xFF);
}

#define TYPE_SIZE          1
#define TRANS_ID_SIZE      4
#define PROC_ID_SIZE       4
#define HTTP_STATUS_SIZE   4

#define DATA_LEN_SIZE      4
#define DATA_TYPE_SIZE     2

#define DATA_HEADER_SIZE  DATA_TYPE_SIZE + DATA_LEN_SIZE + TYPE_SIZE + \
                          DATA_TYPE_SIZE + DATA_LEN_SIZE + TRANS_ID_SIZE + \
                          DATA_TYPE_SIZE + DATA_LEN_SIZE + PROC_ID_SIZE + \
                          DATA_TYPE_SIZE + DATA_LEN_SIZE + HTTP_STATUS_SIZE

#define EVENT_DATA_TYPE    1
#define TRANS_ID_DATA_TYPE 2
#define PROC_ID_DATA_TYPE  3
#define STATUS_DATA_TYPE   4
#define METHOD_DATA_TYPE   5
#define URL_DATA_TYPE      6
#define HEADERS_DATA_TYPE  7
#define BODY_DATA_TYPE     8


/* ======================================================================= */
  CHttpPluginFilter::CHttpPluginFilter(RHTTPSession aSession)
    : iSession(aSession)
/* ======================================================================= */
  {
  } 

/* ======================================================================= */
  CEComFilter* CHttpPluginFilter::InstallFilterL (TAny* aSession)
/* ======================================================================= */
  {
    RHTTPSession*      session = REINTERPRET_CAST (RHTTPSession*, aSession);
    CHttpPluginFilter* filter = new (ELeave) CHttpPluginFilter (*session);
    
    CleanupStack::PushL (filter);
    filter->ConstructL (*session);
    CleanupStack::Pop (filter);
    
    return filter;
  }

/* ======================================================================= */
  void CHttpPluginFilter::ConstructL(RHTTPSession aSession) 
/* ======================================================================= */
  {
    // Connect to Ecmt server first
    User::LeaveIfError(iEcmtClient.Connect());

    iStringPool = aSession.StringPool ();
    RStringF filterName = iStringPool.OpenFStringL (KHttpFilterName);
  
    CleanupClosePushL (filterName);

    iSession.FilterCollection ().AddFilterL (*this, THTTPEvent::ESubmit,
                                             KFilterPosition, filterName );

    iSession.FilterCollection().AddFilterL (*this, 
                                            THTTPEvent::EGotResponseHeaders,
                                            KFilterPosition, filterName);                
    iSession.FilterCollection ().AddFilterL (*this, 
                                             THTTPEvent::EGotResponseBodyData,
                                             KFilterPosition,  filterName);
  
    iSession.FilterCollection ().AddFilterL (*this, 
                                             THTTPEvent::EResponseComplete,
                                             KFilterPosition, filterName);

    iSession.FilterCollection ().AddFilterL (*this, THTTPEvent::EClosed,
                                             KFilterPosition, filterName);

    iSession.FilterCollection ().AddFilterL (*this, THTTPEvent::EGetCipherSuite,
                                             KFilterPosition, filterName);
  
    CleanupStack::PopAndDestroy ();
    TRACE1("[%08x] created", this);
  }

/* ======================================================================= */
  CHttpPluginFilter::~CHttpPluginFilter ()
/* ======================================================================= */
  {
    iEcmtClient.Close();
    TRACE1("[%08x] destroyed", this);
  } 

/* ======================================================================= */
  void CHttpPluginFilter::MHFLoad (RHTTPSession, THTTPFilterHandle)
/* ======================================================================= */
  {
    User::LockedInc(iLoadCount);
  }

/* ======================================================================= */
  void CHttpPluginFilter::MHFUnload (RHTTPSession , THTTPFilterHandle)
/* ======================================================================= */
  {
    // User::LockedDec returns the value before it is decremented.
    if (User::LockedDec(iLoadCount) == 1)
    {
      delete this;
    }
  }

/* ======================================================================= */
  void CHttpPluginFilter::MHFRunL (RHTTPTransaction aTransaction, 
                                   const THTTPEvent& aEvent)
/* ======================================================================= */
  {
    if (aEvent.iUID != KHTTPUid) return;

    switch (aEvent.iStatus)
    {
      case THTTPEvent::ESubmit:
        TRACE1 ("MHFRunL: ESubmit (%d)", aEvent.iStatus);
        ProcessRequestL (aTransaction, aEvent.iStatus);
        break;
      case THTTPEvent::EGotResponseHeaders:
        TRACE1 ("MHFRunL: EGotResponseHeaders (%d)", aEvent.iStatus);
        ProcessResponseL (aTransaction, aEvent.iStatus);
        break;
      case THTTPEvent::EGotResponseBodyData:
        TRACE1 ("MHFRunL: EGotResponseBodyData (%d)", aEvent.iStatus);
        ProcessResponseL (aTransaction, aEvent.iStatus);
        break;
      case THTTPEvent::EResponseComplete:
        TRACE1 ("MHFRunL: EResponseComplete (%d)", aEvent.iStatus);
        ReportHTTPEventL (aTransaction, aEvent.iStatus);
        break;
      case THTTPEvent::EClosed:
        TRACE1 ("MHFRunL: EClosed (%d)", aEvent.iStatus);
        ReportHTTPEventL (aTransaction, aEvent.iStatus);
        break;
      case THTTPEvent::EGetCipherSuite:
        TRACE1 ("MHFRunL: EGetCipherSuite (%d)", aEvent.iStatus);
        break;
      default: 
        break;
  }
}

/* ======================================================================= */
  TInt CHttpPluginFilter::MHFRunError (TInt DEBUG_ONLY(aError), 
                                       RHTTPTransaction aTransaction,
                                       const THTTPEvent& )
/* ======================================================================= */
  {
    TRACE1 ("MHFRunError: error %d", aError);
    // error happened, cleanup the datasupplier related with this transaction
    aTransaction.Fail ();
    return KErrNone;
  }

/* ======================================================================= */
  void CHttpPluginFilter::MHFSessionRunL (const THTTPSessionEvent& )
/* ======================================================================= */
  {
  }

/* ======================================================================= */
  TInt CHttpPluginFilter::MHFSessionRunError (TInt aError, 
                                              const THTTPSessionEvent& )
/* ======================================================================= */ 
  {
    return aError;
  }

/* ======================================================================= */
void CHttpPluginFilter::ProcessResponseL (RHTTPTransaction& aTransaction, 
                                          TInt  aEvent)
/* ======================================================================= */
{
  RHTTPResponse     response = aTransaction.Response ();     
  CHttpPluginData* httpData =  NULL;
  
  TRAPD (error, httpData = CHttpPluginData::NewL (aEvent, aTransaction.Id(), 
                                            response, iSession.StringPool ()));
  
  if (error == KErrNone)
  {
    TDesC8*  dataDes = httpData->GetData ();
    iEcmtClient.Send (KJAVA_HTTPPLUGIN_UID, *dataDes);
  }

  delete httpData; 
}

/* ======================================================================= */
void CHttpPluginFilter::ProcessRequestL (RHTTPTransaction& aTransaction, 
                                         TInt aEvent)
/* ======================================================================= */
{
  RHTTPRequest     request = aTransaction.Request ();     
  CHttpPluginData* httpData = NULL;
  
  TRAPD (error, httpData = CHttpPluginData::NewL (aEvent, aTransaction.Id(),
                                           request, iSession.StringPool ()));
  if (error == KErrNone)
  {
    TDesC8*  dataDes = httpData->GetData ();
    iEcmtClient.Send (KJAVA_HTTPPLUGIN_UID, *dataDes);
  }
  
  delete httpData; 
}

/* ======================================================================= */
void CHttpPluginFilter::ReportHTTPEventL (RHTTPTransaction& aTransaction, 
                                          TInt aEvent)
/* ======================================================================= */
{
  CHttpPluginData* httpData = NULL;
  TRAPD(error, httpData = CHttpPluginData::NewL (aEvent, aTransaction.Id()));
  if (error == KErrNone)
  {
    TDesC8* dataDes = httpData->GetData ();
    iEcmtClient.Send (KJAVA_HTTPPLUGIN_UID, *dataDes);
  }  
  delete httpData; 
}

//------------------------------------------------------------------------

/* CHttpPluginData represent HTTP data as a set of records, 
 * where the each record has 3 fileds:
 * <Type><Length><Data>, 
 * <Type> - 2 bytes,
 * <Length> - 4 bytes,
 * <Data> - <Length> bytes
 */

/* ======================================================================= */
  CHttpPluginData* CHttpPluginData::NewL (TInt aType, TInt aId) 
/* ======================================================================= */
  {
    CHttpPluginData* self = new (ELeave) CHttpPluginData ();
    CleanupStack::PushL (self);
    self->ConstructL (aType, aId);
    CleanupStack::Pop ();
    return self;
  }

/* ======================================================================= */
  CHttpPluginData* CHttpPluginData::NewL (TInt aType, TInt aId, 
                                          RHTTPRequest& aRequest,
                                          RStringPool  strPool)      
/* ======================================================================= */
  {
    CHttpPluginData* self = new (ELeave) CHttpPluginData (strPool);
    CleanupStack::PushL (self);
    self->ConstructL (aType, aId, aRequest);
    CleanupStack::Pop ();
    return self;
  }

/* ======================================================================= */
  CHttpPluginData* CHttpPluginData::NewL (TInt aType, TInt aId, 
                                          RHTTPResponse& aResponse,
                                          RStringPool  strPool)      
/* ======================================================================= */
  {
    CHttpPluginData* self = new (ELeave) CHttpPluginData (strPool);
    CleanupStack::PushL (self);
    self->ConstructL (aType, aId, aResponse);
    CleanupStack::Pop ();
    return self;
  }

/* ======================================================================= */
  CHttpPluginData::CHttpPluginData ()
/* ======================================================================= */
  {
  }

/* ======================================================================= */
  CHttpPluginData::CHttpPluginData (RStringPool strPool) : iStrPool (strPool)
/* ======================================================================= */
  {
  }

/* ======================================================================= */
  void CHttpPluginData::ConstructL (TInt aType, TInt aId)
/* ======================================================================= */
  {
    AllocDataHeaderL (aType, aId);
  }


/* ======================================================================= */
  void CHttpPluginData::ConstructL (TInt aType, TInt aId, RHTTPRequest& aRequest)
/* ======================================================================= */
  {
    AllocDataHeaderL (aType, aId);
    SetHeadersL (aRequest);
    SetMethodL (aRequest);
    SetURIL (aRequest);
 //   SetBodyL (aRequest);
  }

/* ======================================================================= */
  void CHttpPluginData::ConstructL (TInt aType, TInt aId, RHTTPResponse& aResponse)
/* ======================================================================= */
  {
    AllocDataHeaderL (aType, aId);
    SetStatusL (aResponse);
    if (aType == THTTPEvent::EGotResponseHeaders)
    {
      SetHeadersL (aResponse);
    }
    else if (aType == THTTPEvent::EGotResponseBodyData)
    {
      SetBodyL (aResponse);
    }
  }

/* ======================================================================= */
  void CHttpPluginData::AllocDataHeaderL (TInt aType, TUint32 aId) 
/* ======================================================================= */
  {
    AllocDataHeaderL (aType, aId, 0);
  }

/* ======================================================================= */
  void CHttpPluginData::AllocDataHeaderL (TInt aType, TUint32 aId, 
                                         TUint aDataSize) 
/* ======================================================================= */
  {
    iDataPtr = HBufC8::NewL (DATA_HEADER_SIZE + aDataSize);
    
    TPtr8    ptr = iDataPtr->Des ();       
    TUint32  procID = (TUint) RProcess().Id ();

    ptr.FillZ (DATA_HEADER_SIZE);
    ptr.SetLength (0);    

    SetDataL ((TUint8)aType, EVENT_DATA_TYPE);
    SetDataL (aId, TRANS_ID_DATA_TYPE);
    SetDataL (procID, PROC_ID_DATA_TYPE);
  }

/* ======================================================================= */
  CHttpPluginData::~CHttpPluginData ()
/* ======================================================================= */
  {
    delete iDataPtr;
  }

/* ======================================================================= */
void CHttpPluginData::CollectHeadersL (RHTTPHeaders& aHeaders, HBufC8*& aData)
/* ======================================================================= */
  {
    THTTPHdrFieldIter it = aHeaders.Fields();
    while (!it.AtEnd())
    {
      RStringTokenF fieldName = it();
      RStringF fieldNameStr = iStrPool.StringF (fieldName);
      TPtrC8 headerName(fieldNameStr.DesC());
      TPtrC8 headerValue;
      if (aHeaders.GetRawField(fieldNameStr, headerValue) == KErrNone)
      {
        /* Leave room for the separator and end of line character */;
        TInt headerStrSize = headerName.Length() + headerValue.Length() + 3;
        if (aData == NULL)
        {
          aData = HBufC8::NewL(headerStrSize);
        }
        else if (aData->Length() + headerStrSize >= aData->Des().MaxLength())
        {
          aData = aData->ReAllocL(aData->Length() + headerStrSize);
        }
        aData->Des().AppendFormat(KHEADER_FORMAT, &headerName, &headerValue);
      }
      ++it;
    }
  }

/* ======================================================================= */
  void CHttpPluginData::SetStatusL (RHTTPResponse& aResponse)
/* ======================================================================= */
  {
    TUint32   status = aResponse.StatusCode ();

    SetDataL (status, STATUS_DATA_TYPE);
  }

/* ======================================================================= */
  void CHttpPluginData::SetMethodL (RHTTPRequest& aRequest)
/* ======================================================================= */
  {
    const TDesC8&     method = aRequest.Method ().DesC ();
    SetDataL (method, METHOD_DATA_TYPE);
  }

/* ======================================================================= */
  void CHttpPluginData::SetHeadersL (RHTTPMessage& aHttpMsg)
/* ======================================================================= */
  {
    RHTTPHeaders         headers = aHttpMsg.GetHeaderCollection ();
    HBufC8*              hdrsPtr = NULL;
    
    CollectHeadersL (headers, hdrsPtr);
    if (hdrsPtr)
    {
      SetDataL (hdrsPtr->Des (), HEADERS_DATA_TYPE);
      delete hdrsPtr;
    }
  }

/* ======================================================================= */
  void CHttpPluginData::SetURIL (RHTTPRequest& aRequest)
/* ======================================================================= */
  {
    const TDesC8&  uri = aRequest.URI ().UriDes ();
    SetDataL (uri, URL_DATA_TYPE);    
  }

/* ======================================================================= */
  void CHttpPluginData::SetBodyL (RHTTPMessage& aHttpMsg)
/* ======================================================================= */
  {
    MHTTPDataSupplier*   body = aHttpMsg.Body (); 
    TPtrC8               bodyPart;

    if (body != NULL)
    {
      body->GetNextDataPart (bodyPart);
      SetDataL (bodyPart, BODY_DATA_TYPE);
    }
  }

/* ======================================================================= */
  void CHttpPluginData::SetDataL (const TDesC8& aNewData, TUint16 aDataType)
/* ======================================================================= */
  {
    TPtr8  ptr = iDataPtr->Des ();
    TInt   addSize = DATA_TYPE_SIZE + DATA_LEN_SIZE + aNewData.Length ();

    if (ptr.MaxLength () < ptr.Length () + addSize)
    {
      iDataPtr = iDataPtr->ReAllocL (ptr.Length () + addSize);
      ptr.Set (iDataPtr->Des ());
    }

    ptr.SetLength (ptr.Length () + DATA_TYPE_SIZE + DATA_LEN_SIZE);
    BIG_ENDIAN_PACK_SHORT(&ptr[ptr.Length () - DATA_TYPE_SIZE - DATA_LEN_SIZE], 
                          aDataType);
    BIG_ENDIAN_PACK_INT(&ptr[ptr.Length() - DATA_LEN_SIZE], aNewData.Length ());
    
    ptr.Append (aNewData);
  }

/* ======================================================================= */
  void CHttpPluginData::SetDataL (const TUint32 aNewData, TUint16 aDataType)
/* ======================================================================= */
  {
    TPtr8  ptr = iDataPtr->Des ();
    TInt   addSize = DATA_TYPE_SIZE + DATA_LEN_SIZE + sizeof (TUint32);

    if (ptr.MaxLength () < ptr.Length () + addSize)
    {
      iDataPtr = iDataPtr->ReAllocL (ptr.Length () + addSize);
      ptr.Set (iDataPtr->Des ());
    }

    ptr.SetLength (ptr.Length () + addSize);
    BIG_ENDIAN_PACK_SHORT(&ptr[ptr.Length () - addSize], aDataType);
    BIG_ENDIAN_PACK_INT(&ptr[ptr.Length () - DATA_LEN_SIZE - sizeof (TUint32)],
                        sizeof (TUint32));
    BIG_ENDIAN_PACK_INT(&ptr[ptr.Length () - sizeof (TUint32)], aNewData);
  }

/* ======================================================================= */
  void CHttpPluginData::SetDataL (const TUint16 aNewData, TUint16 aDataType)
/* ======================================================================= */
  {
    TPtr8  ptr = iDataPtr->Des ();
    TInt   addSize = DATA_TYPE_SIZE + DATA_LEN_SIZE + sizeof (TUint16);

    
    if (ptr.MaxLength () < ptr.Length () + addSize)
    {
      iDataPtr = iDataPtr->ReAllocL (ptr.Length () + addSize); 
      ptr.Set (iDataPtr->Des ());
    }

    ptr.SetLength (ptr.Length () + addSize);
    BIG_ENDIAN_PACK_SHORT(&ptr[ptr.Length () - addSize], aDataType);
    BIG_ENDIAN_PACK_INT(&ptr[ptr.Length () - DATA_LEN_SIZE - sizeof (TUint16)],
                        sizeof (TUint16));
    BIG_ENDIAN_PACK_SHORT(&ptr[ptr.Length () - sizeof (TUint16)], aNewData);
  }

/* ======================================================================= */
  void CHttpPluginData::SetDataL (const TUint8 aNewData, TUint16 aDataType)
/* ======================================================================= */
  {
    TPtr8  ptr = iDataPtr->Des ();
    TInt   addSize = DATA_TYPE_SIZE + DATA_LEN_SIZE + sizeof (TUint8);

    if (ptr.MaxLength () < ptr.Length() + addSize)
    {
      iDataPtr = iDataPtr->ReAllocL (ptr.Length () + addSize);
      ptr.Set (iDataPtr->Des ());
    }

    ptr.SetLength (ptr.Length () + addSize);
    BIG_ENDIAN_PACK_SHORT(&ptr[ptr.Length() - addSize], aDataType);
    BIG_ENDIAN_PACK_INT(&ptr[ptr.Length () - DATA_LEN_SIZE - sizeof (TUint8)],
                        sizeof (TUint8));
    ptr[ptr.Length () - sizeof (TUint8)] = aNewData;
  }

/* ======================================================================= */
  HBufC8* CHttpPluginData::GetData ()
/* ======================================================================= */
  {
    return iDataPtr;
  }

/* ======================================================================= */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
/* ======================================================================= */
{
  static const TImplementationProxy KImplementationTable[] =
    {
      IMPLEMENTATION_PROXY_ENTRY( KHTTPPLUGINFILTER_IMPL_UID,
                                  CHttpPluginFilter::InstallFilterL )
    };

  aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
  return KImplementationTable;
}
