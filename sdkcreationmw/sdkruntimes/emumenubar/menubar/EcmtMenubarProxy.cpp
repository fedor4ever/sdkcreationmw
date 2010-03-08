/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Windows specific class and utilities
 *
*/


#include "EcmtMenubar.h"
#include "EcmtMenubarProxy.h"
#include "EcmtMenubarUtils.h"
#include "EcmtSdkPlugin.h"

/**
 * Callbacks invoked on Ecmt thread
 */
typedef struct _MsgData {
    unsigned int iUid;
    const void* iData;
    int iSize;
} MsgData;
typedef struct _LoadFileData {
    unsigned int iBrowserUid;
    unsigned short* iFileName;
    unsigned short* iFragment;
} LoadFileData;
typedef struct _RunExeData {
    const unsigned short* iExe;
    const unsigned short** iArgs;
    int iArgCount;
    unsigned long long iProcessId;
} RunExeData;
static void SendEcmtMessageCB(TAny* aP1,TAny* aP2,TAny*)
{
	if (aP1!= NULL)
	{
    	MEcmtMenubar* target = (MEcmtMenubar*)aP1;
    	MsgData* data = (MsgData*)aP2;
    	target->SendEcmtMessage(data->iUid, data->iData, data->iSize);		
	}

}
static void SmartLoadCB(TAny* aP1,TAny* aP2,TAny* aP3)
{
    MEcmtMenubar* target = (MEcmtMenubar*)aP1;
    target->SmartLoad((unsigned int)aP2,(unsigned short*)aP3);
}
static void LoadFileCB(TAny* aP1, TAny* aP2, TAny*)
{
    MEcmtMenubar* target = (MEcmtMenubar*)aP1;
    LoadFileData* data = (LoadFileData*)aP2;
    target->LoadFile(data->iBrowserUid, data->iFileName, data->iFragment);
    EcmtOsUtils::Free (data->iFragment);
    EcmtOsUtils::Free (data->iFileName);
    EcmtOsUtils::Free(data);
}
static void LaunchBrowserCB(TAny* aP1, TAny* aP2, TAny* aP3)
{
	if (aP1!= NULL)
	{
   	 	MEcmtMenubar* target = (MEcmtMenubar*)aP1;
    	target->LaunchBrowser((unsigned int)aP2,(unsigned short*)aP3);		
	}

}
static void RunExeCB(TAny* aP1, TAny* aP2, TAny* aP3)
{
	if (aP1!= NULL)
	{
    	MEcmtMenubar* target = (MEcmtMenubar*)aP1;
    	RunExeData* d = (RunExeData*)aP2;
    	bool* result = (bool*)aP3;
    	*result = target->RunExe(d->iExe, d->iArgs, d->iArgCount, &d->iProcessId);		
	}

}
static void IsEmulatorStartedCB(TAny* aP1, TAny* aP2, TAny*)
{
	if (aP1!= NULL)
	{
		MEcmtMenubar* target = (MEcmtMenubar*)aP1;
    	bool* result = (bool*)aP2;
    	(*result) = target->IsEmulatorStarted();
	}
}
  
static void IsCoreUpCB(TAny* aP1, TAny* aP2, TAny*)
{
	if (aP1!= NULL)
	{
		MEcmtMenubar* target = (MEcmtMenubar*)aP1;
    	bool* result = (bool*)aP2;
    	(*result) = target->IsCoreUp();		
	}

}
static void ReleaseCB(TAny* aP1, TAny*, TAny*)
{
	if (aP1!= NULL)
	{
    	MEcmtMenubar* target = (MEcmtMenubar*)aP1;
    	target->Release();		
	}

}

/**
 * Construction and destruction
 */
EcmtMenubarProxy* EcmtMenubarProxy::New(MEcmtMenubarCallbacks* aCallbacks)
{
    EcmtMenubarProxy* self = new EcmtMenubarProxy;
    if (self)
    {
        if (self->Construct(aCallbacks)) return self;
        delete self;
    }
    return NULL;
}

EcmtMenubarProxy::EcmtMenubarProxy() : 
    iEcmtMenubar(NULL), 
    iStarted(false)
{
}

EcmtMenubarProxy::~EcmtMenubarProxy()
{
    if (iEcmtMenubar) 
    {
        // CEcmtMenubar must be destroyed in context of a Symbian thread
        CEcmtSdkPlugin::InvokeAndWait(ReleaseCB, iEcmtMenubar);
    }
}

bool EcmtMenubarProxy::Construct(MEcmtMenubarCallbacks* aCallbacks)
{
    iEcmtMenubar = new CEcmtMenubar(aCallbacks);
    return (iEcmtMenubar != NULL);
}

