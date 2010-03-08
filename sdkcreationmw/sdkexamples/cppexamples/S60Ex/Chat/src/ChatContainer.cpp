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
#include "ChatContainer.h"
#include "Chat.pan"
#include "Chat.hrh"
#include "ChatProtocolConstants.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CChatContainer::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatContainer* CChatContainer::NewL( const TRect& aRect )
    {
    CChatContainer* self = CChatContainer::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CChatContainer::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatContainer* CChatContainer::NewLC( const TRect& aRect )
    {
    CChatContainer* self = new ( ELeave ) CChatContainer;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// ----------------------------------------------------------------------------
// CChatContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CChatContainer::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect( aRect );

    // Create a control to display a list of messages
    iListBox = new ( ELeave ) CAknSingleNumberStyleListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, 0 );

    iListBox->SetRect( aRect.Size() );

    iListBox->ActivateL();
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOn, 
        CEikScrollBarFrame::EAuto );

    // Create an array to hold the messages
    iMessageList = new ( ELeave ) CDesCArrayFlat( KMessageListLen );

    // Give it to the control
    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( iMessageList );

    // transfer ownership of iMessageList
    model->SetOwnershipType( ELbmOwnsItemArray );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CChatContainer::CChatContainer()
// Constructor.
// ----------------------------------------------------------------------------
//
CChatContainer::CChatContainer()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CChatContainer::~CChatContainer()
// Destructor.
// ----------------------------------------------------------------------------
//
CChatContainer::~CChatContainer()
    {
    // iMessageList not deleted as it is owned by iListBox->Model()
    delete iListBox;
    iListBox = NULL;
    }

// ----------------------------------------------------------------------------
// CChatContainer::ClearMessageListL()
// Clears all the entries in the list view.
// ----------------------------------------------------------------------------
//
void CChatContainer::ClearMessageListL()
    {
    iMessageList->Reset();

    iListBox->HandleItemRemovalL();
    iListBox->Reset();
      
    // tell the control about the change
    iListBox->HandleItemAdditionL();
    }

// ----------------------------------------------------------------------------
// CChatContainer::CountComponentControls()
// Returns the specified component control.
// ----------------------------------------------------------------------------
//
TInt CChatContainer::CountComponentControls() const
    {
    return 1; // Only have one Component
    }

// ----------------------------------------------------------------------------
// CChatContainer::ComponentControl()
// Gets the specified component of a compound control.
// ----------------------------------------------------------------------------
//
CCoeControl* CChatContainer::ComponentControl( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex == 0, Panic( EChatInvalidControlIndex ) );
    return iListBox;    //  Return the component
    }

// ----------------------------------------------------------------------------
// CChatContainer::OfferKeyEventL()
// Offer the key event to the list box.
// ----------------------------------------------------------------------------
//
TKeyResponse CChatContainer
::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    return iListBox->OfferKeyEventL( aKeyEvent, aType );
    }

// ----------------------------------------------------------------------------
// CChatContainer::LogL()
// Add an entry to the log.
// ----------------------------------------------------------------------------
//
void CChatContainer::LogL( const TDesC& aText )
    {
    LogL( aText, KNullDesC );
    }

// ----------------------------------------------------------------------------
// CChatContainer::LogL()
// A number to append onto the entry.
// ----------------------------------------------------------------------------
//
void CChatContainer::LogL( const TDesC& aText, TInt aNumber )
    {
    TBuf<KMessageHeaderLen> numberString;
    HBufC* buffer = HBufC::NewLC( KMessageHeaderLen + aText.Length() +
        numberString.Length() );
    numberString.Num( aNumber );
    buffer->Des().Append( aText );
    buffer->Des().Append( ' ' );
    buffer->Des().Append( numberString );
    LogL( buffer->Des(), KNullDesC );
    CleanupStack::PopAndDestroy( buffer );
    }

// ----------------------------------------------------------------------------
// CChatContainer::LogL()
// Extra text to append onto the entry.
// ----------------------------------------------------------------------------
//
void CChatContainer::LogL( const TDesC& aText,
    const TDesC& aExtraText )
    {
    HBufC* buffer = HBufC::NewLC( KMessageHeaderLen + aText.Length() +
        aExtraText.Length() );

    buffer->Des().Append( aExtraText );
    buffer->Des().Append( '\t' );
    buffer->Des().Append( aText );
    
    // add the message to the list
    iMessageList->AppendL( *buffer );
    CleanupStack::PopAndDestroy( buffer );

    // tell the control about the change
    iListBox->HandleItemAdditionL();
    }

// ----------------------------------------------------------------------------
// CChatContainer::ContainsEntries()
// Does the view contain any log entries.
// ----------------------------------------------------------------------------
//
TBool CChatContainer::ContainsEntries()
    {
    return iListBox->Model()->NumberOfItems() != 0;
    }


// ----------------------------------------------------------------------------
// CChatContainer::ShowDataQueryL()
// Display of Data Query.
// ----------------------------------------------------------------------------
//
void CChatContainer::ShowDataQueryL(
    const TInt aQueryResourceId,
    const TInt aTextResourceId,
    const TInt aPromptResoureId,
    const TInt aMaxLength,
    const TBool aDisplayFlag,
    TDes& aText )
    {
    
    iDisplayDialog = ETrue;

    TBuf<KChatTextBufLength> textData;
    
    if ( aTextResourceId )
        {
        iCoeEnv->ReadResourceL( textData, aTextResourceId );
        }
    
    CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL(
        textData, CAknQueryDialog::ENoTone );
    CleanupStack::PushL( dlg );
         
    if ( aPromptResoureId )
        {
        TBuf<KChatTextBufLength> prompt;
        iCoeEnv->ReadResourceL( prompt, aTextResourceId );
        dlg->SetPromptL( prompt );
        }

    if ( aMaxLength )
        {
        dlg->SetMaxLength( aMaxLength );
        }
        
	  CleanupStack::Pop( dlg );
 	  TBool answer( dlg->ExecuteLD( aQueryResourceId ) );
    
    if ( aDisplayFlag && answer )
        {
        HBufC* buffer = HBufC::NewLC( KMessageHeaderLen + 
                                  textData.Length() );

        buffer->Des().Append( '<' );
        buffer->Des().Append( '\t' );
        buffer->Des().Append( textData );
        
        // add the message to the list
        iMessageList->AppendL( *buffer );
        CleanupStack::PopAndDestroy( buffer );
      
        // tell the control about the change
        iListBox->HandleItemAdditionL();
        }

    iDisplayDialog = EFalse;
  
    // get message
    aText = textData;
    
    DrawNow();
    }
 

// -----------------------------------------------------------------------------
// CChatContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CChatContainer::SizeChanged()
    { 
    if ( iListBox ) 
        { 
        iListBox-> SetRect( iAvkonAppUi->ApplicationRect() ); 
        } 
    }
    
// End of File  
