/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface to the SDK plugin
*
*/

#ifndef _ECMTSDKPLUGIN_H_
#define _ECMTSDKPLUGIN_H_

#include "..\..\..\..\sdkconnectivityfw\emuconnectserver\Core\inc\EcmtPlugin.h"

/**
 * A plugin that allows to execute arbitrary callbacks in context
 * of the ECMT process.
 */
class CEcmtSdkPlugin : public MEcmtPlugin
{
private:
    class CAppUi;
    class CRunner;
    class CStopper;
    class WorkItem;
    friend class CRunner;

    // Flags
    enum _Flag
    {
        EWaitThreadCreated = 0x0001,
        EWaitThreadExiting = 0x0002,
        EWorkThreadCreated = 0x0004
    };

    // Panic codes
    enum _Panic
    {
        EInvalidThread = 1,
        ENoActiveThread
    };

public:

    // The callback invoked on the Ecmt thread
    typedef void (*TCallback)(TAny* aArg1, TAny* aArg2, TAny* aArg3);

private:
    TDblQue<WorkItem> iQueue;
    CRunner* iRunner;
    CStopper* iStopper;
    void* iMutex;
    void* iEvent;
    RThread iWaitThread;
    RThread iWorkThread;
    TRequestStatus iWaitThreadStatus;
    TInt iFlags;

public:
    static MEcmtPlugin* NewL();

    virtual ~CEcmtSdkPlugin();
    virtual void BindMessagingL(MEcmtMessaging* aMessaging);

    // Checks if Ecmt SDK plugin has been loaded
    // Can be invoked from any (Symbian or non-Symbian) thread context
    EXPORT_C static TBool IsStarted();

    // Invokes the callback in the context of the Ecmt process.
    // Returns ETrue if callback has been successfully scheduled
    // for execution, EFalse if it wasn't. The most typical reason
    // for returning EFalse is that Ecmt process hasn't been started.
    // Other reasons include system failure which is rather unlikely.
    //
    // THIS FUNCTION MAY LOCK THE EMULATOR IF INVOKED FROM CONTEXT OF
    // A SYMBIAN THREAD. IT IS SUPPOSED TO BE CALLED FROM NON-SYMBIAN
    // THREADS
    // 
    EXPORT_C static TBool InvokeLater(TCallback aCallback,
                                      const void* aArg1 = NULL, 
                                      const void* aArg2 = NULL, 
                                      const void* aArg3 = NULL);

    // Invokes the callback in the context of the Ecmt process and
    // waits until the callback returns. Returns ETrue if callback 
    // has been successfully invoked, EFalse if it wasn't. The most 
    // typical reason for returning EFalse is that Ecmt process hasn't
    // been started. Other reasons include system failure which is
    // rather unlikely.
    //
    // THIS FUNCTION MAY LOCK THE EMULATOR IF INVOKED FROM CONTEXT OF
    // A SYMBIAN THREAD. IT IS SUPPOSED TO BE CALLED FROM NON-SYMBIAN
    // THREADS
    //
    EXPORT_C static TBool InvokeAndWait(TCallback aCallback, 
                                        const void* aArg1 = NULL,
                                        const void* aArg2 = NULL,
                                        const void* aArg3 = NULL);

private:
    CEcmtSdkPlugin();
    void ConstructL();

    TInt SubmitInternal(TCallback aCallback, 
                        const void* aArg1 = NULL, 
                        const void* aArg2 = NULL, 
                        const void* aArg3 = NULL);

    TBool DoInvokeLater(TCallback aCallback, 
                        const void* aArg1, 
                        const void* aArg2, 
                        const void* aArg3);

    TBool DoInvokeAndWait(TCallback aCallback, 
                          const void* aArg1, 
                          const void* aArg2, 
                          const void* aArg3);

    void QueueInternal(WorkItem* aWorkItem);
    void Queue(WorkItem* aWorkItem);
    void FlushQueue(TBool aFinal);
    void WaitThread();
    void WorkThread();
    static TInt WaitThread(TAny* aParam);
    static TInt WorkThread(TAny* aParam);
    static void Panic(TInt aReason);
};

#endif // _ECMTSDKPLUGIN_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