/**
 * Context-safe new and delete operators
 */
void* EcmtMenubarProxy::operator new(unsigned int aSize)
{
    return EcmtOsUtils::Alloc(aSize);
}

void EcmtMenubarProxy::operator delete(void* aPtr)
{
    EcmtOsUtils::Free(aPtr);
}

/**
 * Sends a Ecmt message with the specified UID to the Java side.
 */
bool EcmtMenubarProxy::SendEcmtMessage(unsigned int aUid,
                                       const void* aMessageData,
                                       int aMessageSize)
{
    MsgData data;
    data.iUid = aUid;
    data.iData = aMessageData;
    data.iSize = aMessageSize;
    return CEcmtSdkPlugin::InvokeAndWait(SendEcmtMessageCB,
        iEcmtMenubar, &data);
}

/**
 * Loads a URL into the browser. File URLs are handled differently
 * from other URLs, they get transformed into local: URLs, and file
 * name gets URL-encoded. The fragment part of the URL is preserved.
 * Other URLs are passed to the browser as is.
 */
bool EcmtMenubarProxy::SmartLoad(unsigned int aBrowserUid,
                                 const unsigned short* aUrl)
{
    return CEcmtSdkPlugin::InvokeAndWait(SmartLoadCB, 
        iEcmtMenubar, (void*)aBrowserUid, aUrl);
}

/**
 * Loads a file into the emulator, typically (but not necessarily) 
 * into the built-in browser.
 */
bool EcmtMenubarProxy::LoadFile(unsigned int aBrowserUid,
                                const unsigned short* aFileName, 
                                const unsigned short* aFragment)
{
    LoadFileData* data = (LoadFileData *) EcmtOsUtils::Alloc(sizeof(LoadFileData));
    if (data) {
        data->iBrowserUid = aBrowserUid;
        TInt len = 2 * (User::StringLength(aFileName) + 1);

        data->iFileName = (TText *)EcmtOsUtils::Alloc (len);
        if (data->iFileName) {
          Mem::Copy (data->iFileName, aFileName, len);
          if (aFragment) {
            len = 2 * (User::StringLength(aFragment) + 1);
            data->iFragment = (TText *)EcmtOsUtils::Alloc (len);
          }
          if (!aFragment || data->iFragment) {
            if (data->iFragment) {
              Mem::Copy (data->iFragment, aFragment, len);
            }
            if (CEcmtSdkPlugin::InvokeLater(LoadFileCB,
              iEcmtMenubar, data)) {
              return true;
            }
            EcmtOsUtils::Free (data->iFragment);
          }
          EcmtOsUtils::Free (data->iFileName);
        }
        EcmtOsUtils::Free (data);

    }
    return false;
}

/**
 * Launches the browser with the specified URL. 
 */
bool EcmtMenubarProxy::LaunchBrowser(unsigned int aBrowserUid,
                                     const unsigned short* aUrl)
{
    return CEcmtSdkPlugin::InvokeAndWait(LaunchBrowserCB,
        iEcmtMenubar, (void*)aBrowserUid, aUrl);
}

/**
 * Runs a Symbian executable.
 */
bool EcmtMenubarProxy::RunExe(const unsigned short* aExe,
                              const unsigned short** aArgs, int aArgCount,
                              unsigned long long* aProcessId)
{
    bool ok = false;
    RunExeData data;
    data.iExe = aExe;
    data.iArgs = aArgs;
    data.iArgCount = aArgCount;
    data.iProcessId = 0;
    CEcmtSdkPlugin::InvokeAndWait(RunExeCB, iEcmtMenubar, &data, &ok);
    if (aProcessId) *aProcessId = data.iProcessId;
    return ok;
}

/**
 * Tests whether the emulator is up and running. If invoked during the 
 * startup process, returns EFalse.
 */
bool EcmtMenubarProxy::IsEmulatorStarted()
{
    if (!iStarted)
    {
        CEcmtSdkPlugin::InvokeAndWait(IsEmulatorStartedCB, iEcmtMenubar,
            &iStarted);
    }
    return iStarted;
}

/**
 * Tests whether Ecmt core (server) is up. 
 */
bool EcmtMenubarProxy::IsCoreUp()
{
    bool up = false;
    CEcmtSdkPlugin::InvokeAndWait(IsCoreUpCB, iEcmtMenubar, &up);
    return up;
}

/**
 * Self-destructs
 */
void EcmtMenubarProxy::Release()
{
    delete this;
}

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
