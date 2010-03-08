/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <avkon.hrh>

#include "EnvMappingAppUi.h"
#include "EnvMappingContainer.h"
#include <EnvMapping.rsg>
#include "envmapping.hrh"


// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CEnvMappingAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------
//
void CEnvMappingAppUi::ConstructL()
{
    BaseConstructL();

    iAppContainer = new (ELeave) CEnvMappingContainer;
    iAppContainer->SetMopParent(this);

    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
}

// ----------------------------------------------------
// CEnvMappingAppUi::~CEnvMappingAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CEnvMappingAppUi::~CEnvMappingAppUi()
{
    if ( iAppContainer )
    {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
    }
}

// ------------------------------------------------------------------------------
// CEnvMappingAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CEnvMappingAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CEnvMappingAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CEnvMappingAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
{
    if ( iAppContainer->iEnvMapping->GetState() == CEnvMapping::ERunning )
    {
        if( aType == EEventKeyDown )
        {
            TUint scan = aKeyEvent.iScanCode;

            switch(scan)
            {
            case(14):
                iAppContainer->iEnvMapping->RotateObjectLeft();
                return EKeyWasConsumed;
            case(15):
                iAppContainer->iEnvMapping->RotateObjectRight();
                return EKeyWasConsumed;
            default:
                return EKeyWasNotConsumed;
            }
        }
        else if( aType == EEventKeyUp )
        {
            TUint scan = aKeyEvent.iScanCode;

            switch(scan)
            {
            case(14):
                iAppContainer->iEnvMapping->RotateObjectLeft();
                return EKeyWasConsumed;
            case(15):
                iAppContainer->iEnvMapping->RotateObjectRight();
                return EKeyWasConsumed;
            default:
                return EKeyWasNotConsumed;
            }
        }
    }

    return EKeyWasNotConsumed;
}

// ----------------------------------------------------
// CEnvMappingAppUi::HandleCommandL(TInt aCommand)
// command handler
// ----------------------------------------------------
//
void CEnvMappingAppUi::HandleCommandL(TInt aCommand)
{
  switch ( aCommand )
  {
      case EAknSoftkeyBack:
      case EEikCmdExit:
      {
          Exit();
          break;
      }
      case EEnvMappingDuckObject:
      {
          iAppContainer->iEnvMapping->DuckModel();
          break;
      }
      case EEnvMappingHeadObject:
      {
          iAppContainer->iEnvMapping->HeadModel();
          break;
      }
      default:
          break;
  }
}


// End of File
