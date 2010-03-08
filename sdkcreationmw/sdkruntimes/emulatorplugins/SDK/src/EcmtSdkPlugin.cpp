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
* Description:  SDK plugin to Ecmt
*
*/


#include <ecom.h>
#include <eikenv.h>
#include <eikappui.h>
#include <ImplementationProxy.h>
#include "EcmtServerDebug.h"
#include "EcmtMessagingIf.h"
#include "EcmtSdkPlugin.h"
#include "EcmtSdkPluginUids.h"

#include <emulator.h>

// Don't use C-runtime malloc and free functions here. They don't work.
// Also, standard User::Alloc and operator new won't work either because
// we need to invoke them from a strange context (emulator's UI thread)
// Only Win32 API always works :)
#define MemAllocZ(size) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size)
#define MemFree(ptr) HeapFree(GetProcessHeap(),0,ptr)

// Static instance of the plugin
static CEcmtSdkPlugin* gEcmtSdkPlugin = NULL;
static HANDLE gEcmtSdkPluginThread = NULL;

//===========================================================================
// Respect Ecmt server-wide debug trace configuration
//===========================================================================

#ifdef ECMT_RDEBUG
#  define _REALLY_DEBUG
#else // !ECMT_RDEBUG
#  undef _REALLY_DEBUG
#endif // !ECMT_RDEBUG

//===========================================================================
// Debug trace
//===========================================================================

#define TRACE_PREFIX "EcmtSdkPlugin: "

#ifdef _REALLY_DEBUG

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

//===========================================================================
// Debug assert
//===========================================================================

#undef ASSERT
#ifdef _REALLY_DEBUG
void EcmtSdkAssert(const TText* s,const TText* file,TInt line);
void EcmtSdkLeaveIfError(TInt err,const TText* s,const TText* file,TInt line);
#  define __S(x) _S(x)
#  define ASSERT(x) ((void)((x)||(EcmtSdkAssert(_S(#x),__S(__FILE__),__LINE__),0),0))
#  define LEAVE_IF_ERROR(x) EcmtSdkLeaveIfError(x,_S(#x),__S(__FILE__),__LINE__)
#  define VERIFY_SUCCESS(x) ASSERT((x) == KErrNone)
#else  // _DEBUG
#  define ASSERT(x)         ((void)0)
#  define VERIFY_SUCCESS(x) (x)
#  define LEAVE_IF_ERROR(x) User::LeaveIfError(x)
#endif // _DEBUG

#ifdef _REALLY_DEBUG
extern "C" int __stdcall IsDebuggerPresent(void);
void EcmtSdkAssert(const TText* s,const TText* file,TInt /*line*/)
{
    // TRACE macro panics outside of a Symbian thread context
    OutputDebugStringA(TRACE_PREFIX "Assertion failed: ");
    OutputDebugString(s);
    OutputDebugStringA("\nFile: ");
    OutputDebugString(file);
    OutputDebugStringA("\n");
    //TRACE1("Line: %d",line);
    if (IsDebuggerPresent()) {
        __BREAKPOINT();
    }
}
void EcmtSdkLeaveIfError(TInt err,const TText* s,const TText* file,TInt line)
{
    if (err != KErrNone) {
        TRACE2("Error %d in %s",err,s);
        TRACE1("File: %s",file);
        TRACE1("Line: %d",line);
        if (IsDebuggerPresent()) {
            __BREAKPOINT();
        }
        User::Leave(err);
    }
}
#endif // _REALLY_DEBUG

//===========================================================================
// CEcmtSdkPlugin::WorkItem
//===========================================================================

class CEcmtSdkPlugin::WorkItem : public TDblQueLink
{
public:
    TCallback iCallback;
    void* iArg1;
    void* iArg2;
    void* iArg3;
    HANDLE iEvent;
};

//===========================================================================
// CEcmtSdkPlugin::CRunner
//===========================================================================

class CEcmtSdkPlugin::CRunner : public CActive
{
friend class CEcmtSdkPlugin;
public:
    CRunner(CEcmtSdkPlugin* aPlugin);
    void DoCancel();
    void RunL();
private:
    CEcmtSdkPlugin* iPlugin;
};

CEcmtSdkPlugin::CRunner::CRunner(CEcmtSdkPlugin* aPlugin) : 
CActive(EPriorityStandard), iPlugin(aPlugin)
{
    // Will be changed to KRequestPending by plugin thread
    iStatus = KErrNone;
}

