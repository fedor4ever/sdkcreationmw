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
* Description:  
*
*/


// INCLUDE FILES
#include "RegistrationView.h"
#include "RegistrationListbox.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CRegistrationView::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRegistrationView* CRegistrationView::NewL()
    {
    CRegistrationView* self = CRegistrationView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CRegistrationView::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRegistrationView* CRegistrationView::NewLC()
    {
    CRegistrationView* self = new ( ELeave ) CRegistrationView;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ----------------------------------------------------------------------------
// CRegistrationView::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CRegistrationView::ConstructL()
    {
    BaseConstructL();
	
    iListbox = new ( ELeave ) CRegistrationListbox;
    iListbox->SetData( &iItemData );
    iListbox->SetMopParent( this );
    iListbox->ConstructFromResourceL( R_REGISTRATION_SETTING_ITEM_LIST );

    AppUi()->AddToStackL( *this, iListbox );
    iListbox->MakeVisible( ETrue );
    iListbox->SetRect( ClientRect() );
    iListbox->ActivateL();
    iListbox->DrawNow();
    
    }

// ----------------------------------------------------------------------------
// CRegistrationView::CRegistrationView()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CRegistrationView::CRegistrationView()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CRegistrationView::~CRegistrationView()
// Destructor.
// ----------------------------------------------------------------------------
//
CRegistrationView::~CRegistrationView()
    {
    if ( iListbox )
        {
        AppUi()->RemoveFromStack( iListbox );
        }

    delete iListbox;
    }


// ----------------------------------------------------------------------------
// CRegistrationView::Id() const
// Returns ID of View.
// ----------------------------------------------------------------------------
//
TUid CRegistrationView::Id() const
    {
    return KViewId;
    }

// ----------------------------------------------------------------------------
// CRegistrationView::HandleCommandL()
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CRegistrationView::HandleCommandL( TInt aCommand )
    {

    switch ( aCommand )
        {
        case EAknCmdExit:
        case EAknSoftkeyBack:
            {
            AppUi()->ProcessCommandL( EAknCmdExit );
            break;
            }
        
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CRegistrationView::SizeChanged
// Called by AppUi when the view size is changed.
// ----------------------------------------------------------------------------
//
void CRegistrationView::SizeChanged()
  {
  iListbox->SetRect( ClientRect() );
  }

// ----------------------------------------------------------------------------
// CCRegistrationView::DoActivateL()
// ----------------------------------------------------------------------------
//
void CRegistrationView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                         TUid /*aCustomMessageId*/,
                                         const TDesC8& /*aCustomMessage*/ )
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CRegistrationView::DoDeactivate()
// ----------------------------------------------------------------------------
//
void CRegistrationView::DoDeactivate()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CRegistrationView::ValidateInput()
// Checks validity of input.
// Returns TBool True if input is valid and EFalse if not valid. 
// ----------------------------------------------------------------------------
//
TBool CRegistrationView::ValidateInput()
    {
    _LIT( KAt, "@" );
    TBool isValid = EFalse;
    TBuf<KMaxEmailLength> iEmailEnd;
    
    //Searching for @ character. 
	// Returns index of character if found. Returns KErrNotFound if not found.
    TInt index  = iItemData.iEmail.Find( KAt );
    if( KErrNotFound != index )
        {
	    if ( iItemData.iEmail.Length() > ++index )
	        {
	        // iEmailEnd contains email address from @ to end.
	        iEmailEnd = iItemData.iEmail.MidTPtr( index );
	    
	        // Search for @ is performed again.
	        if ( iEmailEnd.Find(KAt) == KErrNotFound )
	            {
	    	    isValid = ETrue;
	            }
            }
        else
            {
            isValid = ETrue;
            }
        }
     return isValid;
     }  

// End of File
