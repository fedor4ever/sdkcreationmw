/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include <eikmenup.h>
#include <aknnotewrappers.h> 
#include <aknuniteditor.h>
#include <e32math.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <UEEx.rsg>

#include "ueex.hrh"
#include "ueexform.h"

// CONSTANTS
_LIT ( KCustomUnit, "XYZ" );

const TInt KMaxFractionalDigits = 8;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUEExForm::ProcessCommandL()
// Handles commands from menu.
// -----------------------------------------------------------------------------
//
void CUEExForm::ProcessCommandL( TInt aCommandId )
    {
    CAknUnitEditor* unitEditor =
        static_cast<CAknUnitEditor*>( ControlOrNull( EUEExUnitEditor ) );
    
    if ( unitEditor )
        {
        switch ( aCommandId )
            {
            case ECmdSetValue:
                DoSetValue( unitEditor );
            break;
            case ECmdSetNan:
                DoSetNaN( unitEditor );
            break;
            case ECmdSetMinMax:
                DoSetMinMax( unitEditor );
            break;
            case ECmdSetUnit:
                DoSetUnit( unitEditor );
            break;
            case ECmdSetCustomUnit:
                DoSetCustomUnit( unitEditor );
            break;
            case ECmdSetMaxFractionalDigits:
                DoSetMaxFractionalDigits( unitEditor );
            break;
            case ECmdGetValue:
                DoGetValue( unitEditor );
            break;
            default:
            break;
            }
        }

    // Exit form gracefully
    if ( aCommandId == EAknCmdExit )
        {
        TryExitL( EAknSoftkeyBack );
        }
    else
        {
        CAknForm::ProcessCommandL( aCommandId );
        }
    }
    
// -----------------------------------------------------------------------------
// CUEExForm::DynInitMenuPaneL()
// Initializes the menu pane when it is opened.
// -----------------------------------------------------------------------------
//
void CUEExForm::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane *aMenuPane )
    {
    CAknForm::DynInitMenuPaneL( aResourceId, aMenuPane );
    
    if ( aResourceId == R_AVKON_FORM_MENUPANE )
        {
        // Disable the Label, Add, Delete, and Save Field
        aMenuPane->SetItemDimmed( EAknFormCmdLabel, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdAdd, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdDelete, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdSave, ETrue );
        }
    }    

// -----------------------------------------------------------------------------
// CUEExForm::QuerySaveChangesL()
// -----------------------------------------------------------------------------
//
TBool CUEExForm::QuerySaveChangesL()
    {
    return EFalse;
    }

    
// -----------------------------------------------------------------------------
// CUEExForm::DoSetValue()
// -----------------------------------------------------------------------------
//
void CUEExForm::DoSetValue( CAknUnitEditor* aUnitEditor )
    {
    TInt number( 0 );
    CAknNumberQueryDialog* dlg = CAknNumberQueryDialog::NewL( number );
    dlg->PrepareLC( R_AVKON_DIALOG_QUERY_VALUE_NUMBER );
    dlg->SetMinimumAndMaximum( KMinTInt, KMaxTInt );
    if ( dlg->RunLD() )
        {
        aUnitEditor->SetValue( number );
        }    
    }
    
// -----------------------------------------------------------------------------
// CUEExForm::DoSetNaN()
// -----------------------------------------------------------------------------
//
void CUEExForm::DoSetNaN( CAknUnitEditor* aUnitEditor )
    {
    TRealX nan;
    nan.SetNaN();
    aUnitEditor->SetValue( nan );    
    }
    
// -----------------------------------------------------------------------------
// CUEExForm::DoSetMinMax()
// -----------------------------------------------------------------------------
//
void CUEExForm::DoSetMinMax( CAknUnitEditor* aUnitEditor )
    {
    TInt newMin( 0 );
    TInt newMax( 0 );
    TInt ret;
    
    CAknNumberQueryDialog* dlg = CAknNumberQueryDialog::NewL( newMin );
    dlg->PrepareLC( R_NEWMINQUERY );
    dlg->SetMinimumAndMaximum( KMinTInt, KMaxTInt );
    ret = dlg->RunLD();
    
    if ( ret == EAknSoftkeyOk )
        {
        dlg = CAknNumberQueryDialog::NewL( newMax );
        dlg->PrepareLC( R_NEWMAXQUERY );
        dlg->SetMinimumAndMaximum( KMinTInt, KMaxTInt );
        ret = dlg->RunLD();
        
        if ( ret == EAknSoftkeyOk )
            {
            aUnitEditor->SetMinimumAndMaximum( newMin, newMax ); 
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CUEExForm::DoSetUnit()
// -----------------------------------------------------------------------------
//
void CUEExForm::DoSetUnit( CAknUnitEditor* aUnitEditor )
    {
    TRAP_IGNORE( aUnitEditor->SetUnitL( EAknUnitEditorMetricSpeed ) );
    }
    
// -----------------------------------------------------------------------------
// CUEExForm::DoSetCustomUnit()
// -----------------------------------------------------------------------------
//
void CUEExForm::DoSetCustomUnit( CAknUnitEditor* aUnitEditor )
    {
    TRAP_IGNORE( aUnitEditor->SetUnitL( KCustomUnit ) );    
    }
    
// -----------------------------------------------------------------------------
// CUEExForm::DoSetMaxFractionalDigits()
// -----------------------------------------------------------------------------
//
void CUEExForm::DoSetMaxFractionalDigits( CAknUnitEditor* aUnitEditor )
    {
    TInt number( 0 );
    CAknNumberQueryDialog* dlg = CAknNumberQueryDialog::NewL( number );
    dlg->PrepareLC( R_AVKON_DIALOG_QUERY_VALUE_NUMBER );
    dlg->SetMinimumAndMaximum( 0, KMaxFractionalDigits );
    if ( dlg->RunLD() )
        {
        aUnitEditor->SetMaxFractionalDigits( number );
        }
    }
    
// -----------------------------------------------------------------------------
// CUEExForm::DoGetValue()
// -----------------------------------------------------------------------------
//
void CUEExForm::DoGetValue( CAknUnitEditor* aUnitEditor )
    {
    TReal realValue = aUnitEditor->Value();
    TBuf<32> buf;
    TRealFormat format( 10, aUnitEditor->MaxFractionalDigits() );
    buf.Num( realValue, format );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
    note->ExecuteLD( buf );    
    }
