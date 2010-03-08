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
* Description:  Application View class, CAddressBookView - 
*				 shows the application state on the screen
*
*/


// INCLUDES
#include "AddressBookView.h"
#include "AddressBookEngine.h"
#include "AddressBookContact.h"

#include <eiklabel.h>  // for example label control
#include <barsread.h>
#include <akntitle.h>
#include <eikbtgpc.h>  // CEikButtonGroupContainer
#include <gulicon.h>   // CGulIcon
#include <fbs.h> 	   // CFbsBitmap

#include <SenXmlUtils.h>

#include <AddressBook.rsg>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------- 
// CAddressBookView::CAddressBookView(CAddressBookEngine& aEngine)
// constructor
// ---------------------------------------------------------------------------- 
//
CAddressBookView::CAddressBookView(CAddressBookEngine& aEngine)
:
iEngine(aEngine),
iDetailedView(EFalse),
iSearchList(NULL),
iDetailList(NULL)
    {
    }

// ----------------------------------------------------------------------------  
// CAddressBookView::~CAddressBookView()
// destructor
// ---------------------------------------------------------------------------- 
//
CAddressBookView::~CAddressBookView()
    {
    delete iSearchList;
	if (iItemList)
		iItemList->Reset();
	
	delete iItemList;
	delete iDetailList;
	}
    
// ---------------------------------------------------------------------------- 
// CAddressBookView::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------------------------- 
//
void CAddressBookView::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    
    iFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
    
	iSearchList = new (ELeave) CAknSingleStyleListBox();
	iSearchList->ConstructL(this);
	iSearchList->SetContainerWindowL(*this);
	iSearchList->CreateScrollBarFrameL(ETrue);
	iSearchList->ScrollBarFrame()->SetScrollBarVisibilityL(
													CEikScrollBarFrame::EOn,
													CEikScrollBarFrame::EAuto);

	iItemList = new (ELeave) CDesCArrayFlat(3);
	iSearchList->Model()->SetItemTextArray(iItemList);
	iSearchList->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	
	iDetailList = new (ELeave) CAknFormDoubleStyleListBox;
	iDetailList->ConstructL(this);
	iDetailList->SetContainerWindowL(*this);
	iDetailList->CreateScrollBarFrameL(ETrue);
	iDetailList->ScrollBarFrame()->SetScrollBarVisibilityL(
												CEikScrollBarFrame::EOn,
												CEikScrollBarFrame::EAuto);
	iDetailList->Model()->SetOwnershipType(ELbmOwnsItemArray);
	iDetailList->MakeVisible(EFalse);
	iSearchList->MakeVisible(ETrue); // show "no results" at startup
	iSearchList->SetListBoxObserver(this);
    SetRect(aRect);
    ActivateL();
	MakeVisible(ETrue);
	}

// ---------------------------------------------------------------------------- 
// CAddressBookView::SizeChanged()
// Is called by framework when the view size is changed
// ---------------------------------------------------------------------------- 
//
void CAddressBookView::SizeChanged()
    {
	if (iSearchList)
		{
		iSearchList->SetExtent( TPoint(0,0), iSearchList->MinimumSize() );
		}
	if (iDetailList)
		{
		iDetailList->SetExtent( TPoint(0,0), iDetailList->MinimumSize() );
		}
    }
    
// ---------------------------------------------------------------------------- 
// CAddressBookView::UpdateL(RPointerArray<CAddressBookContact>& aContactArray)  
// Updates bufer containing the contact information when search was finished 
// ---------------------------------------------------------------------------- 
//
void CAddressBookView::UpdateL(RPointerArray<CAddressBookContact>& aContactArray)
	{
	iItemList->Reset();
	iSearchList->MakeVisible(ETrue);
	TInt count = aContactArray.Count();
	if(count > 0)
		{
		for( TInt i = 0; i < count; ++i )
			{
			CAddressBookContact* item = aContactArray[i];
			
			HBufC8* pRow = HBufC8::NewLC(128);
			TPtr8 ptrRow = pRow->Des();
			if (item->MobilePhoneL() != KNullDesC8)
				{
				ptrRow.Append(KHasPhoneChar);	
				}
			ptrRow.Append(KTab);
			ptrRow.Append(item->LastName());
			ptrRow.Append(KSpace);
			ptrRow.Append(item->FirstName());
			ptrRow.Append(KTab);
			ptrRow.Append(KTab);

			HBufC* pRowAsUnicode = SenXmlUtils::ToUnicodeLC(ptrRow);
			if(pRowAsUnicode)
				{
				iItemList->AppendL(*pRowAsUnicode);
				}
			CleanupStack::PopAndDestroy(pRowAsUnicode); 
			CleanupStack::PopAndDestroy(pRow); 
			}
		
		iSearchList->HandleItemAdditionL();
		iSearchList->SetFocus(ETrue, EDrawNow);
		iSearchList->SetCurrentItemIndexAndDraw(0);
		}
	else
		{
		iSearchList->HandleItemRemovalL();
		}
	}

