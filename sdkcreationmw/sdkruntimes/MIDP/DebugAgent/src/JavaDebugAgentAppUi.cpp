/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#include <avkon.hrh>
#include <eikmenup.h>
#include "JavaDebugAgent.hrh"
#include "JavaDebugAgentAppUi.h"
#include "JavaDebugAgentLog.h"
#include "JavaDebugAgentLogView.h"
#include "JavaDebugAgentFullScreenLogView.h"
#include "JavaDebugAgentSettingsView.h"
#include "JavaDebugAgentSettings.h"
#include "JavaDebugAgentDriver.h"
#include "JavaDebugAgentRes.h"

#define SUPER CAknViewAppUi

CJavaDebugAgentAppUi::CJavaDebugAgentAppUi() :
iAsyncDeleteDriver(CActive::EPriorityStandard)
{
    TCallBack cb(AsyncDeleteDriver,this);
    iAsyncDeleteDriver.Set(cb);
}

void CJavaDebugAgentAppUi::ConstructL()
{
    TInt setResId;
    BaseConstructL(EAknEnableSkin);

    // Create the right log view
    TInt localPort = 0;
    TInt cmdLineLen = User::CommandLineLength();
    if (cmdLineLen > 0)
    {
        // Parse command line
        HBufC* rawArgs = HBufC::NewLC(cmdLineLen);
        TPtr ptr = rawArgs->Des();
        User::CommandLine(ptr);
        TLex lex(*rawArgs);
        User::LeaveIfError(lex.Val(localPort));
        if (localPort <= 0 || localPort > 0xffff) User::Leave(KErrArgument);
        CleanupStack::PopAndDestroy(rawArgs);

        // Create full screen view
        iLogView = CJavaDebugAgentFullScreenLogView::NewLC();
        setResId = R_JAVA_DEBUG_AGENT_SHORT_SETTINGS_LIST;
        iDefaultSoftKeys = R_JAVA_DEBUG_AGENT_FULL_SCREEN_SOFTKEYS;
        iSlaveMode = ETrue;
    }
    else
    {
        iLogView = CJavaDebugAgentLogView::NewLC();
        setResId = R_JAVA_DEBUG_AGENT_SETTINGS_LIST;
        iDefaultSoftKeys = R_AVKON_SOFTKEYS_OPTIONS_EXIT;
        iSlaveMode = EFalse;
    }

    // Add log view
    AddViewL(iLogView);
    CleanupStack::Pop(iLogView);

    // Add settings view
    CJavaDebugAgentLogScreen* logScreen = iLogView->Screen();
    iSettingsView = CJavaDebugAgentSettingsView::NewLC(logScreen, setResId);
    AddViewL(iSettingsView);
    CleanupStack::Pop(iSettingsView);

    ActivateLocalViewL(iLogView->Id());
    logScreen->Log(_S("Ready"));
    
    // If we have the port number, start listening
    if (localPort)
    {
        CJavaDebugAgentDriver* driver = NewDriverLC();
        driver->SetAgentPort(localPort);
        driver->StartListeningL();
        CleanupStack::Pop(driver);
        iDriver = driver;
    }
}

CJavaDebugAgentAppUi::~CJavaDebugAgentAppUi()
{
    iLogView = NULL;    // View is already dead
    if (iDriver)
    {
        iDriver->SetObserver(NULL);
        delete iDriver;
    }
}

void CJavaDebugAgentAppUi::HandleResourceChangeL(TInt aType)
{
    SUPER::HandleResourceChangeL(aType);
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        iLogView->HandleClientRectChange();
        iSettingsView->HandleClientRectChange();
    }
}

void CJavaDebugAgentAppUi::DynInitMenuPaneL(TInt aResId, CEikMenuPane* aMenu)
{
    if (aResId == R_JAVA_DEBUG_AGENT_LOG_MENU ||
        aResId == R_JAVA_DEBUG_AGENT_LOG_POPUP_MENU)
    {
#ifdef SUPPORT_CONNECT_FROM_IDE
        const CJavaDebugAgentSettings* settings = iSettingsView->Settings();
        TBool canListen = !iDriver && !settings->ConnectToPC();
        TBool canConnect = !iDriver && settings->ConnectToPC();
#else  // SUPPORT_CONNECT_FROM_IDE
        TBool canListen = EFalse;
        TBool canConnect = !iDriver;
#endif // SUPPORT_CONNECT_FROM_IDE
        TBool canStop = (iDriver && (!iSlaveMode || 
            iDriver->State() == CJavaDebugAgentDriver::EListening));
        TBool stopLabel = 
            (iCurrentSoftKeys == R_JAVA_DEBUG_AGENT_CANCEL_SOFTKEYS) ?
            R_JAVA_DEBUG_AGENT_MENU_ITEM_LABEL_CANCEL :
            R_JAVA_DEBUG_AGENT_MENU_ITEM_LABEL_STOP;

        aMenu->SetItemDimmed(EJavaDebugAgentListen, !canListen);
        aMenu->SetItemDimmed(EJavaDebugAgentConnect, !canConnect);
        aMenu->SetItemDimmed(EJavaDebugAgentStop, !canStop);
        aMenu->SetItemTextL(EJavaDebugAgentStop, stopLabel);

    }
    else if (aResId == R_JAVA_DEBUG_AGENT_FULL_SCREEN_LOG_MENU || 
             aResId == R_JAVA_DEBUG_AGENT_FULL_SCREEN_LOG_POPUP_MENU)
    {

        TBool canStop = (iDriver->State()==CJavaDebugAgentDriver::EListening);
        aMenu->SetItemDimmed(EJavaDebugAgentStop, !canStop);
    }
}

