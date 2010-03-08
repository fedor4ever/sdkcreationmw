/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECMT plugin for pushing URL to the browser in the phone
*
*/


#include "PushURL.h"
#include "PushURLUids.h"
#include "EcmtMessageEvent.h"
#include <apgtask.h>
#include <apgcli.h>
#include <w32std.h>
#include <utf.h>
#include <ecom.h>
#include <DocumentHandler.h>
#include <AiwGenericParam.h>
#include <ImplementationProxy.h>
#include <aknglobalnote.h>

#define PARAM_TYPE_SIZE     1
#define PARAM_LENGTH_SIZE   4
#define URL_PARAM           0  // ASCII string
#define BROWSER_PARAM       1  // Browser UID, 0 to use default

// a few local utility functions and definitions
namespace {

_LIT(KFileScheme, "file://");

typedef struct _ExtMimeMap {
    const TText* ext;
    const TText8* contentType;
} ExtMimeMap;

static const ExtMimeMap BrowserTypes[] = {
    _S("html"),  _S8("text/html"),
    _S("htm"),   _S8("text/html"),
    _S("xhtml"), _S8("text/xhtml"),
    _S("xml"),   _S8("text/xml"),
    _S("wml"),   _S8("text/vnd.wap.wml"),
    _S("wmlc"),  _S8("application/vnd.wap.wmlc"),
    _S("wmlsc"), _S8("application/vnd.wap.wmlscriptc"),
    _S("wmls"),  _S8("text/vnd.wap.wmlscript"),
    _S("dtd"),   _S8("text/vnd.symbian.wml.dtd"),
    _S("wbxml"), _S8("application/vnd.wap.wbxml")
};

// MACROS
#define COUNT(a) (sizeof(a)/sizeof((a)[0]))

TPtrC FileExt(const TDesC& aFileName)
{
    TInt dotPos = aFileName.LocateReverse('.');
    TInt extLen = (dotPos >= 0) ? (aFileName.Length() - dotPos - 1) : 0;
    return aFileName.Right(extLen);
}

TBool IsBrowserFile(const TDesC& aFileName)
{
    TPtrC fileExt = FileExt(aFileName);
    for (TInt i=0; i<COUNT(BrowserTypes); ++i)
    {
        if (fileExt.CompareF(TPtrC(BrowserTypes[i].ext)) == 0)
        {
            return ETrue;
        }
    }
    return EFalse;
}

TBool IsFilePath(const TDesC& aFileName)
{
    TParse file;
    if (KErrNone == file.Set(aFileName, NULL, NULL))
    {
        return file.DrivePresent();
    }
    return EFalse;
}

} // anonymous namespace

/* ======================================================================= */
  CPushURL::CPushURL ()
    : iUid (TUid::Uid (KPUSH_URL_PLUGIN_IMPL_UID))
    , iDocHandler(NULL)
/* ======================================================================= */
  {
  }

/* ======================================================================= */
  MEcmtPlugin* CPushURL::NewL ()
/* ======================================================================= */
  {
    CPushURL*  self = new (ELeave) CPushURL;
    return self;
  }

/* ======================================================================= */
  CPushURL::~CPushURL ()
/* ======================================================================= */
  {
    if (iDocHandler)
    {
      delete iDocHandler;
    }
  }

/* ======================================================================= */
  void CPushURL::BindMessagingL (MEcmtMessaging* aMessaging)
/* ======================================================================= */
  {
    iMessaging = aMessaging;
    iMessaging->AddMessageEventObserverL (*this, iUid);
  }

/* ======================================================================= */
  void CPushURL::HandleNotifyL (const CEcmtMessageEvent& aEvent)