void CEcmtSdkPlugin::CRunner::DoCancel()
{
    TRequestStatus *status = (&iStatus);
    User::RequestComplete(status, KErrCancel);
}

void CEcmtSdkPlugin::CRunner::RunL()
{
    iPlugin->FlushQueue(EFalse);
}

//===========================================================================
// CEcmtSdkPlugin::CStopper
//===========================================================================

class CEcmtSdkPlugin::CStopper : public CActive
{
public:
    CStopper();
    void DoCancel();
    void RunL();
    void Install();
};

CEcmtSdkPlugin::CStopper::CStopper() : CActive(EPriorityStandard)
{ 
    iStatus = KRequestPending;
}

void CEcmtSdkPlugin::CStopper::Install()
{
    CActiveScheduler::Add(this);
    SetActive();
}

void CEcmtSdkPlugin::CStopper::DoCancel()
{
    TRequestStatus *status = (&iStatus);
    User::RequestComplete(status, KErrCancel);
    delete this;
}

void CEcmtSdkPlugin::CStopper::RunL()
{
    CActiveScheduler::Stop();
    delete this;
}


//===========================================================================
// CEcmtSdkPlugin::CAppUi
//===========================================================================

class CEcmtSdkPlugin::CAppUi : public CEikAppUi
{
public:
    static CCoeAppUi* NewL();
    virtual ~CAppUi();
};

CCoeAppUi* CEcmtSdkPlugin::CAppUi::NewL()
{
    CAppUi* self = new(ELeave)CAppUi;
    CleanupStack::PushL(self);
    self->BaseConstructL(ENoAppResourceFile|ENoScreenFurniture);
    CleanupStack::Pop(self);
    TRACE1("AppUi [%08X] created",self);
    return self;
}

CEcmtSdkPlugin::CAppUi::~CAppUi()
{
    TRACE1("AppUi [%08X] destroyed",this);
}

//===========================================================================
// CEcmtSdkPlugin
//===========================================================================

CEcmtSdkPlugin::CEcmtSdkPlugin() :
    iRunner(NULL),
    iStopper(NULL),
    iMutex(NULL),
    iEvent(NULL),
    iFlags(0)
{
}

CEcmtSdkPlugin::~CEcmtSdkPlugin()
{
    if (gEcmtSdkPlugin == this) {
        gEcmtSdkPlugin = NULL;
        CloseHandle(gEcmtSdkPluginThread);
        gEcmtSdkPluginThread = NULL;
    }

    if (iFlags & EWaitThreadCreated) {
        // Gracefully terminate the wait thread
        Emulator::Escape();
        WaitForSingleObject(iMutex, INFINITE);
        iFlags |= EWaitThreadExiting;
        SetEvent(iEvent);
        ReleaseMutex(iMutex);
        Emulator::Reenter();
        User::WaitForRequest(iWaitThreadStatus);
        iWaitThread.Close();
    }

    if (iFlags & EWorkThreadCreated) {
        // Stopper deletes itself when and if it gets chance to run.
        // We can't wait here until the thread is done because it may 
        // be blocked on REcmt.Connect waiting for us to respond. If
        // EcmtCore app is being terminated, the notify thread will 
        // get terminated, so we don't have much to lose anyway.
        TRequestStatus *status = (&iStopper->iStatus);
        iWorkThread.RequestComplete(status, KErrNone);
        iWorkThread.Close();
    } else {
        delete iStopper;
    }

    FlushQueue(ETrue);
    delete iRunner;
    if (iMutex) CloseHandle(iMutex);
    if (iEvent) CloseHandle(iEvent);
    TRACE1("[%08X] destroyed",this);
}

// Symbian style constructor
MEcmtPlugin* CEcmtSdkPlugin::NewL()
{
    CEcmtSdkPlugin* self = new(ELeave)CEcmtSdkPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    TRACE1("[%08X] created",self);
    return self;
}

// Second stage construction
void CEcmtSdkPlugin::ConstructL()
{
    User::LeaveIfNull(iMutex = CreateMutex(NULL,EFalse,NULL));
    User::LeaveIfNull(iEvent = CreateEvent(NULL,ETrue,EFalse,NULL));
    iRunner = new(ELeave)CRunner(this);
    iStopper = new(ELeave)CStopper;
}

