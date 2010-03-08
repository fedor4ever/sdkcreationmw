/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application UI class, CAddressBookAppUi - 
*				 responsible for handling user commands
*
*/


// INCLUDES
#include "AddressBookAppUi.h"
#include "AddressBookView.h" 
#include <AddressBook.rsg>
#include "AddressBook.hrh"
#include "AddressBookEngine.h"
#include "AddressBookContact.h"

#include <SenXmlUtils.h>

#include <cpbkfieldinfo.h> 
#include <cpbkfieldsinfo.h>

#include <avkon.hrh>
#include <eikmenup.h>
#include <aknnotewrappers.h> 
#include <aknquerydialog.h> 

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CAddressBookAppUi::ConstructL()
// ----------------------------------------------------------------------------
//
void CAddressBookAppUi::ConstructL()
    {
    BaseConstructL(CAknAppUi::EAknEnableSkin);
	iWaitNote = CAknWaitNoteWrapper::NewL();
	iReady = EFalse;
	iFirstTime = ETrue;
	iConnecting = EFalse;
	iAbout = EFalse;

	iEngine = CAddressBookEngine::NewL(*this);
    iAppContainer = new (ELeave) CAddressBookView(*iEngine);
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
    iContactEngine = CPbkContactEngine::NewL(NULL);
    }

// ----------------------------------------------------------------------------
// CAddressBookAppUi::~CAddressBookAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------------------------------
//
CAddressBookAppUi::~CAddressBookAppUi()
    {
    if (iAppContainer)
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
	delete iEngine;
	delete iContactEngine;
	delete iWaitNote;
   }

// ----------------------------------------------------------------------------
// CAddressBookAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
// This function is called by the EIKON framework just before it displays
// a menu pane. Sets the state of menu items dynamically according
// to the state of application data.
// ----------------------------------------------------------------------------
//
void CAddressBookAppUi::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
    {
	if (aResourceId == R_ADDRESSBOOK_MENU)
		{
		TBool connected = iEngine->Connected();
		TBool isContact(EFalse);
		if ((iAppContainer->CurrentContact()) && (!iAbout))
			isContact = ETrue;

		aMenuPane->SetItemDimmed(EAddressBookCmdAppConnect, connected);
		aMenuPane->SetItemDimmed(EAddressBookCmdAppSearch, !connected);
		aMenuPane->SetItemDimmed(EAddressBookCmdAppAdd, !isContact);
		aMenuPane->SetItemDimmed(EAddressBookCmdAppConnectAbout, iAbout);
		}
	}