CJavaDebugAgentDriver* CJavaDebugAgentAppUi::NewDriverLC()
{
    DeleteDriver();
    MJavaDebugAgentLog* log = iLogView->Screen();
    CJavaDebugAgentDriver* drv = CJavaDebugAgentDriver::NewLC(log);
    iSettingsView->Settings()->ApplyTo(drv);
    drv->SetObserver(this);
    drv->InitL();
    return drv;
}

void CJavaDebugAgentAppUi::DeleteDriver()
{
    if (iDriver)
    {
        iDriver->Shutdown();
        delete iDriver;
        iDriver = NULL;
    }
}

TInt CJavaDebugAgentAppUi::AsyncDeleteDriver(TAny* aThis)
{
    ((CJavaDebugAgentAppUi*)aThis)->DeleteDriver();
    return KErrNone;
}

void CJavaDebugAgentAppUi::HandleCommandL(TInt aCommand)
{
    switch (aCommand)
    {
    case EEikCmdExit:
    case EAknSoftkeyExit:
        iLogView->Screen()->Log(_S("Exiting"));
        Exit();
        break;

    case EJavaDebugAgentListen:
        TRAP_IGNORE(HandleListenL());
        break;

    case EJavaDebugAgentConnect:
        TRAP_IGNORE(HandleConnectL());
        break;

    case EJavaDebugAgentStop:
        if (iDriver)
        {
            CJavaDebugAgentDriver::TState state = iDriver->State();
            iDriver->Shutdown();
            delete iDriver;
            iDriver = NULL;
            if (iSlaveMode)
            {
                Exit();
            }
        }
        break;

    case EJavaDebugAgentClearLog:
        iLogView->Screen()->ClearLog();
        break;

    case EJavaDebugAgentEditSettings:
        ActivateLocalViewL(TUid::Uid(KJavaDebugAgentSettingsViewId));
        break;

    default:
        break;
    }
}

void CJavaDebugAgentAppUi::HandleListenL()
{
    CJavaDebugAgentDriver* driver = NewDriverLC();
    driver->StartListeningL();
    CleanupStack::Pop(driver);
    iDriver = driver;
}

void CJavaDebugAgentAppUi::HandleConnectL()
{
    CJavaDebugAgentDriver* driver = NewDriverLC();
    driver->ConnectL();
    CleanupStack::Pop(driver);
    iDriver = driver;
}

// MJavaDebugAgentObserver
void CJavaDebugAgentAppUi::StateChanged(CJavaDebugAgentDriver::TState aState)
{
    if (iLogView)
    {
	    CEikButtonGroupContainer* cba = iLogView->Cba();
        if (cba)
        {
            TInt softKeys = iDefaultSoftKeys;
            switch (aState)
            {
            case CJavaDebugAgentDriver::EConnecting:
            case CJavaDebugAgentDriver::EListening:
                softKeys = R_JAVA_DEBUG_AGENT_CANCEL_SOFTKEYS;
                break;
            case CJavaDebugAgentDriver::EDisconnected:
                iAsyncDeleteDriver.CallBack();
                break;
            default:
                break;
            }
            if (iCurrentSoftKeys != softKeys)
            {
                iCurrentSoftKeys = softKeys;
                TRAP_IGNORE(cba->SetCommandSetL(iCurrentSoftKeys));
                cba->DrawDeferred();

                // Sometimes Cba() and iLogView->Cba() return the same object,
                // sometimes different objects. Apparently there are quite a
                // few different CBAs on so-called "control stack" at any
                // given point of time, and they somehow interfere with each
                // other. I don't pretend to fully understand what that all
                // means and how it's supposed to work, but let's try to get
                // them all (or at least some of them) do the same thing.
                // That seems to help.
                CEikButtonGroupContainer* appCba = Cba();
                if (appCba != cba) {
                    TRAP_IGNORE(appCba->SetCommandSetL(iCurrentSoftKeys));
                    appCba->DrawDeferred();
                }
            }
        }
    }
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
