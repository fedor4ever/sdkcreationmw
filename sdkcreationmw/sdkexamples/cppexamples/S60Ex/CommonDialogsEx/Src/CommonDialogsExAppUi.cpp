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
* Description:  Implements CommonDialogsEx appui.
*
*/

#include <avkon.hrh>
#include <CAknMemorySelectionSettingPage.h>
#include <CAknFileSelectionDialog.h>
#include <CAknFileNamePromptDialog.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
#include <CommonDialogsEx.rsg>
#include "CommonDialogsExAppUi.h"
#include "CommonDialogsExContainer.h"
#include "CommonDialogsEx.hrh"
#include "CommonDialogsExSettingsDialog.h"


_LIT( KCommonDialogsExSelected, "User selected" );
_LIT( KCommonDialogsExCanceled, "User canceled" );
_LIT( KCommonDialogsExSpace, " " );
_LIT( KCommonDialogsExMultiDrive, "Multi drive: " );
_LIT( KCommonDialogsExPhoneMemoryName, "Phone memory" );
_LIT( KCommonDialogsExMemoryCardName,  "Memory card" );
_LIT( KCommonDialogsExDefaultSetting,  "Default setting" );
_LIT( KCommonDialogsExMultiSetting,  "Multi drive setting" );
_LIT( KCommonDialogsExFile,  " A file " );
_LIT( KCommonDialogsExFileName,  " Filename is OK " );
_LIT( KRootPath,                    "C:\\data\\Images\\" );
_LIT( KDefaultFileName,             "DynamicFilename.jpg" );

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CCommonDialogsExAppUi::ConstructL()
// Symbian 2nd phase constructor.
// ----------------------------------------------------------
//
void CCommonDialogsExAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin);
    iAppContainer = new (ELeave) CCommonDialogsExContainer;
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CCommonDialogsExAppUi::~CCommonDialogsExAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CCommonDialogsExAppUi::~CCommonDialogsExAppUi()
    {
    RemoveFromStack( iAppContainer );
    delete iAppContainer;
    }

// ------------------------------------------------------------------------------
// CCommonDialogsExAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CCommonDialogsExAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CCommonDialogsExAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// From CEikAppUi, handles key events.
// ----------------------------------------------------
//
TKeyResponse CCommonDialogsExAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CCommonDialogsExAppUi::HandleCommandL(TInt aCommand)
// Handles the commands.
// ----------------------------------------------------
//
void CCommonDialogsExAppUi::HandleCommandL(TInt aCommand)
    {
    TFileName rootFileName( KRootPath );
    TFileName emptyFileName( KNullDesC );
    TFileName defaultFileName( KDefaultFileName );
    TFileName tempRootPath;
    TFileName tempDefaultFolder;
    TInt returnValue;
    
    TBuf<32> textMain;
    TBuf<64> textSub;
            
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        //Memory selection (default)
        case ECommonDialogsExCmd101:
            {
            CAknMemorySelectionDialog* dlg = CAknMemorySelectionDialog::NewL( 
                                                        ECFDDialogTypeNormal, 
                                                        ETrue );
            CleanupStack::PushL( dlg );
            returnValue = dlg->ExecuteL( iMemory );
            if( returnValue )
                {
                textMain.Append( KCommonDialogsExSelected );
                if ( iMemory == CAknMemorySelectionSettingPage::EPhoneMemory )
                    {
                    textSub.Append( KCommonDialogsExPhoneMemoryName );
                    }
                else
                    {
                    textSub.Append( KCommonDialogsExMemoryCardName );
                    }
                }
            else
                {
                textMain.Append( KCommonDialogsExCanceled );
                textSub.Append( KCommonDialogsExSpace );
                }
            //set display text
            iAppContainer->ShowTextL( textMain, textSub );
            CleanupStack::PopAndDestroy();
            break;
            }
            
        //Memory selection (Multi drive)
        case ECommonDialogsExCmd102:
            {
            CAknMemorySelectionDialogMultiDrive* dlg =
                CAknMemorySelectionDialogMultiDrive::NewL(
                        ECFDDialogTypeNormal, ETrue );
            CleanupStack::PushL( dlg );
            returnValue = dlg->ExecuteL( iDrive , &tempRootPath, NULL );
            
            if( returnValue )
                {
                textMain.Append( KCommonDialogsExSelected );
                textSub.Append( KCommonDialogsExMultiDrive );
                textSub.Append( tempRootPath );
                }
            else
                {
                textMain.Append( KCommonDialogsExCanceled );
                textSub.Append( KCommonDialogsExSpace );
                }
                
            iAppContainer->ShowTextL( textMain, textSub );
            CleanupStack::PopAndDestroy();

            break;
            }
            
        //Memory setting (default)           
        case ECommonDialogsExCmd201:
            {
            CCommonDialogsExSettingsDialog* dlg = 
                          CCommonDialogsExSettingsDialog::NewL(0, EFalse);
            dlg->ExecuteLD( R_COMMONDIALOGSEX_SETTINGS_DIALOG );
            textMain.Append( KCommonDialogsExSelected );
            textSub.Append( KCommonDialogsExDefaultSetting );
            iAppContainer->ShowTextL( textMain, textSub );
            break;
            }
            
        //Memory setting (Multi drive)   
        case ECommonDialogsExCmd202:
            {
            CCommonDialogsExSettingsDialog* dlg =
                CCommonDialogsExSettingsDialog::NewL(0, ETrue);
            dlg->ExecuteLD( R_COMMONDIALOGSEX_SETTINGS_DIALOG );
            textMain.Append( KCommonDialogsExSelected );
            textSub.Append( KCommonDialogsExMultiSetting );
            iAppContainer->ShowTextL( textMain, textSub );
            break;
            }
            
        //File selection            
        case ECommonDialogsExCmd301:
            {
            CAknFileSelectionDialog* dlg = 
                       CAknFileSelectionDialog::NewL( ECFDDialogTypeSelect );
            CleanupStack::PushL( dlg );
            returnValue = dlg->ExecuteL( rootFileName );
            if( returnValue )
                {
                textMain.Append( KCommonDialogsExSelected );
                textSub.Append( KCommonDialogsExFile );
                }
            else
                {
                textMain.Append( KCommonDialogsExCanceled );
                textSub.Append( KCommonDialogsExSpace );
                }
            iAppContainer->ShowTextL( textMain, textSub );
            CleanupStack::PopAndDestroy();
            break;
            }

        // Filename prompt
        case ECommonDialogsExCmd401:
            {
            returnValue = CAknFileNamePromptDialog::RunDlgLD( 
                                  defaultFileName, 
                                  KRootPath );
            if( returnValue )
                {
                textMain.Append( KCommonDialogsExSelected );
                textSub.Append( KCommonDialogsExFileName );
                }
            else
                {
                textMain.Append( KCommonDialogsExCanceled );
                textSub.Append( KCommonDialogsExSpace );
                }
            iAppContainer->ShowTextL( textMain, textSub );
            break;
            }
            
        default:
            {
            break;
            }
        }
    }