// Called by ECMT to initialize the plugin
void CEcmtSdkPlugin::BindMessagingL(MEcmtMessaging* /*aMessaging*/)
{
    // Create the thread waiting on the native Win32 event
    _LIT(KName,"EcmtSdkPluginWaitThread");
    User::LeaveIfError(iWaitThread.Create(KName(), WaitThread,
        KDefaultStackSize, NULL, this));
    iFlags |= EWaitThreadCreated;
    iWaitThread.Logon(iWaitThreadStatus);
    iWaitThread.Resume();

    // Create the thread that runs the work items
    _LIT(KName2,"EcmtSdkPluginWorkThread");
    User::LeaveIfError(iWorkThread.Create(KName2(), WorkThread,
        KDefaultStackSize, NULL, this));
    iFlags |= EWorkThreadCreated;
    iWorkThread.Resume();

    TRACE1("[%08X] initialized",this);
    ASSERT(!gEcmtSdkPlugin);
    gEcmtSdkPlugin = this;

    DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),
                    GetCurrentProcess(),&gEcmtSdkPluginThread,
                    0, FALSE, DUPLICATE_SAME_ACCESS);
}

// Activates work items sitting in the queue
void CEcmtSdkPlugin::FlushQueue(TBool aFinal)
{
    // Lock the queue
    Emulator::Escape();
    WaitForSingleObject(iMutex, INFINITE);
    Emulator::Reenter();

    // Move queued callbacks to a temp queue
    TDblQue<WorkItem> q;
    while (!iQueue.IsEmpty()) {
        WorkItem* workItem = iQueue.Last();
        workItem->Deque();
        q.AddFirst(*workItem);
    }
    if (aFinal) {
        iRunner->Cancel();
    } else {
        iRunner->iStatus = KRequestPending;
        iRunner->SetActive();
    }
    ReleaseMutex(iMutex);

    // Invoke the callbacks
    while (!q.IsEmpty()) {
        WorkItem* w = q.First();
        w->Deque();
        TRACE2("[%08X] running work item %08X",this,w);
	
	if ((*w->iCallback != NULL) && (w->iArg1 != NULL))
	{ 
        TRAPD(err,(*w->iCallback)(w->iArg1,w->iArg2,w->iArg3));
	}
	 if (w->iEvent)
       	 	{
            		TRACE2("[%08X] setting event for work item %08X",this,w);
           		SetEvent(w->iEvent);
        	}
        MemFree(w);
    }
}

// Panics the current thread
void CEcmtSdkPlugin::Panic(TInt aReason)
{
    _LIT(KPanicCategory,"CEcmtSdkPlugin");
    User::Panic(KPanicCategory(), aReason);
}

/**
 * Checks if Ecmt SDK plugin has been loaded
 * Can be invoked from any (Symbian or non-Symbian) thread context
 */
TBool CEcmtSdkPlugin::IsStarted()
{
	BOOL val = FALSE;
    DWORD exitCode;
	val = GetExitCodeThread(gEcmtSdkPluginThread,&exitCode);
    return (gEcmtSdkPlugin && val
             && exitCode == STILL_ACTIVE);
}

/**
 * Invokes the callback in the context of the Ecmt process.
 * Returns ETrue if callback has been successfully scheduled
 * for execution, EFalse if it wasn't. The most typical reason
 * for returning EFalse is that Ecmt process hasn't been started.
 * Other reasons include system failure which is rather unlikely.
 *
 * THIS FUNCTION MAY LOCK THE EMULATOR IF INVOKED FROM CONTEXT OF
 * A SYMBIAN THREAD. IT IS SUPPOSED TO BE CALLED FROM NON-SYMBIAN
 * THREADS
 */
TBool CEcmtSdkPlugin::InvokeLater(TCallback aCallback,
                                  const void* aArg1,
                                  const void* aArg2,
                                  const void* aArg3)
{
    if (gEcmtSdkPlugin) {
        return gEcmtSdkPlugin->DoInvokeLater(aCallback, aArg1, aArg2, aArg3);
    } else {
        return EFalse;
    }
}

/**
 * Invokes the callback in the context of the Ecmt process and
 * waits until the callback returns. Returns ETrue if callback 
 * has been successfully invoked, EFalse if it wasn't. The most 
 * typical reason for returning EFalse is that Ecmt process hasn't
 * been started. Other reasons include system failure which is
 * rather unlikely.
 *
 * THIS FUNCTION MAY LOCK THE EMULATOR IF INVOKED FROM CONTEXT OF
 * A SYMBIAN THREAD. IT IS SUPPOSED TO BE CALLED FROM NON-SYMBIAN
 * THREADS
 */