// ----------------------------------------------------------------------------
// CAddressBookAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode) 
// takes care of key event handling
// ----------------------------------------------------------------------------
//
TKeyResponse CAddressBookAppUi::HandleKeyEventL(
    const TKeyEvent&,TEventCode)
    {
	return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// CAddressBookAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// ----------------------------------------------------------------------------
//
void CAddressBookAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
			iAppContainer->ChangeCbaL(R_AVKON_SOFTKEYS_OPTIONS_EXIT, EDrawNow);
			iAppContainer->CloseItem();
			iAbout = iAppContainer->ShowAbout(EFalse);
			break;
        case EEikCmdExit:
		case EAknSoftkeyExit:
            {
            Exit();
            break;
            }
		case EAddressBookCmdAppConnect:
			{
			iReady = EFalse;
			iConnecting = EFalse;
		
			// ---------------------------------------------------------------------
			// ExecuteL(R_CONNECTING_NOTE, *this, ETrue) runs the active scheduler, 
			// that calls CAknWaitNoteWrapper::RunL(),it checks 
			//       1. if is process already done - then calls function IsProcessDone() 
			//		    (it is overridden from interface MAknBackgroundProcess to AddressBookAppUi)
			//       2. if process is not finished yet then RunL() calls StepL() that does one step of processing
			// 			(it is overridden from interface MAknBackgroundProcess to AddressBookAppUi)
			// 			This step establishes connection.
			// After establishing of connection the active scheduler calls 
			// function DialogDismissedL(TInt aButtonId) which is overriden 
			// in AddressBookAppUi from interface MAknBackgroundProcess 
			// ---------------------------------------------------------------------
			//
			iWaitNote->ExecuteL(R_CONNECTING_NOTE, *this, ETrue);
			iAbout = iAppContainer->ShowAbout(EFalse);
			break;
			}
        case EAddressBookCmdAppSearch:
            {
            if (iFirstTime)
				{
				_LIT(KInfoLine, "How to Search? Use first name, last name or phone number as criteria for search.");
				CAknInformationNote* note = new (ELeave) CAknInformationNote(ETrue);
				note->ExecuteLD(KInfoLine);
				}
			TBuf<50> searchFor;
			CAknTextQueryDialog* dlg = new(ELeave)CAknTextQueryDialog(searchFor, CAknQueryDialog::ENoTone );
			dlg->SetPredictiveTextInputPermitted(ETrue);
			if (dlg->ExecuteLD( R_MY_DATA_QUERY_DIALOG ) )
				{
				iFirstTime = EFalse;
				iReady = EFalse;
				iEngine->SendSearchL(searchFor);
				
				// ---------------------------------------------------------------------
				// ExecuteL(R_SEARCHING_NOTE, *this) runs the active scheduler, 
				// that calls CSenServiceConnectionImpl::RunL() and it calls 
				// function HandleMessageL(const TDesC8& aMessage)
				// (it is overridden from interface MSenServiceConsumer to AddressBookAppUi)
				// for receiving messages from the SendL()(CSenServiceConnection)function
				// call
				// ---------------------------------------------------------------------
				//
				iWaitNote->ExecuteL(R_SEARCHING_NOTE, *this);
				iAppContainer->ChangeCbaL(R_AVKON_SOFTKEYS_OPTIONS_EXIT, EDrawNow);
				iAbout = iAppContainer->ShowAbout(EFalse);
				}
            break;
            }
		case EAddressBookCmdAppConnectAbout:
			iAbout = iAppContainer->ShowAbout();
			iAppContainer->ChangeCbaL(R_AVKON_SOFTKEYS_OPTIONS_BACK, EDrawNow);
			break;
		case EAddressBookCmdAppAdd:
			{
			CAddressBookContact* pContact = iAppContainer->CurrentContact();
			if (pContact)
				{
				CPbkContactItem* newItem = 
								iContactEngine->CreateEmptyContactL();
				CleanupStack::PushL(newItem);
				// Update personal information
				UpdateFieldL( newItem, EPbkFieldIdPhoneNumberMobile, 
					pContact->MobilePhoneL());
				UpdateFieldL( newItem, EPbkFieldIdPhoneNumberGeneral, 
					pContact->PhoneL());
				UpdateFieldL( newItem, EPbkFieldIdFirstName, 
					pContact->FirstName());
				UpdateFieldL( newItem, EPbkFieldIdLastName, 
					pContact->LastName());
				UpdateFieldL( newItem, EPbkFieldIdFaxNumber, 
					pContact->FaxL());
				UpdateFieldL( newItem, EPbkFieldIdEmailAddress, 
					pContact->Email());
				UpdateFieldL( newItem, EPbkFieldIdJobTitle, 
					pContact->Title());
				UpdateFieldL( newItem, EPbkFieldIdCompanyName, 
					pContact->Organization());
				UpdateFieldL( newItem, EPbkFieldIdPostalAddress, 
					pContact->Street());

				// add the new contact to the phone book. 
				iContactEngine->AddNewContactL(*newItem);
				CleanupStack::PopAndDestroy(newItem);
				}
			}
			break;
        default:
            break;
        }
  }

// ----------------------------------------------------------------------------
// CAddressBookAppUi::DialogDismissedL(TInt aButtonId)
// It is overridden function from interface MAknBackgroundProcess
// and is called by the active scheduler to dismiss the dialog
// ----------------------------------------------------------------------------
//
void CAddressBookAppUi::DialogDismissedL(TInt aButtonId)
	{
	if (aButtonId == EAknSoftkeyCancel)
		{
		iEngine->Cancel();
		}
	}

