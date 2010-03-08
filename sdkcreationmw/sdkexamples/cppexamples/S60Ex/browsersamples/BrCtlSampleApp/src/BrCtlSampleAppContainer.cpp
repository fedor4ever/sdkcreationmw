/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the container control for Browser Control Sample application.
*
*/


// INCLUDE FILES
#include "BrCtlSampleAppContainer.h"
#include "BrCtlSampleAppSpecialLoadObserver.h"
#include "BrCtlSampleAppLayoutObserver.h"
#include "BrCtlSampleAppSoftkeysObserver.h"
#include "BrCtlSampleAppLoadEventObserver.h"
#include "BrCtlSampleAppLinkResolver.h"
#include "BrCtlSampleAppStateChangeObserver.h"
#include "BrCtlSampleAppDialogsProvider.h"
#include "BrCtlSampleAppQueryDialog.h"
#include <eiklabel.h>  // for example label control
#include <avkon.hrh>
#include <BrCtlSampleApp.rsg>
#include "brctlsampleapp.hrh"
#include <BrCtlInterface.h>
#include <CHARCONV.H>
#include <gdi.h>
#include <bitdev.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBrCtlSampleAppContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CBrCtlSampleAppContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    SetRect(aRect);
    ActivateL();

    iCommandBase = TBrCtlDefs::ECommandIdBase;

    // Create the Observers
    iBrCtlSampleAppSpecialLoadObserver = CBrCtlSampleAppSpecialLoadObserver::NewL();
    iBrCtlSampleAppLayoutObserver = CBrCtlSampleAppLayoutObserver::NewL(this);
    iBrCtlSampleAppSoftkeysObserver = CBrCtlSampleAppSoftkeysObserver::NewL(this);
    iBrCtlSampleAppLoadEventObserver = CBrCtlSampleAppLoadEventObserver::NewL(this);
    iBrCtlSampleAppLinkResolver = CBrCtlSampleAppLinkResolver::NewL();
    iBrCtlSampleAppStateChangeObserver = CBrCtlSampleAppStateChangeObserver::NewL(this);
    iBrCtlSampleAppDialogsProvider = CBrCtlSampleAppDialogsProvider::NewL(this);

    // Initialize memeber variables
    _LIT(KEmpty, "");
    iText.Set(KEmpty);
    iPoint = TPoint(0,130);
    iBrCtlCapabilities = TBrCtlDefs::ECapabilityDisplayScrollBar | TBrCtlDefs::ECapabilityLoadHttpFw |
                         TBrCtlDefs::ECapabilityGraphicalHistory | TBrCtlDefs::ECapabilityGraphicalPage;

    // Create a Screen device for use with fonts for writing text to the screen
    CEikonEnv* eikEnv = CEikonEnv::Static();
    CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();
    _LIT( KDummy, "Dummy");
    iFbsScrDev = CFbsScreenDevice::NewL( KDummy, screenDev.DisplayMode());
    
    // Create a font spec in order to retrieve a valid font
    _LIT(KFontFamily, "S60 Sans");
    TFontSpec fs(KFontFamily, 100);
    iFbsScrDev->GetNearestFontInTwips((CFont*&)iFont, fs);

    TRect rect(Position(), Size());
    iBrCtlInterface = CreateBrowserControlL( this, 
        rect, 
        iBrCtlCapabilities,
        iCommandBase, 
        iBrCtlSampleAppSoftkeysObserver, 
        iBrCtlSampleAppLinkResolver, 
        iBrCtlSampleAppSpecialLoadObserver,
        iBrCtlSampleAppLayoutObserver,
        iBrCtlSampleAppDialogsProvider);

    // These observers can be added and removed dynamically
    iBrCtlInterface->AddLoadEventObserverL(iBrCtlSampleAppLoadEventObserver);
    iBrCtlInterface->AddStateChangeObserverL(iBrCtlSampleAppStateChangeObserver);
    }