/* ======================================================================= */
  {
    const   TPtrC8 msg = iMessaging->Message (aEvent);
    HBufC   *url = NULL;
    TInt    offset = 0;
    TInt    param_type = 0;
    TUint32 param_data_len = 0;
    TInt    browser_uid = KWmlcHandler;
    TInt    msg_len  = msg.Length ();

    while (offset <= (msg_len - PARAM_TYPE_SIZE - PARAM_LENGTH_SIZE)) {

      // Parse the chunk header
      param_type = msg[offset];
      offset += PARAM_TYPE_SIZE;
      param_data_len = (msg[offset] << 24) |
                       (msg[offset + 1] << 16) |
                       (msg[offset + 2] << 8) |
                       (msg[offset + 3]);
      offset += PARAM_LENGTH_SIZE;

      // Make sure the chunk length is within bounds
      if (offset + param_data_len <= msg_len) {
        switch (param_type) {
        case BROWSER_PARAM:
          if (param_data_len == 4) {
            TUint uid = (msg[offset] << 24) |
                        (msg[offset + 1] << 16) |
                        (msg[offset + 2] << 8) |
                        (msg[offset + 3]);
            if (uid) {
              browser_uid = uid;
            }
          }
          break;
        case URL_PARAM:
          if (param_data_len > 0) {
            TPtrC8 data(msg.Mid(offset, param_data_len));
            delete url; // In case if we have already been here

            // Determine the length of the Unicode string
            TPtr nothing(NULL,0);
            TInt len = CnvUtfConverter::ConvertToUnicodeFromUtf8(nothing,data);

            // Convert to Unicode
            url = HBufC::New(len);
            if (url) {
              TPtr des(url->Des());
              CnvUtfConverter::ConvertToUnicodeFromUtf8(des, data);
            }
          }
          break;
        }
      }
      offset += param_data_len;
    }

    if (url) {
      // if a non-browser file is given with 'file://'-prefix or the url is a 
      // file-path, use document handler to launch an app; otherwise pass 
      // the url to the browsewr
      //
      TBool fileSchemeFound = (url->FindF(KFileScheme) == 0);
      
      if ((fileSchemeFound || IsFilePath(*url)) && (! IsBrowserFile(*url))) {

        TRAPD( err, LoadFileToDocHandlerL(*url, fileSchemeFound) );
        if (err != KErrNone) {
          CAknGlobalNote *note = CAknGlobalNote::NewLC();
         _LIT(KFormat, "ECMT: Loading %S failed: %d");

          // should be enough room for format-string, url and error-code
          HBufC* buf = HBufC::NewLC(url->Length () + 40);
          TPtr ptr = buf->Des();
          ptr.Format(KFormat, url, err);
          note->ShowNoteL(EAknGlobalInformationNote, *buf);
          CleanupStack::PopAndDestroy(2); // note, buf
        }
      }
      else {
        LoadUrlL (*url, TUid::Uid(browser_uid));
      }
      delete url;
    }
  }

/* ======================================================================= */
  void CPushURL::LoadUrlL (const TDesC&  aUrl, const TUid&  browser_uid)
/* ======================================================================= */
  {
    RWsSession aWs;
    User::LeaveIfError (aWs.Connect ());

    HBufC* param = HBufC::NewLC (aUrl.Length () + 2);
    TPtr ptr = param->Des ();
    ptr.Format(_L("4 %S"),&aUrl);

    // Find the browser among running applications
    TApaTaskList taskList (aWs);
    TApaTask task = taskList.FindApp (browser_uid);
    if (task.Exists ())
    {

      // Handle issue with image viewer
      // If image is being viewed need to force switch back to browser...
      task.SwitchOpenFile (aUrl);

      HBufC8* param8 = HBufC8::NewLC (param->Length ());
      param8->Des ().Append (*param);
      task.SendMessage (TUid::Uid (0), *param8); // Uid is not used
      CleanupStack::PopAndDestroy(param8);
    }
    else
    {
      // connect to AppArc server
      RApaLsSession appArcSession;
      User::LeaveIfError (appArcSession.Connect ());
      TThreadId tid;

      appArcSession.StartDocument (*param, browser_uid, tid);
      appArcSession.Close();
    }
    CleanupStack::PopAndDestroy(param);
  }

/* ======================================================================= */
  void CPushURL::LoadFileToDocHandlerL (const TDesC&  aUrl, TBool aPrefix)
/* ======================================================================= */
  {
    if (iDocHandler)
    {
      delete iDocHandler;
      iDocHandler = NULL;
    }
    iDocHandler = CDocumentHandler::NewL();

    // not sure, if this is needed or not - but menubar does it, so we do too
    CAiwGenericParamList& param = iDocHandler->InParamListL();
    param.AppendL(TAiwGenericParam(EGenericParamURL,TAiwVariant(aUrl)));

    TPtrC file;
    TDataType emptyType;
    if (aPrefix)
    {
      // make a descriptor pointer to the string without 'file://'-prefix
      file.Set( aUrl.Mid( TPtrC(KFileScheme).Length() ));
    }
    else
    {
      file.Set( aUrl );
    }
    User::LeaveIfError( iDocHandler->OpenFileL(file, emptyType) );

    // the actions below are a kind of a hack to overcome issue, when the
    // application to be launched is already running: we have to order the app
    // to switch the file. This actually should be document handler's job, but
    // it does not offer any mechanism to do it.
    // However, the part below is unnecessary, if the app was not already
    // up... but it should not cause problems either, just extra actions.
    // And we don't care if this fails.
    TUid uid;
    if (KErrNone == iDocHandler->HandlerAppUid(uid))
    {
      RWsSession ws;
      if (KErrNone == ws.Connect())
      {
        TApaTaskList taskList( ws );
        TApaTask task = (taskList.FindApp(uid));
        if (task.Exists())
        {
          task.SwitchOpenFile(file);
        }
        ws.Close();
      }
    }
  }

/* ======================================================================= */
/* ECOM stuff                                                              */
/* ======================================================================= */

// Fix for RVCT Error #144: a value of type "MEcmtPlugin *(*)()" cannot
// be used to initialize an entity of type "TProxyNewLPtr"
static TAny* NewPushURLPluginL()
  {
    return CPushURL::NewL();
  }

const TImplementationProxy ImplementationTable[] =
  {
    {{KPUSH_URL_PLUGIN_IMPL_UID}, NewPushURLPluginL}
  };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
  {
    aCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
  }
