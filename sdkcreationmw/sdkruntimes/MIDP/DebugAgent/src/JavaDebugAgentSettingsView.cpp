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


#include <eiklbv.h>
#include <eiklbm.h>
#include <akntitle.h>
#include <barsread.h>
#include <aknviewappui.h>

#include "JavaDebugAgent.hrh"
#include "JavaDebugAgentRes.h"
#include "JavaDebugAgentLog.h"
#include "JavaDebugAgentDriver.h"
#include "JavaDebugAgentLogScreen.h"
#include "JavaDebugAgentSettings.h"
#include "JavaDebugAgentSettingsView.h"
#include "JavaDebugAgentSettingsScreen.h"

_LIT(KLogFileName, "c:\\midp2\\agent.log");

#define SUPER CAknView

// Settings view
CJavaDebugAgentSettingsView* 
CJavaDebugAgentSettingsView::NewLC(CJavaDebugAgentLogScreen* aLog, TInt aResId)
{
    CJavaDebugAgentSettingsView* self = 
        new(ELeave)CJavaDebugAgentSettingsView(aLog, aResId);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CJavaDebugAgentSettingsView::
CJavaDebugAgentSettingsView(CJavaDebugAgentLogScreen* aLog, TInt aListResId) : 
iLog(aLog), iListResId(aListResId)
{ 
}

void CJavaDebugAgentSettingsView::ConstructL()
{
    BaseConstructL(R_JAVA_DEBUG_AGENT_SETTINGS_VIEW);
    iCurrentSettings = CJavaDebugAgentSettings::NewL();
    iBackupSettings = CJavaDebugAgentSettings::NewL();
    TRAP_IGNORE(iCurrentSettings->LoadL());
    iBackupSettings->CopyFrom(iCurrentSettings);

    // Communicate current options to the logger
    iLog->SetLogFile(iCurrentSettings->FileLog() ? &KLogFileName() : NULL);
    iLog->SetFontSize(iCurrentSettings->FontSize());
}

CJavaDebugAgentSettingsView::~CJavaDebugAgentSettingsView()
{
    if (iScreen)
    {
        AppUi()->RemoveFromStack(iScreen);
        delete iScreen;
    }
    delete iCurrentSettings;
    delete iBackupSettings;
}

void CJavaDebugAgentSettingsView::ApplySettingsL()
{
    iCurrentSettings->SaveL();
    iBackupSettings->CopyFrom(iCurrentSettings);
}

TUid CJavaDebugAgentSettingsView::Id() const
{
    return TUid::Uid(KJavaDebugAgentSettingsViewId);
}

void CJavaDebugAgentSettingsView::HandleCommandL(TInt aCommand)
{
    switch (aCommand)
    {
    case EAknSoftkeyBack:
    case EAknSoftkeyDone:
        AppUi()->ActivateLocalViewL(iPreviousViewId);
        break;

    case EJavaDebugAgentChangeValue:
        if (iScreen)
        {
            MListBoxModel* model = iScreen->ListBox()->Model();
            CListBoxView* view = iScreen->ListBox()->View();
            TInt index = view->CurrentItemIndex();
            if (index >= 0 && index < model->NumberOfItems())
            {
                iScreen->EditItemL(index, ETrue);
            }
        }

    default:
        AppUi()->HandleCommandL(aCommand);
        break;
    }
}

// Called by CJavaDebugAgentAppUi on KEikDynamicLayoutVariantSwitch event
void CJavaDebugAgentSettingsView::HandleClientRectChange()
{
    if (iScreen)
    {
        iScreen->SetRect(AppUi()->ClientRect());
    }
}

void CJavaDebugAgentSettingsView::DoActivateL(const TVwsViewId& aPrevViewId,
                                              TUid, const TDesC8&)
{
    if (!iScreen)
    {
        iScreen = CJavaDebugAgentSettingsScreen::NewL(iCurrentSettings,
                                                      iListResId);
        iScreen->SetMopParent(this);
        iScreen->ActivateL();
    }
    iScreen->LoadSettingsL();

    // Get title pane
    CAknTitlePane* title = (CAknTitlePane*)AppUi()->StatusPane()->
        ControlL(TUid::Uid(EEikStatusPaneUidTitle));

    // Configure the title
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader, R_JAVA_DEBUG_AGENT_SETTINGS_TITLE);
    title->SetFromResourceL(reader);
    CleanupStack::PopAndDestroy();

    // Add control to stack
    AppUi()->AddToStackL(*this, iScreen);
    Cba()->MakeVisible(ETrue);
    StatusPane()->MakeVisible(ETrue);
    iScreen->SetRect(AppUi()->ClientRect());
    iScreen->MakeVisible(ETrue);
    iScreen->DrawNow();

    iPreviousViewId = aPrevViewId.iViewUid;
}

void CJavaDebugAgentSettingsView::DoDeactivate()
{
    if (iScreen)
    {
        TRAP_IGNORE(iScreen->StoreSettingsL());
        AppUi()->RemoveFromStack(iScreen);
        iScreen->MakeVisible(EFalse);
    }

    // Detect changes
    if (!iBackupSettings->Equals(iCurrentSettings))
    {
        TRAPD(err, ApplySettingsL());
        if (err == KErrNone) {
            iLog->Log(_S("Settings saved"));
        } else {
            iLog->LogFormat(_S("Failed to save settings, error %d"),err);
        }
        iLog->SetLogFile(iCurrentSettings->FileLog() ? &KLogFileName() : NULL);
        iLog->SetFontSize(iCurrentSettings->FontSize());
    }
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