// Destructor
CBrCtlSampleAppContainer::~CBrCtlSampleAppContainer()
    {
    if (iBrCtlInterface)
        {
        iBrCtlInterface->RemoveLoadEventObserver(iBrCtlSampleAppLoadEventObserver);
        iBrCtlInterface->RemoveStateChangeObserver(iBrCtlSampleAppStateChangeObserver);
        delete iBrCtlInterface;
        }

    // we are required to release the font
    iFbsScrDev->ReleaseFont(iFont);

    // delete the screen device
    delete iFbsScrDev;

    // delete the observers
    delete iBrCtlSampleAppSpecialLoadObserver;
    delete iBrCtlSampleAppLayoutObserver;
    delete iBrCtlSampleAppSoftkeysObserver;
    delete iBrCtlSampleAppLoadEventObserver;
    delete iBrCtlSampleAppLinkResolver;
    delete iBrCtlSampleAppStateChangeObserver;
    delete iBrCtlSampleAppDialogsProvider;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CBrCtlSampleAppContainer::SizeChanged()
    {
    if (iBrCtlInterface)
        {
        iBrCtlInterface->SetRect(Rect());
        }
    }

// ---------------------------------------------------------
// CBrCtlSampleAppContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CBrCtlSampleAppContainer::CountComponentControls() const
    {
    if (iBrCtlInterface)
        return 1;
    return 0;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CBrCtlSampleAppContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iBrCtlInterface; // Could be NULL
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CBrCtlSampleAppContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CBrCtlSampleAppContainer::Draw(const TRect& aRect) const
    {
    
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    gc.SetBrushColor(KRgbGray);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    
    gc.UseFont(iFont);

    gc.DrawText(iText, iPoint);      

    gc.DiscardFont();
    }

// ---------------------------------------------------------
// CBrCtlSampleAppContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CBrCtlSampleAppContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    // TODO: Add your control event handler code here
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ----------------------------------------------------
//
TKeyResponse CBrCtlSampleAppContainer::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::SetText(const TDesC& aText)
// ----------------------------------------------------
//
void
CBrCtlSampleAppContainer::SetText(const TDesC& aText)
    {
    iText.Set(aText);
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::SetPoint(const TPoint& aPoint)
// ----------------------------------------------------
//
void
CBrCtlSampleAppContainer::SetPoint(const TPoint& aPoint)
    {
    iPoint = aPoint;
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::CancelLoad()
// ----------------------------------------------------
//
void CBrCtlSampleAppContainer::CancelLoad()
    {
    iCancelInitDataLoad = ETrue;
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::HandleCommandL(TInt aCommand)
// ----------------------------------------------------
//
void CBrCtlSampleAppContainer::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EBrCtlSampleAppCmdAppRestoreSetting:
            {
            if (iBrCtlInterface)
                {
                TRect rect( Position(), Size() );
                iBrCtlInterface->SetRect( rect );
                iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsSmallScreen, 1);
                iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsSecurityWarnings, 1);
                iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsAutoLoadImages, 1);
                iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsECMAScriptEnabled, 1);
                }
            break;
            }
        case EBrCtlSampleAppCmdAppReload:
            {
            if (iBrCtlInterface)
                {
                iBrCtlInterface->HandleCommandL(iCommandBase + TBrCtlDefs::ECommandReload);
                }
            break;
            }
        case EBrCtlSampleAppCmdAppBack:
            {
            if (iBrCtlInterface)
                {
                iBrCtlInterface->HandleCommandL(iCommandBase + TBrCtlDefs::ECommandBack);
                }
            break;
            }
        case EBrCtlSampleAppCmdAppForward:
            {
            if (iBrCtlInterface)
                {
                iBrCtlInterface->HandleCommandL(iCommandBase + TBrCtlDefs::ECommandForward);
                }
            break;
            }
        case EBrCtlSampleAppCmdApp1:
            {
            BasicBrowserControlL();
            break;
            }
        case EBrCtlSampleAppCmdApp2:
            {
            BrowserControlWithObserversL();
            break;
            }
        case EBrCtlSampleAppCmdApp3:
            {
            LoadingContentWithFileHandleL();
            break;
            }
        case EBrCtlSampleAppCmdApp4:
            {
            LoadingContentWithBufferL();
            break;
            }
        case EBrCtlSampleAppCmdApp5:
            {
            IncrementalLoadingL();
            break;
            }
        case EBrCtlSampleAppCmdApp6:
            {
            SpecialLoadRequestsL();
            break;
            }
        case EBrCtlSampleAppCmdApp7:
            {
            CustomizedDialogsL();
            break;
            }
        case EBrCtlSampleAppCmdApp8:
            {
            CustomizedSoftKeysL();
            break;
            }
        case EBrCtlSampleAppCmdApp9:
            {
            ResolvingEmbeddedLinksL();
            break;
            }
        case EBrCtlSampleAppCmdApp10:
            {
            CustomizedScrollBarsL();
            break;
            }
        case EBrCtlSampleAppCmdApp11:
            {
            HandleStateChangedL();
            break;
            }
        case EBrCtlSampleAppCmdApp12:
            {
            ChangeSizeExtentsL();
            break;
            }
        case EBrCtlSampleAppCmdApp13:
            {
            PageInfoL();
            break;
            }
        case EBrCtlSampleAppCmdApp14:
            {
            ContentSizeImageCountL();
            break;
            }

        default:
            if (aCommand >= iCommandBase &&
                aCommand < iCommandBase + TBrCtlDefs::ECommandIdWMLBase &&
                iBrCtlInterface)
                {
                iBrCtlInterface->HandleCommandL(aCommand);
                }

            if (aCommand >= iCommandBase + TBrCtlDefs::ECommandIdWMLBase &&
                aCommand < iCommandBase + TBrCtlDefs::ECommandIdRange &&
                iBrCtlInterface)
                {
                iBrCtlInterface->HandleCommandL(aCommand);
                }              
            break;      
        }
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::OfferKeyEventL(const 
//               TKeyEvent& aKeyEvent,TEventCode aType)
// ----------------------------------------------------
//
TKeyResponse 
CBrCtlSampleAppContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if (iBrCtlInterface)
        {
        return iBrCtlInterface->OfferKeyEventL(aKeyEvent, aType);
        }
    return EKeyWasConsumed;
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::DynInitMenuPaneL(
//           TInt aResourceId, CEikMenuPane* aMenuPane)
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    if (iBrCtlInterface && aResourceId == R_BRCTLSAMPLEAPP_MENU )
        {
        iBrCtlInterface->AddOptionMenuItemsL(*aMenuPane, aResourceId);
        RPointerArray<TBrCtlWmlServiceOption>* options;
        options = iBrCtlInterface->WMLOptionMenuItemsL();
        TInt i;
        TInt count = options->Count();
        for (i = 0; i < count; i++)
            {
            TBrCtlWmlServiceOption* option = (*options)[i];
            if (option != NULL)
                {
                CEikMenuPaneItem::SData item;
                item.iText.Copy(option->Text());
                item.iCommandId = option->ElemID();
                item.iFlags = 0;
                item.iCascadeId = 0;
                aMenuPane->InsertMenuItemL(item, 0);
                }
            }
        }
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::CreateBasicBrowserControlL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::CreateBasicBrowserControlL()
    {
    if (iBrCtlInterface != NULL)
        {
        delete iBrCtlInterface;
        iBrCtlInterface = NULL;
        }
    else
        {
        TRect rect(Position(), Size());
        
        iBrCtlInterface = CreateBrowserControlL( this, 
            rect, 
            TBrCtlDefs::ECapabilityDisplayScrollBar | TBrCtlDefs::ECapabilityLoadHttpFw |
            TBrCtlDefs::ECapabilityGraphicalHistory | TBrCtlDefs::ECapabilityGraphicalPage,
            TBrCtlDefs::ECommandIdBase, 
            NULL, 
            NULL,
            NULL, 
            NULL);
        }
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::CreateBrowserControlWithObserversL(TUint aCapabilities)
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::CreateBrowserControlWithObserversL(TUint aCapabilities)
    {
    if (iBrCtlCapabilities != aCapabilities)
        {
        delete iBrCtlInterface;
        iBrCtlInterface = NULL;
        }
    iBrCtlCapabilities = aCapabilities;
    if (iBrCtlInterface == NULL)
        {
        TRect rect(Position(), Size());
        iBrCtlInterface = CreateBrowserControlL( this, 
            rect, 
            iBrCtlCapabilities,
            TBrCtlDefs::ECommandIdBase, 
            iBrCtlSampleAppSoftkeysObserver, 
            iBrCtlSampleAppLinkResolver, 
            iBrCtlSampleAppSpecialLoadObserver,
            iBrCtlSampleAppLayoutObserver);
    
        iBrCtlInterface->AddLoadEventObserverL(iBrCtlSampleAppLoadEventObserver);
        iBrCtlInterface->AddStateChangeObserverL(iBrCtlSampleAppStateChangeObserver);
        }
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::ReadFileLC(const TDesC& aFileName)
// ----------------------------------------------------
//
HBufC8* 
CBrCtlSampleAppContainer::ReadFileLC(const TDesC& aFileName)
    {
    RFs rfs;
    RFile file;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    User::LeaveIfError(file.Open(rfs, aFileName, EFileRead));
    CleanupClosePushL(file);
    TInt size;
    User::LeaveIfError(file.Size(size));
    HBufC8* buf = HBufC8::NewLC(size);
    TPtr8 bufPtr(buf->Des());
    User::LeaveIfError(file.Read(bufPtr));
    CleanupStack::Pop(); // buf
    CleanupStack::PopAndDestroy(2); // file, rfs
    CleanupStack::PushL(buf);
    return buf;
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::BasicBrowserControlL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::BasicBrowserControlL()
    {
    if (iBrCtlInterface == NULL)
        {
        TRect rect(Position(), Size());
        iBrCtlInterface = CreateBrowserControlL( this, 
            rect, 
            TBrCtlDefs::ECapabilityDisplayScrollBar | TBrCtlDefs::ECapabilityLoadHttpFw |
            TBrCtlDefs::ECapabilityGraphicalHistory | TBrCtlDefs::ECapabilityGraphicalPage,
            iCommandBase, 
            NULL, 
            NULL,
            NULL, 
            NULL,
            NULL);
        }

    _LIT(KUrl, "file:///c:\\BrCtlSampleApp\\sample1.htm");

    iBrCtlInterface->LoadUrlL( KUrl );
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::BrowserControlWithObserversL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::BrowserControlWithObserversL()
    {
    CreateBrowserControlWithObserversL( TBrCtlDefs::ECapabilityDisplayScrollBar | 
                                        TBrCtlDefs::ECapabilityLoadHttpFw |
                                        TBrCtlDefs::ECapabilityGraphicalHistory |
                                        TBrCtlDefs::ECapabilityGraphicalPage);
    _LIT(KUrl, "file:///c:\\BrCtlSampleApp\\sample2.htm");

    iBrCtlInterface->LoadUrlL( KUrl );
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::LoadingContentWithFileHandleL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::LoadingContentWithFileHandleL()
    {
    CreateBrowserControlWithObserversL(TBrCtlDefs::ECapabilityDisplayScrollBar |
        							   TBrCtlDefs::ECapabilityLoadHttpFw | 
        							   TBrCtlDefs::ECapabilityGraphicalHistory |
       								   TBrCtlDefs::ECapabilityGraphicalPage);
    _LIT(KUrl, "c:\\BrCtlSampleApp\\sample3.htm");
    RFs rfs;
    RFile file;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    User::LeaveIfError(file.Open(rfs, KUrl(), EFileShareReadersOnly));
    CleanupClosePushL(file);
    iBrCtlInterface->LoadFileL(file);
    CleanupStack::PopAndDestroy(2); // file, rfs
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::LoadingContentWithBufferL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::LoadingContentWithBufferL()
    {
    CreateBrowserControlWithObserversL(TBrCtlDefs::ECapabilityDisplayScrollBar |
                                       TBrCtlDefs::ECapabilityLoadHttpFw |
    								   TBrCtlDefs::ECapabilityGraphicalHistory |
    								   TBrCtlDefs::ECapabilityGraphicalPage );
    _LIT(KUrl, "c:\\BrCtlSampleApp\\sample4.htm");
    _LIT8(KDataType, "text/html");

    HBufC8* data = ReadFileLC(KUrl);
    TDataType dataType(KDataType());
    TUid uid;
    uid.iUid = KCharacterSetIdentifierIso88591;
    iBrCtlInterface->LoadDataL(KUrl, *data, dataType, uid);
    CleanupStack::PopAndDestroy(); // data
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::IncrementalLoadingL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::IncrementalLoadingL()
    {
    CreateBrowserControlWithObserversL(TBrCtlDefs::ECapabilityDisplayScrollBar | 
                                       TBrCtlDefs::ECapabilityLoadHttpFw |
    								   TBrCtlDefs::ECapabilityGraphicalHistory |
    								   TBrCtlDefs::ECapabilityGraphicalPage );
    _LIT(KUrl, "c:\\BrCtlSampleApp\\sample5.htm");
    HBufC8* data = ReadFileLC(KUrl);
    _LIT8(KDataType, "text/html");
    TDataType dataType(KDataType());
    MBrCtlDataLoadConsumer* brCtlDataLoadConsumer = NULL;
    TUid uid;
    uid.iUid = KCharacterSetIdentifierIso88591;
    iBrCtlInterface->InitLoadDataL(KUrl, dataType, uid, data->Length(),
        this, &brCtlDataLoadConsumer);

    if (brCtlDataLoadConsumer)
        {
        brCtlDataLoadConsumer->HandleNextDataChunk(*data);
        if (!iCancelInitDataLoad)
            {
            brCtlDataLoadConsumer->HandleLoadComplete();
            }
        }
    iCancelInitDataLoad = EFalse;
    CleanupStack::PopAndDestroy(); // data
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::SpecialLoadRequestsL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::SpecialLoadRequestsL()
    {
    CreateBrowserControlWithObserversL(TBrCtlDefs::ECapabilityDisplayScrollBar |
        							   TBrCtlDefs::ECapabilityClientNotifyURL | 
        							   TBrCtlDefs::ECapabilityGraphicalHistory |
        							   TBrCtlDefs::ECapabilityGraphicalPage );
    
    _LIT(KUrl, "file:///c:\\BrCtlSampleApp\\sample6.htm");

    iBrCtlInterface->LoadUrlL( KUrl );
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::CustomizedDialogsL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::CustomizedDialogsL()
    {
    CreateBrowserControlWithObserversL(TBrCtlDefs::ECapabilityDisplayScrollBar |
        							   TBrCtlDefs::ECapabilityLoadHttpFw | 
        							   TBrCtlDefs::ECapabilityGraphicalHistory |
        							   TBrCtlDefs::ECapabilityGraphicalPage);
    _LIT(KUrl, "file:///c:\\BrCtlSampleApp\\sample7.htm");

    iBrCtlInterface->LoadUrlL( KUrl );
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::CustomizedSoftKeysL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::CustomizedSoftKeysL()
    {
    CreateBrowserControlWithObserversL(TBrCtlDefs::ECapabilityDisplayScrollBar |
        							   TBrCtlDefs::ECapabilityLoadHttpFw | 
        							   TBrCtlDefs::ECapabilityGraphicalHistory |
        							   TBrCtlDefs::ECapabilityGraphicalPage);
    _LIT(KUrl, "file:///c:\\BrCtlSampleApp\\sample8.htm");

    iBrCtlInterface->LoadUrlL( KUrl );
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::ResolvingEmbeddedLinksL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::ResolvingEmbeddedLinksL()
    {
    CreateBrowserControlWithObserversL(TBrCtlDefs::ECapabilityDisplayScrollBar |
        							   TBrCtlDefs::ECapabilityClientResolveEmbeddedURL |
        							   TBrCtlDefs::ECapabilityGraphicalHistory |
        							   TBrCtlDefs::ECapabilityGraphicalPage);
    _LIT(KUrl, "file:///c:\\BrCtlSampleApp\\sample9.htm");

    iBrCtlInterface->LoadUrlL( KUrl );
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::CustomizedScrollBarsL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::CustomizedScrollBarsL()
    {
    CreateBrowserControlWithObserversL(TBrCtlDefs::ECapabilityDisplayScrollBar | 
                                       TBrCtlDefs::ECapabilityLoadHttpFw |
                                       TBrCtlDefs::ECapabilityGraphicalHistory |
                                       TBrCtlDefs::ECapabilityGraphicalPage );
    _LIT(KUrl, "file:///c:\\BrCtlSampleApp\\sample10.htm");

    iBrCtlInterface->SetExtent(Position(), TSize(300, 180));

    iBrCtlInterface->SetBrowserSettingL(TBrCtlDefs::ESettingsSmallScreen, 0);

    iBrCtlInterface->LoadUrlL( KUrl );
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::HandleStateChangedL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::HandleStateChangedL()
    {
    CreateBrowserControlWithObserversL(TBrCtlDefs::ECapabilityDisplayScrollBar |
                                       TBrCtlDefs::ECapabilityLoadHttpFw |  
    								   TBrCtlDefs::ECapabilityGraphicalHistory |
    								   TBrCtlDefs::ECapabilityGraphicalPage);
    _LIT(KUrl, "file:///c:\\BrCtlSampleApp\\sample11.htm");

    iBrCtlInterface->SetExtent(Position(), TSize(176, 72));

    iBrCtlInterface->LoadUrlL( KUrl );
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::ChangeSizeExtentsL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::ChangeSizeExtentsL()
    {
    if (iBrCtlInterface)
        {
        _LIT(KTitle,"Query Dialog");
        _LIT(KPromptWidth, "Enter new width");
        _LIT(KPromptHeight, "Enter new height");
        HBufC* returnedInput = NULL;
        TUint width = iBrCtlInterface->Rect().Width();
        TUint height = iBrCtlInterface->Rect().Height();
        TBool ret;
	    // defInput is not modified by the dialog.
        TBuf<10>defInput;
        defInput.Num(width);

        CBrCtlSampleAppQueryDialog* dialog = new (ELeave) CBrCtlSampleAppQueryDialog( defInput, returnedInput );
        CleanupStack::PushL(dialog);

        dialog->PrepareLC( R_DATA_QUERY_WITH_HEADING );
	    dialog->SetHeaderTextL( KTitle );
        dialog->SetPromptL( KPromptWidth );
        CleanupStack::Pop(); // dialog
        ret = dialog->RunLD();
        if (ret)
            {
            TInt i = 0;
            for (width = 0; i < returnedInput->Length(); i++)
                {
                if (((*returnedInput)[i] > '9') || ((*returnedInput)[i] < '0'))
                    break;
                width *= 10;
                width += ((*returnedInput)[i] - '0');
                }

            defInput.Num(height);

            dialog = new (ELeave) CBrCtlSampleAppQueryDialog( defInput, returnedInput );
            CleanupStack::PushL(dialog);
            dialog->PrepareLC( R_DATA_QUERY_WITH_HEADING );
	        dialog->SetHeaderTextL( KTitle );
            dialog->SetPromptL( KPromptHeight );
            CleanupStack::Pop(); // dialog
            ret = dialog->RunLD();
            i = 0;
            for (height = 0; i < returnedInput->Length(); i++)
                {
                if ((*returnedInput)[i] > '9' || (*returnedInput)[i] < '0')
                    break;
                height *= 10;
                height += ((*returnedInput)[i] - '0');
                }
            if (width && height)
                {
                iBrCtlInterface->SetExtent(Position(), TSize(width, height));
                }
            }
        }
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::PageInfoL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::PageInfoL()
    {
    if (iBrCtlInterface)
        {
        HBufC* title = iBrCtlInterface->PageInfoLC(TBrCtlDefs::EPageInfoTitle);
        if (title)
            {
            CAknQueryDialog* dialog = new (ELeave) CAknQueryDialog();
            dialog->PrepareLC( R_CONFIRM_QUERY );
            dialog->SetPromptL( *title );
            dialog->RunLD();
            CleanupStack::PopAndDestroy();
            }
        HBufC* url = iBrCtlInterface->PageInfoLC(TBrCtlDefs::EPageInfoUrl);
        if (url)
            {
            CAknQueryDialog* dialog = new (ELeave) CAknQueryDialog();
            dialog->PrepareLC( R_CONFIRM_QUERY );
            dialog->SetPromptL( *url );
            dialog->RunLD();
            CleanupStack::PopAndDestroy();
            }
        }
    }

// ----------------------------------------------------
// CBrCtlSampleAppContainer::ContentSizeImageCountL()
// ----------------------------------------------------
//
void 
CBrCtlSampleAppContainer::ContentSizeImageCountL()
    {
    if (iBrCtlInterface)
        {
        TSize size(0, 0);
        size = iBrCtlInterface->ContentSize();
        TInt images = iBrCtlInterface->ImageCountL();
        _LIT(KStr, "Width = %d, Height = %d. %d images");

        TBuf<100> str;
        str.Format(KStr, size.iWidth, size.iHeight, images);
        CAknQueryDialog* dialog = new (ELeave) CAknQueryDialog();
        dialog->PrepareLC( R_CONFIRM_QUERY );
        dialog->SetPromptL( str );
        dialog->RunLD();
        }
    }

// End of File  