// ---------------------------------------------------------------------------- 
// CAddressBookView::CountComponentControls() const
// it is called by the active scheduler
// ---------------------------------------------------------------------------- 
//
TInt CAddressBookView::CountComponentControls() const
    {
    return 1; 
    }

// ---------------------------------------------------------------------------- 
// CAddressBookView::ComponentControl(TInt aIndex) const
// it is called by the active scheduler
// ---------------------------------------------------------------------------- 
//
CCoeControl* CAddressBookView::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
			if (iDetailedView)
				return iDetailList;
			else
				{
				return iSearchList;
				}
        default:
            return NULL;
        }
    }
    
// ---------------------------------------------------------------------------- 
// CAddressBookView::Draw(const TRect& aRect) const
// ---------------------------------------------------------------------------- 
//
void CAddressBookView::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );

	gc.UseFont(iFont);
	TInt lineHeight = iFont->HeightInPixels()+1;
	
	const TInt col = 5;
	gc.DrawText(KAboutString1, TPoint(col, lineHeight));
	gc.DrawText(KAboutString2, TPoint(col, lineHeight*2));
	gc.DrawText(KAboutString3, TPoint(col, lineHeight*3));	
	gc.DrawText(KAboutString4, TPoint(col, lineHeight*4));
	gc.DrawText(KAboutString5, TPoint(col, lineHeight*5));
    }

// ---------------------------------------------------------------------------- 
// CAddressBookView::HandleControlEventL(CCoeControl*,TCoeEvent)
// ---------------------------------------------------------------------------- 
//
void CAddressBookView::HandleControlEventL(CCoeControl*, TCoeEvent)
    {
    }

// ---------------------------------------------------------------------------- 
// CaddressBookView::CloseItem()
// It is called when Contact information was browsed and needed to be closed
// ---------------------------------------------------------------------------- 
//    
void CAddressBookView::CloseItem()
	{
	iDetailedView = EFalse;
	iSearchList->MakeVisible(ETrue);
	iDetailList->MakeVisible(EFalse);
	DrawDeferred();
	}
// ---------------------------------------------------------------------------- 
// CaddressBookView::OpenItemL()
// to browse Contact information
// ---------------------------------------------------------------------------- 
//
void CAddressBookView::OpenItemL()
	{
	if (iDetailedView)
		return;

	CAddressBookContact* item = NULL; 
	TInt success = iEngine.GetContact(iSearchList->CurrentItemIndex(), item);
	if (success == KErrNotFound)
		return;
	iSearchList->MakeVisible(EFalse);
	CDesCArray* array = new (ELeave) CDesCArrayFlat(3);

	AppendRowL(item->LastName(), KLastname, *array);
	AppendRowL(item->FirstName(), KFirstname,*array);
	AppendRowL(item->MiddleName(), KMiddlename, *array);
	AppendRowL(item->ExtraAddress(), KExtAddr, *array);
	AppendRowL(item->Street(), KStreet, *array);
	AppendRowL(item->Locality(), KLocality, *array);
	AppendRowL(item->Region(), KRegion, *array);
	AppendRowL(item->PostCode(), KPCode, *array);
	AppendRowL(item->Country(), KCountry, *array);
	AppendRowL(item->PhoneL(), KPhone, *array);
	AppendRowL(item->MobilePhoneL(), KMobile, *array);
	AppendRowL(item->FaxL(), KFax, *array);
	AppendRowL(item->Email(), KEmail, *array);
	AppendRowL(item->Title(), KTitle, *array);
	AppendRowL(item->Organization(), KOrganization, *array);
	AppendRowL(item->Unit(), KUnit, *array);
	
	iDetailedView = ETrue;
	iDetailList->Model()->SetItemTextArray(array);
	iDetailList->MakeVisible(ETrue);
	iDetailList->SetCurrentItemIndexAndDraw(0);
	SizeChanged();
	iDetailList->ActivateL();
	}