TBool CEcmtSdkPlugin::InvokeAndWait(TCallback aCallback, 
                                    const void* aArg1,
                                    const void* aArg2,
                                    const void* aArg3)
{
    if (gEcmtSdkPlugin) {
        return gEcmtSdkPlugin->DoInvokeAndWait(aCallback, aArg1, aArg2, aArg3);
    } else {
        return EFalse;
    }
}

/**
 * Submits internal work item. Must be invoked on a Symbian thread
 */
TInt CEcmtSdkPlugin::SubmitInternal(TCallback aCallback, 
                                    const void* aArg1, 
                                    const void* aArg2, 
                                    const void* aArg3)
{
    WorkItem* workItem = (WorkItem*)MemAllocZ(sizeof(WorkItem));
    if (workItem) {
        workItem->iCallback = aCallback;
        workItem->iArg1 = (void*)aArg1;
        workItem->iArg2 = (void*)aArg2;
        workItem->iArg3 = (void*)aArg3;
        QueueInternal(workItem);
        return KErrNone;
    }
    return KErrNoMemory;
}

/**
 * Internal implementation of InvokeLater
 */
TBool CEcmtSdkPlugin::DoInvokeLater(TCallback aCallback,
                                    const void* aArg1, 
                                    const void* aArg2,
                                    const void* aArg3)
{
    WorkItem* workItem = (WorkItem*)MemAllocZ(sizeof(WorkItem));
    if (workItem) {
        workItem->iCallback = aCallback;
        workItem->iArg1 = (void*)aArg1;
        workItem->iArg2 = (void*)aArg2;
        workItem->iArg3 = (void*)aArg3;
        Queue(workItem);
        return ETrue;
    }
    return EFalse;
}

/**
 * Internal implementation of InvokeAndWait
 */
TBool CEcmtSdkPlugin::DoInvokeAndWait(TCallback aCallback,
                                      const void* aArg1, 
                                      const void* aArg2,
                                      const void* aArg3)
{
    WorkItem* workItem = (WorkItem*)MemAllocZ(sizeof(WorkItem));
    if (workItem) {
        HANDLE hEvent = CreateEvent(NULL, ETrue, EFalse, NULL);
        if (hEvent) {
            workItem->iCallback = aCallback;
            workItem->iArg1 = (void*)aArg1;
            workItem->iArg2 = (void*)aArg2;
            workItem->iArg3 = (void*)aArg3;
            workItem->iEvent = hEvent;
            Queue(workItem);
            
            // Here we assume that the caller is not running in context 
            // of a Symbian thread. If we are being "scheduled" by the 
            // Symbian scheduler, we must call Emulator::Escape() before
            // supending the current thread, and Emulator::Reenter() after
            // we are done waiting. If we do that on a non-Symbian thread,
            // we get a panic.
            DWORD err;
            do {
                // When emulator is shutting down, it terminates the threads
                // and we may end up waiting forever. Hence this check.
                err = WaitForSingleObject(hEvent, 5000);
            } while (err == WAIT_TIMEOUT && IsStarted());
            ASSERT(err == NO_ERROR);
            CloseHandle(hEvent);
            return (err == NO_ERROR);
        }
        MemFree(workItem);
    }
    return EFalse;
}

/**
 * Queues a work item and wakes up the Ecmt thread
 */
void CEcmtSdkPlugin::Queue(WorkItem* aWorkItem)
{
    // Here we assume that the caller is not running in context 
    // of a Symbian thread. If we are being "scheduled" by the 
    // Symbian scheduler, we must call Emulator::Escape() before
    // supending the current thread, and Emulator::Reenter() after
    // we are done waiting. If we do that on a non-Symbian thread,
    // we get a panic. 
    //KTRACE2("[%08X] queue work item %08X",this,aWorkItem);
    WaitForSingleObject(iMutex, INFINITE);
    iQueue.AddLast(*aWorkItem);
    if (iRunner->iStatus == KRequestPending) {
        // Wake up the wait thread, it will wake up the active object
        //KTRACE1("[%08X] waking up wait thread",this);
        SetEvent(iEvent);
    }
    ReleaseMutex(iMutex);
}

/**
 * Queues an internal work item and wakes up the Ecmt thread.
 * Must be invoked in context of a Symbian thread
 */
