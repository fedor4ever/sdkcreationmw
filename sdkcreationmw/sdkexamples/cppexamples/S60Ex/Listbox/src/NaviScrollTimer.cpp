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


#include <aknnavi.h> 
#include <aknnavide.h> 
#include <eikspane.h> 
#include <aknutils.h>
#include "NaviScrollTimer.h"

// This is the maximum length for visible text in navigation pane
const TInt KMaxVisibleStringLenth = 19;
// Following timeout is used on scrolling starts from the beginning (after text 
// is scrolled totally to left). Timeout is in microseconds. 
const TInt KScrollStartTimeout = 1000000;
// Timeout for scrolling (in microseconds). 
const TInt KScrollTimeout = 90000;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNaviScrollTimer::CNaviScrollTimer()
// C++ Default constructor.
// -----------------------------------------------------------------------------
//
CNaviScrollTimer::CNaviScrollTimer() 
    : CTimer( CActive::EPriorityStandard )
    {        
    }

// -----------------------------------------------------------------------------
// CNaviScrollTimer::~CNaviScrollTimer()
// Destructor
// -----------------------------------------------------------------------------
//    
CNaviScrollTimer::~CNaviScrollTimer()
    {
    Cancel();   
    delete iNaviText;
    
    if( iDecorator )
        {
        if( iNaviPane ) iNaviPane->Pop( iDecorator );   
        delete iDecorator;
        }
    }

// -----------------------------------------------------------------------------
// CNaviScrollTimer::ConstructL()
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNaviScrollTimer::ConstructL( const TDesC& aNavitext )
    {
    CActiveScheduler::Add( this );
    CTimer::ConstructL(); 
    
    iNaviText = aNavitext.AllocL();
    iCount = iNaviText->Des().Length();
    
    // Get the navigation pane
    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    iNaviPane = static_cast<CAknNavigationControlContainer*>(
        sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );         
    }
  
// -----------------------------------------------------------------------------
// CNaviScrollTimer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CNaviScrollTimer* CNaviScrollTimer::NewL( const TDesC& aNavitext )
    {
    CNaviScrollTimer* self = new ( ELeave ) CNaviScrollTimer();
    CleanupStack::PushL( self );
    self->ConstructL( aNavitext );
    CleanupStack::Pop( self );
    return self;    
    }

// -----------------------------------------------------------------------------
// CNaviScrollTimer::RunL()
// After completed timer period, execution comes to RunL.
// -----------------------------------------------------------------------------
//  
void CNaviScrollTimer::RunL()
    {    
    TInt timeout;
 
    if( UpdateNaviPaneL() )
        timeout = KScrollStartTimeout;
    else 
        timeout = KScrollTimeout;
    
    After( timeout );
    }

// -----------------------------------------------------------------------------
// CNaviScrollTimer::StartScroll()
// Starts to scroll the navigation pane text.
// -----------------------------------------------------------------------------
//
void CNaviScrollTimer::StartScroll()
    {   
    // Start scrolling only if text do not fit to navi pane 
    if( iNaviText->Des().Length() > KMaxVisibleStringLenth ) 
        After( 1 );
    else
        UpdateNaviPaneL();
    }

// -----------------------------------------------------------------------------
// CNaviScrollTimer::UpdateNaviPaneL()
// Updates the Navigation pane. Every time when this function is called
// it moves the text to the left by one character.
// -----------------------------------------------------------------------------
//  
TBool CNaviScrollTimer::UpdateNaviPaneL()
    {
    TBool returnVal( EFalse );
    // Return ETrue if text is scrolled totally to left
    if( iCount == iNaviText->Des().Length() )
        returnVal = ETrue;
    
    // NaviLabel can only be updated by creating a new navidecorator.
    if( iDecorator )
        {
        iNaviPane->Pop( iDecorator );
        delete iDecorator;
        iDecorator = NULL;
        }
    // Create new decorator    
    iDecorator = iNaviPane->CreateNavigationLabelL();
    // Push decorator object to the navigation pane's object stack.
    iNaviPane->PushL( *iDecorator );    
    // Get the control inside of decorator object as CAknNaviLabel.
    CAknNaviLabel* naviLabel = 
            static_cast<CAknNaviLabel*>( iDecorator->DecoratedControl() );
    // Finally set label text
    naviLabel->SetTextL( iNaviText->Des().Right(iCount) ); 
    
    iCount--;
    if(iCount == 0) 
        iCount = iNaviText->Des().Length();
    
    return returnVal;    
    }