// ---------------------------------------------------------------------------- 
// CAddressBookView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType )
// the key event handler function.
// It is called every time when user presses some button.
// ---------------------------------------------------------------------------- 
//
TKeyResponse CAddressBookView::OfferKeyEventL(const TKeyEvent& aKeyEvent,
														TEventCode aType )
	{
	return ComponentControl(0)->OfferKeyEventL(aKeyEvent,aType);
	}
	
// ---------------------------------------------------------------------------- 
// CAddressBookView::HandleListBoxEventL(CEikListBox*, TListBoxEvent aEventType)
// It is called when user want to browse the card (full contact information)
// about employees who was found
// ---------------------------------------------------------------------------- 
//	
void CAddressBookView::HandleListBoxEventL(CEikListBox*, TListBoxEvent aEventType)
    {
	if (aEventType == EEventEnterKeyPressed)
		{
		OpenItemL();
		ChangeCbaL(R_AVKON_SOFTKEYS_OPTIONS_BACK, EDrawNow);
		}
    }
    
// ---------------------------------------------------------------------------- 
// CAddressBookView::ChangeCbaL(TInt aResourceId, TDrawNow aDrawNow)
// It is called when need to open or close some item 
// to change common button area
// ---------------------------------------------------------------------------- 
//
void CAddressBookView::ChangeCbaL(TInt aResourceId, TDrawNow aDrawNow)
    {
    CEikButtonGroupContainer* Cba = CEikButtonGroupContainer::Current();
    if(Cba)
    	{
    	Cba->SetCommandSetL(aResourceId);
	
    	if(aDrawNow)
        	Cba->DrawNow();
    	else
       		Cba->DrawDeferred();	
   		}
    }

// ---------------------------------------------------------------------------- 
// CAddressBookView::ShowAbout(TBool aAboutVisible)
// Shows information about Address Book aplication.
// ---------------------------------------------------------------------------- 
//
bool CAddressBookView::ShowAbout(TBool aAboutVisible)
	{
	iDetailedView = EFalse;
	iSearchList->MakeVisible(!aAboutVisible);
	iDetailList->MakeVisible(EFalse);
	DrawDeferred();
	return aAboutVisible; 
	}

// ---------------------------------------------------------------------------- 
// CAddressBookView::CurrentContact()
// Is needed to add current contact into PnoneBook database
// ---------------------------------------------------------------------------- 
//
CAddressBookContact* CAddressBookView::CurrentContact()
	{
	CAddressBookContact* pContact = NULL;
	iEngine.GetContact(iSearchList->CurrentItemIndex(), 
						pContact // pContact remains NULL if not found
						); 
	return pContact;
	}

// ---------------------------------------------------------------------------- 
// CAddressBookView::AppendRowL(TPtrC8 aValue, const TDesC8& aFieldName, 
//									CDesCArray& aArray)
// Creates a card (full contact information) about a person who was found
// ---------------------------------------------------------------------------- 
//
void CAddressBookView::AppendRowL(TPtrC8 aValue, const TDesC8& aFieldName, 
									CDesCArray& aArray)
	{
	if (aValue != KNullDesC8)
		{
		TBuf8<64> row;
		HBufC* pRowAsUnicode = NULL;
		row.Format(KRowFormat, &aFieldName, &aValue);
		pRowAsUnicode = SenXmlUtils::ToUnicodeLC(row);
		if(pRowAsUnicode)
			{
			aArray.AppendL(*pRowAsUnicode);
			CleanupStack::PopAndDestroy(pRowAsUnicode); 
			}
		}
	}

// End of File  

