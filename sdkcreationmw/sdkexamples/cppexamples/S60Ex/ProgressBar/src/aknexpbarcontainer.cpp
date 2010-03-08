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
#include <coemain.h>
#include <barsread.h>
#include <aknappui.h>
#include <eikprogi.h>
#include <AknExPbar.rsg>
#include <avkon.hrh>
#include <akntitle.h>
#include <aknnotewrappers.h> 

#include "AknExPbarContainer.h"
#include "AknExPbarTimer.h"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExPbarContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAknExPbarContainer* CAknExPbarContainer::NewL( const TRect& aRect )
    {
    CAknExPbarContainer* self = CAknExPbarContainer::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;    
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAknExPbarContainer* CAknExPbarContainer::NewLC( const TRect& aRect )
    {
    CAknExPbarContainer* self = new ( ELeave ) CAknExPbarContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self; 
    }
    
    
// -----------------------------------------------------------------------------
// CAknExPbarContainer::CAknExPbarContainer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAknExPbarContainer::CAknExPbarContainer()
    {
    for ( TInt ii = 0; ii < KNumberOfBars; ++ii )
        {
        iPBar[ii] = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAknExPbarContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

    iFinalValuesTwisted = EFalse;

    Window().SetShadowDisabled( EFalse );
    CreateProgressBarsL();
    
    SetRect( aRect );
    ActivateL();
    
    ConstructTimerModel();
    ConstructTimerL();    
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::~CAknExPbarContainer()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExPbarContainer::~CAknExPbarContainer()
    {
    for ( TInt ii = 0; ii < KNumberOfBars; ++ii )
        {
        delete( iPBar[ii] );
        iPBar[ii] = NULL;
        }
    
    delete iTimer;
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::IncrementBarsAndDraw()
// Increase progress bar length
// -----------------------------------------------------------------------------
//
void CAknExPbarContainer::IncrementBarsAndDraw( TInt aIncrement )
    {
    TInt index = 0;
    while ( index < KNumberOfBars )
        {
        iPBar[ index ]->IncrementAndDraw( aIncrement );
        ++index;
        }
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::CountComponentControls() 
// return number of controls inside this container.
// -----------------------------------------------------------------------------
//
TInt CAknExPbarContainer::CountComponentControls() const
    {
    return KNumberOfBars;
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::ComponentControl()
// returns pointer of controls inside this container
// -----------------------------------------------------------------------------
//
CCoeControl* CAknExPbarContainer::ComponentControl( TInt aIndex ) const
    {
    return ( iPBar[ aIndex ] );
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer:Draw()
// Draws the display
// -----------------------------------------------------------------------------
//
void CAknExPbarContainer::Draw( const TRect& aRect ) const
    {
    TRgb MyRgb;
    MyRgb = KAknExPbarBlack;

    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( MyRgb );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::HandleControlEventL()
// Handles an event of type aEventType reported
// by the control aControl to this observer.
// -----------------------------------------------------------------------------
//
void CAknExPbarContainer::HandleControlEventL( CCoeControl* /*aControl*/,
                                               TCoeEvent /*aEventType*/ )
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CAknExPbarContainer::CreateProgressBarsL()
// Create progress bars
// -----------------------------------------------------------------------------
//
void CAknExPbarContainer::CreateProgressBarsL()
    {        
    TResourceReader reader;
    
    // Set Progress Bar property, using resource
    iCoeEnv->CreateResourceReaderLC( reader, R_AKNEXPBAR_PROGRESS_BAR_0 );            
    iPBar[ 0 ] = new( ELeave ) CEikProgressInfo();
    iPBar[ 0 ]->ConstructFromResourceL( reader );
    
    // PopAndDestroy reader from CleanupStack
    CleanupStack::PopAndDestroy();    
      
    iPBar[ 0 ]->SetPosition( ( TPoint( KProgressBar0Position_X,
                               KProgressBar0Position_Y ) ) );
    iPBar[ 0 ]->SetContainerWindowL( *this );
        
    
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    
    // Set Progress Bar property, coding directly
    CEikProgressInfo::SInfo info;
    info.iHeight = KProgressBarHeight;
    info.iSplitsInBlock = KProgressBarSplitsInBlock;
    info.iTextType = EEikProgressTextNone;
    
    //Set progress bar widht using current rectangle width
    info.iWidth = (rect.Width() - KProgressBar1EmptyLeftSpace);
    info.iFinalValue = KProgressBar1FinalValue;
    iPBar[ 1 ] = new( ELeave ) CEikProgressInfo( info );
    iPBar[ 1 ]->ConstructL();
    iPBar[ 1 ]->SetPosition( ( TPoint( KProgressBar1Position_X,
                               KProgressBar1Position_Y ) ) );
    
    iPBar[ 1 ]->SetContainerWindowL( *this );       
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::ReConstructProgressBarsL()
// Deletes old progressbars and creates new ones. Called  from 
// CAknExPbarAppUi::HandleResourceSizeChangeL() when display layout is changed.
// -----------------------------------------------------------------------------
//
void CAknExPbarContainer::ReConstructProgressBarsL()
    {
    //If Progressbars are already created, delete those first.
    if( iPBar[KNumberOfBars - 1] )
        {
        for ( TInt ii = 0; ii < KNumberOfBars; ++ii )
            {
            delete( iPBar[ii] );
            iPBar[ii] = NULL;
            }
        }        
     
    CreateProgressBarsL();
    
    for ( TInt ii = 0; ii < KNumberOfBars; ++ii )
        {
        iPBar[ ii ]->ActivateL();
        }      
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::SetFinalValue()
// Set Final Value to bars. Changes final values between progress bar 1 and 2.
// -----------------------------------------------------------------------------
//
void CAknExPbarContainer::SetFinalValue()
    {    
    if ( iFinalValuesTwisted )
        {
        iPBar[ 0 ]->SetFinalValue( KProgressBar0FinalValue );
        
        iPBar[ 1 ]->SetFinalValue( KProgressBar1FinalValue );
        iFinalValuesTwisted = EFalse;
        }
    else
        {
        iPBar[ 0 ]->SetFinalValue( KProgressBar1FinalValue );
        iPBar[ 1 ]->SetFinalValue( KProgressBar0FinalValue );
        iFinalValuesTwisted = ETrue;
        }

    for ( TInt ii = 0; ii < KNumberOfBars; ++ii )
        iPBar[ ii ]->SetAndDraw( KProgressBarResetValue );   
    }

// -----------------------------------------------------------------------------
// CAknExPbarContainer::ShowInfo()
// Show Bars Info
// -----------------------------------------------------------------------------
//
void CAknExPbarContainer::ShowInfo( TInt aBarNo )
    {
    //Allocate TBuf with constant length.
    TBuf<KLengthOfTempString> str;
    TBuf<KLengthOfTempString> format;

    iCoeEnv->ReadResource( format, R_AKNEXPBAR_FORMATSTRING_INFO );

   str.Format( format, iPBar[ aBarNo ]->Info().iTextType, 
                        iPBar[ aBarNo ]->Info().iSplitsInBlock,
                        iPBar[ aBarNo ]->Info().iFinalValue,
                        iPBar[ aBarNo ]->Info().iWidth, 
                        iPBar[ aBarNo ]->Info().iHeight,
                        iPBar[ aBarNo ]->CurrentValue() );

	CAknInformationNote* note = new (ELeave) CAknInformationNote;
	note->ExecuteLD( str );
    }
    
// -----------------------------------------------------------------------------
// CAknExPbarContainer::ConstructTimerModel()
// Set Timer Model
// -----------------------------------------------------------------------------
void CAknExPbarContainer::ConstructTimerModel()
    {
    iTimerModel.iHundredths = KTimerModelHundredths;
    iTimerModel.iIncrement = KTimerModelIncrement;
    iTimerModel.iRunning = KTimerModelRunning;
    }    

// -----------------------------------------------------------------------------
// CAknExPbarContainer::ConstructTimerL()
// Construct the Timer
// -----------------------------------------------------------------------------
void CAknExPbarContainer::ConstructTimerL()
    {
    iTimer = CAknExPbarTimer::NewL( &iTimerModel, this );
    }
// End of File  
