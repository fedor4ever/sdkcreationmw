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
#include "IMOpenAPIExampleContainer.h"
#include "IMOpenAPIExampleImg.h"
#include "IMOpenAPIExampleGrid.h"
#include "IMOpenAPIExamplemodel.h"

#include <eiklabel.h>
#include <e32base.h> 
#include <stringloader.h>
#include <e32des16.h>
#include <akniconutils.h>
#include <barsread.h>
#include <COECNTRL.H>
#include <AknAppUi.h>
#include <IMOpenAPIExample.rsg>

// CONSTANTS

// This is the file where we have our bitmaps
_LIT (KImageFileName, "mysigns.mbm");

// The location of the above file changes in the emulator and target builds.
#if defined(__WINS__) && defined(_DEBUG)
	_LIT( KImageFilePath, "z:\\resource\\apps\\" );
#else
// drive-neutral path (MMC-installations safe)
	_LIT( KImageFilePath, "\\resource\\apps\\");
#endif

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CIMOpenAPIExampleContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();


	iMySign = EZeroSign;

    iTurnIndicator = new (ELeave) CEikLabel;
    iTurnIndicator->SetContainerWindowL( *this );
    iTurnIndicator->SetTextL(KNullDesC);

    iSignIndicator = new (ELeave) CEikLabel;
    iSignIndicator->SetContainerWindowL( *this );
    iSignIndicator->SetTextL(KNullDesC);

	
	iCrossSign = new (ELeave) CFbsBitmap;
	iZeroSign = new (ELeave) CFbsBitmap;
	iEmptySign = new (ELeave) CFbsBitmap;
	iMySignBMP = new (ELeave) CFbsBitmap;
	
    iImageFileName = KImageFilePath;
    iImageFileName.Append(KImageFileName);
	LoadMySignBMPL(iImageFileName);
	
	// Construct the grid
	iGamesGrid = new (ELeave)CIMOpenAPIExampleGrid;
	iGamesGrid->SetContainerWindowL(*this);
	iGamesGrid->ConstructL(R_IMOPENAPIEXAMPLE_GRID, iImageFileName);
	

	SetRect(aRect);
    ActivateL();
    }

// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::~CIMOpenAPIExampleContainer()
// Destructor
// ---------------------------------------------------------
//
CIMOpenAPIExampleContainer::~CIMOpenAPIExampleContainer()
    {
    delete iTurnIndicator;
    iTurnIndicator = NULL;
    
    delete iSignIndicator;
    iSignIndicator = NULL;
	
    delete iCrossSign;
    iCrossSign = NULL;

	delete iMySignBMP;
    iMySignBMP = NULL;
	
    delete iZeroSign;
    iZeroSign = NULL;
    
    delete iEmptySign;
    iEmptySign = NULL;
	
    delete iGamesGrid;
    iGamesGrid = NULL;
    
    }

// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CIMOpenAPIExampleContainer::SizeChanged()
    {
    TSize selectedImageSize( 91, 91);
    TPoint selectedImageTopLeft( 70, 80 );
	
	iTurnIndicator->SetExtent( TPoint(1,2), iTurnIndicator->MinimumSize() );
    iSignIndicator->SetExtent( TPoint(1,30), iSignIndicator->MinimumSize() );
	
    if (iGamesGrid)
		{
		iGamesGrid->SetExtent (selectedImageTopLeft,selectedImageSize);
		}

    }

// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CIMOpenAPIExampleContainer::CountComponentControls() const
    {
    return 3; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CIMOpenAPIExampleContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iTurnIndicator;
        case 1:
            return iSignIndicator;
        case 2:
			return iGamesGrid;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CIMOpenAPIExampleContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();

    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetBrushColor( KRgbGreen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );

	// Drawing my sign
	TSize selectedImageSize( 15, 15 );
    TPoint selectedImageTopLeft( 160, 35 );
	TRect mySignPlace( selectedImageTopLeft, selectedImageSize );
    gc.DrawBitmap( mySignPlace, iMySignBMP );


    }

// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CIMOpenAPIExampleContainer::HandleControlEventL(
    CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
    {
    }

// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::LoadMySignBMP()
// Loads BMP required to display my sign, 1=*, 0=0
// ---------------------------------------------------------
//
void CIMOpenAPIExampleContainer::LoadMySignBMPL(const TDesC& aImageFile)
	{
	TInt err;

    err = iCrossSign->Load( aImageFile, 0, ETrue);
	User::LeaveIfError(err);

    err = iZeroSign->Load( aImageFile, 1, ETrue);
	User::LeaveIfError(err);

    err = iEmptySign->Load( aImageFile, 2, ETrue);
	User::LeaveIfError(err);

    err = iMySignBMP->Load( aImageFile, iMySign, ETrue);
	User::LeaveIfError(err);

	}
	
// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CIMOpenAPIExampleContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	
	// if it's not an event key we're not interested so return not consumed
	if (aType != EEventKey)
		return EKeyWasNotConsumed;
	
	TInt code = aKeyEvent.iCode;

    if ( iGamesGrid && ( !iGamesGrid->IsDimmed() ) ) //&& iGamesGrid->IsFocused()) 
        {
        if ( code == EKeyOK)
            {
            PlayMove();
            return( EKeyWasConsumed );
            }
		else
		    {
   			TKeyResponse returnValue = iGamesGrid->OfferKeyEventL( aKeyEvent, aType );
   		    DrawNow();
   		    return returnValue;
    	    }
        }
        
	return EKeyWasNotConsumed;
	}
	
// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::PlayMove
// ---------------------------------------------------------
//
void CIMOpenAPIExampleContainer::PlayMove()
    {
    TInt currentIndex = iGamesGrid->CurrentDataIndex();
    if ( iModel->IsIndexPositionEmpty(currentIndex) )
        {
        SetCellSign(iMySign, currentIndex);
        iModel->SendPlayedIndex(currentIndex);
        }
    }
    
// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::SetCellSign
// ---------------------------------------------------------
//
void CIMOpenAPIExampleContainer::SetCellSign(TGameSign aCellSign, TInt aCellIndex)
	{
    iGamesGrid->SetCellSign(aCellSign, aCellIndex);
    DrawNow();
	}
	
// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::SetMySign
// ---------------------------------------------------------
//
void CIMOpenAPIExampleContainer::SetMySign(const TGameSign mysign)
	{
	iMySign = mysign;
	LoadMySignBMPL(iImageFileName);
	DrawNow();	
	}
	
// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::SetGameView
// ---------------------------------------------------------
//
void CIMOpenAPIExampleContainer::SetGameView(const TGameState desiredView)
	{
    TBuf<25> iTurnIndicatorString(KNullDesC);
	TBuf<25> iSignIndicatorString(KNullDesC);
    
	switch (desiredView)
		{
		case EStateNotLogin:
			iGamesGrid->MakeVisible(EFalse);
			iSignIndicator->MakeVisible(EFalse);
			StringLoader::Load ( iTurnIndicatorString, R_INFO_MSG_NOT_LOGGEDIN );
			iTurnIndicator->SetTextL(iTurnIndicatorString);
			break;

		case EStateLoginIdle:
			iGamesGrid->MakeVisible(EFalse);
			iSignIndicator->MakeVisible(EFalse);
			StringLoader::Load ( iTurnIndicatorString, R_INFO_MSG_LOGIN_SUCCESS );
			iTurnIndicator->SetTextL(iTurnIndicatorString);
			break;

		case EStateChallenged:
			iGamesGrid->MakeVisible(EFalse);
			iSignIndicator->MakeVisible(EFalse);
			StringLoader::Load ( iTurnIndicatorString, R_INFO_MSG_WAITING_FOR_REPLY );
			iTurnIndicator->SetTextL(iTurnIndicatorString);
			break;

		case EStateReadytoStart:
            {
			iGamesGrid->MakeVisible(ETrue);
            iGamesGrid->SetDimmed(ETrue);
			iSignIndicator->MakeVisible(ETrue);
			StringLoader::Load ( iSignIndicatorString, R_INFO_MSG_YOUR_SIGN_IS );
			iSignIndicator->SetTextL(iSignIndicatorString);
            if (iMySign == ECrossSign)
                StringLoader::Load ( iTurnIndicatorString, R_INFO_MSG_START_GAME );
            else
                StringLoader::Load ( iTurnIndicatorString, R_INFO_MSG_WAITING_OPP_TO_START );
			iTurnIndicator->SetTextL(iTurnIndicatorString);
			iGamesGrid->ResetGrid();
			break;
            }

		case EStateWaitingForOppMove:
			iGamesGrid->MakeVisible(ETrue);
			iGamesGrid->SetDimmed(ETrue);
			iSignIndicator->MakeVisible(ETrue);
			StringLoader::Load ( iSignIndicatorString, R_INFO_MSG_YOUR_SIGN_IS );
			iSignIndicator->SetTextL(iSignIndicatorString);
			StringLoader::Load ( iTurnIndicatorString, R_INFO_MSG_OPPONENT_MOVE );
			iTurnIndicator->SetTextL(iTurnIndicatorString);
			break;


		case EStateWaitingForOwnMove:
			iGamesGrid->MakeVisible(ETrue);
			iGamesGrid->SetDimmed(EFalse);
			iSignIndicator->MakeVisible(ETrue);
			StringLoader::Load ( iSignIndicatorString, R_INFO_MSG_YOUR_SIGN_IS );
			iSignIndicator->SetTextL(iSignIndicatorString);
			StringLoader::Load ( iTurnIndicatorString, R_INFO_MSG_YOUR_MOVE );
			iTurnIndicator->SetTextL(iTurnIndicatorString);
			break;

		default:
			break;
		}

	SizeChanged();
	DrawNow();
	return;
	}
	
// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::SetModel
// ---------------------------------------------------------
//	
void CIMOpenAPIExampleContainer::SetModel(CIMOpenAPIExamplemodel* aModel)
    {
    iModel = aModel;
    }
    
// ---------------------------------------------------------
// CIMOpenAPIExampleContainer::ActivateL
// ---------------------------------------------------------
//	
void CIMOpenAPIExampleContainer::ActivateL()
	{
	CCoeControl::ActivateL();
	if(iCba)
		{
		iCba->DrawableWindow()->SetOrdinalPosition(0);
		}
	else
		{
		iAvkonAppUi->Cba()->DrawableWindow()->SetOrdinalPosition(0);
		}
	}
    
// End of File  

