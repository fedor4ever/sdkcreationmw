/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handle user interaction. CAknViewAppUi view handling methods added to
*                CAknAppUi.
*                HandleCommandL has (menu+softkey) commands applicable for the entire
*                application.
*                In HandleKeyEvent the application switches between view 1 and view 2.
*                Navipane tabs are not automatically handled, they need to be
*                taken care of separately.
*                HandleForeGroundEventL is called when the application view loses or
*                gains focus.
*                DynInitMenuPaneL allows you to manipulate menu items dynamically.
*                Not used in this example.
*                InfoMessages work only in the emulator, not on the device.
*
*/


// INCLUDE FILES
#include "MyViewAppUi.h"
#include "MyViewView.h"
#include "MyViewView2.h"
#include <MyView.rsg>
#include "myview.hrh"
#include "MSVSTD.HRH"
#include <avkon.hrh>


// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CMyViewAppUi::ConstructL()
// ----------------------------------------------------------
//
void CMyViewAppUi::ConstructL()
    {
    //Initialises this app UI with standard values.
    //The application’s standard resource file will be read unless
    //the ENoAppResourceFile or ENonStandardResourceFile flags are passed.
    BaseConstructL(0x08 | EAknEnableSkin); // Use ELayoutAwareAppFlag (0x08) to make the application support scalable UI on FP3 devices.


    // Show tabs for main views from resources
    CEikStatusPane* sp = StatusPane();

    // Fetch pointer to the default navi pane control
    iNaviPane = (CAknNavigationControlContainer*)sp->ControlL(
        TUid::Uid(EEikStatusPaneUidNavi));

    // Tabgroup has been read from resource and it was pushed to the navi pane.
    // Get pointer to the navigation decorator with the ResourceDecorator() function.
    // Application owns the decorator and it has responsibility to delete the object.
    iDecoratedTabGroup = iNaviPane->ResourceDecorator();
    if (iDecoratedTabGroup)
        {
        iTabGroup = (CAknTabGroup*) iDecoratedTabGroup->DecoratedControl();
        }

    CMyViewView* view1 = new (ELeave) CMyViewView;

    CleanupStack::PushL( view1 );
    view1->ConstructL(iTabGroup);
    AddViewL( view1 );        // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view1 );
    iViewId1 = view1->Id();   // view id to get view from CAknViewAppUi


    CMyViewView2* view2 = new (ELeave) CMyViewView2;

    CleanupStack::PushL( view2 );
    view2->ConstructL();
    AddViewL( view2 );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( view2 );
    iViewId2 = view2->Id(); // view id to get view from CAknViewAppUi


    SetDefaultViewL( *view1 );
    }

// ----------------------------------------------------
// CMyViewAppUi::~CMyViewAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CMyViewAppUi::~CMyViewAppUi()
    {
    delete iDecoratedTabGroup;
    }

// ------------------------------------------------------------------------------
// CMyViewAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CMyViewAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }


// ------------------------------------------------------------------------------
// CMyViewAppUi::HandleForegroundEventL(TBool aForeground)
//  This function is called by the  framework when the screen loses or gains focus.
//   i.e. when it goes to the background or to the foreground. Incoming call
//   softnote is an example.
// This event applies to the entire application, all views.
// ------------------------------------------------------------------------------
//
void CMyViewAppUi::HandleForegroundEventL(TBool aForeground)
    {
    if (aForeground==TRUE)
    iEikonEnv->InfoMsg(_L("Foreground true"));
    else
    iEikonEnv->InfoMsg(_L("Foreground false"));

    }



// ----------------------------------------------------
// CMyViewAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// Here we handle key events: Right and left arrow key
//   to change view.
// ----------------------------------------------------
//
TKeyResponse CMyViewAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
    {
    if ( iTabGroup == NULL )
        {
        return EKeyWasNotConsumed;
        }

    TInt active = iTabGroup->ActiveTabIndex();
    TInt count = iTabGroup->TabCount();

    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
            if ( active > 0 )
                {
                active--;
                iTabGroup->SetActiveTabByIndex( active );
                // ActivateLocalViewL() is used to change the view.
                // To change view from another application we would use ActivateViewL()
                // Send an empty message
                ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
                }
            break;
        case EKeyRightArrow:
            if( (active + 1) < count )
                {
                active++;
                iTabGroup->SetActiveTabByIndex( active );
                // ActivateLocalViewL() is used to change the view.
                // To change view from another application we would use ActivateViewL()
                ActivateLocalViewL(TUid::Uid(iTabGroup->TabIdFromIndex(active)));
                }
            break;
        default:
            return EKeyWasNotConsumed;
        }

    return EKeyWasConsumed;
    }

// ----------------------------------------------------
// CMyViewAppUi::HandleCommandL(TInt aCommand)
// Here we handle commands on the application level.
// In addition, each view has their own HandleCommandL()
// ----------------------------------------------------
//
void CMyViewAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        // You can add your all application applying commands here.
        // You would handle here menu commands that are valid for all views.
        }
    }

// -----------------------------------------------------------------------------
// CChatAppUi::HandleResourceChangeL( TInt aType )
// Called by framework when layout is changed.
// -----------------------------------------------------------------------------
//
void CMyViewAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
		((CMyViewView*) View( iViewId1) )->HandleClientRectChange(  );
		((CMyViewView2*) View( iViewId2) )->HandleClientRectChange(  );
		}

    }

// End of File