// ----------------------------------------------------------------------------
// CAddressBookAppUi::IsProcessDone() const
// It is overridden function from interface MAknBackgroundProcess
// and is called by the active scheduler to check if is process already done 
// ----------------------------------------------------------------------------
//
TBool CAddressBookAppUi::IsProcessDone() const
	{
	return iReady;
	}

// ----------------------------------------------------------------------------
// CAddressBookAppUi::ProcessFinished() 
// It is overridden function from interface MAknBackgroundProcess
// and is called by the active scheduler to finish the process 
// ----------------------------------------------------------------------------
//
void CAddressBookAppUi::ProcessFinished()
	{
	}

// ----------------------------------------------------------------------------
// CAddressBookAppUi::StepL() 
// It is overridden function from interface MAknBackgroundProcess
// and is called by the active scheduler to do one step of processing
// Purpose this function in AddressBook to run AddressBookEngine::ConnectL() 
// if connection is not established yet
// ----------------------------------------------------------------------------
//
void CAddressBookAppUi::StepL()
	{
	if (!iConnecting)
		{
		iConnecting = ETrue;	// do this only once
		iEngine->ConnectL();
		}
	}

// ----------------------------------------------------------------------------
// CAddressBookAppUi::ErrorL(TInt aCode)
// It is overridden function from interface MAddressBookEngineObserver
// Checks errors that can happen
// ----------------------------------------------------------------------------
//
void CAddressBookAppUi::ErrorL(TInt aCode)
	{
	_LIT(KNotReady, "Connection is not ready.");
	_LIT(KTooManyMatches, "Too many search results, please refine your search.");
	_LIT(KSearchFailed, "Internal error, please try again.");

	iReady = ETrue;
	CAknErrorNote* note = new (ELeave) CAknErrorNote(EFalse);
	TBuf<70> errorMessage;
	switch(aCode) 
		{
		case EConnectionNotReady:
			errorMessage = KNotReady;
			break;
		case ETooManyMatches:
			errorMessage = KTooManyMatches;
			break;
		case ESearchFailed:
			errorMessage = KSearchFailed;
			break;
		default:
			break;
		}
	note->ExecuteLD(errorMessage);
	}

// ----------------------------------------------------------------------------
// CAddressBookAppUi::ConnectionReady()
// It is overridden function from interface MAddressBookEngineObserver
// It is called by the CAddressBookEngine::SetStatus(const TInt aStatus)
// to set conection's status as ready
// ----------------------------------------------------------------------------
//
void CAddressBookAppUi::ConnectionReady()
	{
	iReady = ETrue;
	}

// ----------------------------------------------------------------------------
// CAddressBookAppUi::UpdateFieldL (CPbkContactItem* modifiableItem,
//                   			   TPbkFieldId fieldId,
//                    			   const TDesC8& newValue)
// Updates field of CPbkContactItem when user select the point of menu "Add to contacts". 
// The change doesn't reflect to database.
// ----------------------------------------------------------------------------
//
void CAddressBookAppUi::UpdateFieldL(CPbkContactItem* modifiableItem,
                                      TPbkFieldId fieldId,
                                      const TDesC8& newValue)
	{
    TPbkContactItemField* Field =
        modifiableItem->FindField(fieldId);
    if( Field )
		{
		HBufC* pValAsUnicode = SenXmlUtils::ToUnicodeLC(newValue);
        if(pValAsUnicode)
			{
			Field->TextStorage()->SetTextL(*pValAsUnicode);
			}
		CleanupStack::PopAndDestroy(pValAsUnicode); 
		}
	}
	
// ----------------------------------------------------------------------------
// CAddressBookAppUi::SearchFinishedL(RPointerArray<CAddressBookContact>& aContactArray) 
// It is overridden function from MAddressBookEngineObserver interface
// Updates bufer containing the contact information and sets status as READY
// ----------------------------------------------------------------------------
//	
void CAddressBookAppUi::SearchFinishedL(RPointerArray<CAddressBookContact>& aContactArray)
	{
	iAppContainer->UpdateL(aContactArray);
	iReady = ETrue;
	}

// End of File  

