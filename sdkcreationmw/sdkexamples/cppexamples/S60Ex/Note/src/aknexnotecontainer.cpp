/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eiklabel.h>
#include <e32math.h>
#include <aknglobalnote.h>
#include <aknwaitdialog.h>
#include <aknexnote.rsg>
#include <eikprogi.h>
#include <aknnotecontrol.h>

#include "AknExNoteContainer.h"
#include "AknExNoteView.h"
#include "AknExNote.hrh"

// CONSTANTS
#include "Aknexnoteconstants.h"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CMultiViewsView2::CMultiViewsView2()
// C++ default constructor can NOT contain any code, that might leave. 
// -----------------------------------------------------------------------------
//
CAknExNoteContainer::CAknExNoteContainer( CAknExNoteView* aView )
: iLabel( NULL ), iView( aView )
  
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CAknExNoteContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::ConstructL( const TRect& aRect )
    {
    // Creates window.
    CreateWindowL();

    // Creates label object.
    CreateLabelL();

     // Sets rectangle of frame.
    SetRect( aRect );

    // Activates the window. (Ready to draw)
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::~CAknExNoteContainer()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExNoteContainer::~CAknExNoteContainer()
    {
    delete iLabel;
    delete iIdle;
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::SetTextToLabelL()
// Sets text to Label
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::SetTextToLabelL( TInt aResourceId )
    {
    //Allocate TBuf with constant length.
    TBuf<KAknExNoteMessageBufLength> messageString( NULL );

    // Reads a resource into a descriptor.
    iCoeEnv->ReadResource( messageString, aResourceId );
    
    // Set label's text with descriptor
    iLabel->SetTextL( messageString );
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::ShowGeneralNoteL()
// Show General Note
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::ShowGeneralNoteL( TInt aResourceId, 
                                            const CAknNoteDialog::
                                            TTimeout aTimeout, 
                                            const CAknNoteDialog::TTone aTone )
    
    {
    // Create CAknNoteDialog instance
    CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog( aTone, aTimeout );
    
    // Show the Dialog
    dlg->ExecuteLD( aResourceId );
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::ShowGeneralNoteL()
// Show General Note
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::ShowGeneralNoteL( TInt aResourceId, 
    TInt /* aControlId */, const CAknNoteDialog::TTimeout aTimeout,
    const CAknNoteDialog::TTone aTone,TBool aPlural )
    {

    // Create CAknNoteDialog instance
    CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog( aTone, aTimeout );

    dlg->PrepareLC( aResourceId );
    dlg->SetTextPluralityL( aPlural );
    
    // Show the Dialog
    dlg->RunLD();
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::ShowDynamicNumberNoteL()
// Show Dynamic Number Note
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::ShowDynamicNumberNoteL()
    {

    // Create CAknNoteDialog instance
    CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog( 
                                         CAknNoteDialog::EConfirmationTone, 
                                         CAknNoteDialog::ELongTimeout );

    dlg->PrepareLC( R_AKNEXNOTE_DYNAMIC_TEXT );
    TTime theTime( KAknExNoteInitializeTime );
    theTime.UniversalTime();
    TInt64 randSeed( theTime.Int64() );
    TInt number( Math::Rand( randSeed ) % KAknExNoteRandomNumberRange );
    dlg->SetTextPluralityL( EFalse );
    dlg->SetTextNumberL( number );
    
    // Show the Dialog
    dlg->RunLD();
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::ShowWaitNoteL()
// Indicates wait note.
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::ShowWaitNoteL( TInt aResourceId, 
                                         TInt /* aControlId */)
    {        
    // Create CAknWaitDialog instance
    CAknWaitDialog* waitDialog =  new ( ELeave ) CAknWaitDialog( NULL, ETrue );
    
    // Show the Dialog
    waitDialog->ExecuteLD( aResourceId ); 
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::ShowProgressNoteUnderSingleProcessL()
// Show ProgressNote Under Single Process.
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::ShowProgressNoteUnderSingleProcessL( 
                                                          TInt aResourceId, 
                                                          TInt /* aControlId */)
    { 
    // Delete possible previous CAknProgressDialog.
    delete iProgressDialog;

    // Create new CAknProgressDialog.
    iProgressDialog = new ( ELeave ) CAknProgressDialog( reinterpret_cast
                                                         <CEikDialog**> 
                                                         ( &iProgressDialog ) );
    
    iProgressDialog->SetCallback( this );
    iProgressDialog->PrepareLC( aResourceId );
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressInfo->SetFinalValue( KAknExNoteProgressbarFinalValue );
    iProgressDialog->RunLD();

    delete iIdle;
    iIdle = CIdle::NewL( CActive::EPriorityStandard );
    TCallBack callback( CallbackIncrementProgressNoteL, this );
    iIdle->Start( callback );
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::ShowShowNoteL()
// Show Note.
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::ShowShowNoteL( TAknGlobalNoteType aType, 
                                         TInt aResourceId )
    {
    //Allocate TBuf with constant length.
    TBuf<KAknExNoteTextBufLength> text( NULL );

    // Reads a resource into a descriptor.
    CEikonEnv::Static()->ReadResource( text, aResourceId );

    TPtrC noteText( text );

    // Create new CAknGlobalNote instance.
    CAknGlobalNote* globalNote = CAknGlobalNote::NewL();

    // Push CAknGlobalNote's pointer to CleanupStack
    CleanupStack::PushL( globalNote );

    iPermanentNoteId = globalNote->ShowNoteL( aType, noteText );

    // Pop and Destroy CAknGlobalNote's pointer from CleanupStack
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::CallbackIncrementProgressNoteL()
// Just call UpdateProgressNote() function.
// If return 1(ETrue), CIdle calls this again.
// If retrun 0(EFalse), CIdle does not call this.
// -----------------------------------------------------------------------------
//
TInt CAknExNoteContainer::CallbackIncrementProgressNoteL( TAny* aThis )
    {
    return static_cast<CAknExNoteContainer*>( aThis )->UpdateProgressNote();
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::UpdateProgressNote()
// Updates ProgressNote
// -----------------------------------------------------------------------------
//
TInt CAknExNoteContainer::UpdateProgressNote()
    {
    TTime intervalTime;
    intervalTime.HomeTime();
    intervalTime += TTimeIntervalMicroSeconds( 50000 );
    TTime currentTime;
    currentTime.HomeTime();

    while ( intervalTime > currentTime )
        {
        currentTime.HomeTime();
        }

    iProgressInfo->IncrementAndDraw( 1 );
    if ( KAknExNoteProgressbarFinalValue <= iProgressInfo->CurrentValue() )
        {
        iProgressDialog->ProcessFinishedL();
        delete iProgressDialog;
        iProgressDialog = NULL;
        return 0;
        }
    return 1;
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::DialogDismissedL()
// Called when/if the dialog has been dismissed.
// iIdle must be canceled when cancel button is pressed.
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::DialogDismissedL( TInt aButtonId )
    {
    // Check when pressing cancel button.
    if ( aButtonId == -1 )
        { 
        delete iIdle;
        iIdle = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::CreateLabelL()
// Creates label object.
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::CreateLabelL()
    {
    if ( iLabel )
        {
        delete iLabel;
        iLabel = NULL;
        }

    // Create new CEikLabel instance
    iLabel = new ( ELeave ) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    iLabel->SetAlignment( EHCenterVCenter );
    
    // Set lablel's font
    iLabel->SetFont( LatinBold19() );

    // Set the Label's text from resource.
    SetTextToLabelL( R_AKNEXNOTE_TEXT_LABEL_TITLE );
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::OfferKeyEventL()
// Handles the key events.
// -----------------------------------------------------------------------------
//
TKeyResponse CAknExNoteContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                  TEventCode aType )
    {
    // Is not key event?
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

    switch ( aKeyEvent.iCode )
        {
        // Space key.
        case EKeySpace: 

            // Requires to display next outline screen.
            iView->DisplayNextOutlineL();
            DrawNow();
            break;

        // Is list box created?
        default:
            return EKeyWasNotConsumed;
        }
    return EKeyWasConsumed;
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::SizeChanged()
    {
    iLabel->SetRect( Rect() );
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::CountComponentControls()
// Returns number of component.
// -----------------------------------------------------------------------------
//
TInt CAknExNoteContainer::CountComponentControls() const
    {
    // return nbr of controls inside this container
    TInt count( KAknExNoteComponentCount );
    return count;
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::ComponentControl()
// Returns pointer to particular component.
// -----------------------------------------------------------------------------
//
CCoeControl* CAknExNoteContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        // Label object.
        case EAknExNoteComponentLabel: 
            return iLabel;
        // Exception : returns NULL value.
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CAknExNoteContainer::Draw() const
// Clears the window's rectangle.
// -----------------------------------------------------------------------------
//
void CAknExNoteContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KAknExNoteGray );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// End of File