void CEcmtSdkPlugin::QueueInternal(WorkItem* aWorkItem)
{
    Emulator::Escape();
    WaitForSingleObject(iMutex, INFINITE);
    Emulator::Reenter();
    iQueue.AddLast(*aWorkItem);
    if (iRunner->iStatus == KRequestPending) {
        TRACE1("[%08X] queueing internal work item",this);
        TRequestStatus *status = (&iRunner->iStatus);
        iWorkThread.RequestComplete(status, KErrNone);
    }
    ReleaseMutex(iMutex);
}

//===========================================================================
// Wait thread. Note that the event travels from one thread to another
// twice. First, we wake up the wait thread that waits on a Win32 event.
// It then wakes up an active object running in context of the work thread.
//===========================================================================

void CEcmtSdkPlugin::WaitThread()
{
    TRACE1("[%08X] wait thread started",this);
    Emulator::Escape();
    WaitForSingleObject(iMutex, INFINITE);
    while (!(iFlags & EWaitThreadExiting)) {
        ResetEvent(iEvent);
        if (!iQueue.IsEmpty() && iRunner->iStatus == KRequestPending) {
            Emulator::Reenter();
            TRACE1("[%08X] activating ECMT thread",this);
            TRequestStatus *status = (&iRunner->iStatus);
            iWorkThread.RequestComplete(status, KErrNone);
            Emulator::Escape();
        }
        ReleaseMutex(iMutex);
        WaitForSingleObject(iEvent, INFINITE);
        WaitForSingleObject(iMutex, INFINITE);
    }
    ReleaseMutex(iMutex);
    Emulator::Reenter();
    TRACE1("[%08X] wait thread exiting",this);
}

TInt CEcmtSdkPlugin::WaitThread(TAny* aParam)
{
    CEcmtSdkPlugin* that = (CEcmtSdkPlugin*)aParam;
    that->WaitThread();
    return KErrNone;
}

//===========================================================================
// Ecmt client thread receiving notifications from the Ecmt server. We can't
// user REcmt session from context of the Ecmt server thread because it causes
// a deadlock.
//===========================================================================

void CEcmtSdkPlugin::WorkThread()
{
    TRACE1("[%08X] plugin thread started",this);
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (cleanupStack) {

        // Initialize fake ui environment
        CCoeAppUi* ui = NULL;
        CEikonEnv* coe = new CEikonEnv;
        TRAPD(err, coe->ConstructL(EFalse));
        if (err == KErrNone)
        {
            // And fake app ui
            TRAP(err, coe->SetAppUi(ui = CAppUi::NewL()));

            // See private TApaStatus constants in apgwgnam.h
            const TInt ESystem = 0x01;
            const TInt EIsHidden = 0x20;

            // What a hack! See apgwgnam.cpp for details.
            // Note that the caption shows up in Diagnostics.
            _LIT(KCaption, "EcmtSdkPlugin");
            _LIT(KFormatStatus, "%02x");
            _LIT(KFormatUID, "%08x");
            TBuf<28> wgName;  // at least 13 + caption length
            wgName.Format(KFormatStatus, ESystem|EIsHidden);
            wgName.Append(0);
            wgName.AppendFormat(KFormatUID, KSDKPLUGIN_DLL_UID);
            wgName.Append(0);
            wgName.Append(KCaption);
            wgName.Append(0);
            coe->RootWin().SetName(wgName);
        }

        // CEikonEnv installs active scheduler
        CActiveScheduler* as = CActiveScheduler::Current();
        if (!as)
        {
            // Something is wrong with CEikonEnv, create our own
            as = new CActiveScheduler;
            if (as) {
                CActiveScheduler::Install(as);
            }
        }

        // At very least we need a live active scheduler. We can survive
        // without everything else until user picks File -> Open
        if (as)
        {
            CActiveScheduler::Add(iRunner);
            iStopper->Install();
            FlushQueue(EFalse);
            CActiveScheduler::Start();
        }

        // We are not supposed to get here
        delete ui;
        delete coe;
        delete cleanupStack;
    }
    TRACE1("[%08X] plugin thread exiting",this);
}

TInt CEcmtSdkPlugin::WorkThread(TAny* aParam)
{
    CEcmtSdkPlugin* that = (CEcmtSdkPlugin*)aParam;
    that->WorkThread();
    return KErrNone;
}

//===========================================================================
// ECom stuff
//===========================================================================

static const TImplementationProxy ImplementationTable[] = 
{
    {{KSDKPLUGIN_STARTUP_IMPL_UID}, CEcmtSdkPlugin::NewL}
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    aCount = sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
    return ImplementationTable;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
