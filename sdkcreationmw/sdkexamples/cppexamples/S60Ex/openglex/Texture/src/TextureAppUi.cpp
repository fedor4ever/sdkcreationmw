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
#include "TextureAppUi.h"
#include "TextureContainer.h"

#include <eikmenup.h>
#include <avkon.hrh>
#include "texture.hrh"
#include <Texture.rsg>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CTextureAppUi::ConstructL()
// ?implementation_description
// ----------------------------------------------------------
//
void CTextureAppUi::ConstructL()
{
    BaseConstructL();

    iAppContainer = new (ELeave) CTextureContainer;
    iAppContainer->SetMopParent(this);

    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
}

// ----------------------------------------------------
// CTextureAppUi::~CTextureAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CTextureAppUi::~CTextureAppUi()
{
    if (iAppContainer)
    {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
    }
}

// ------------------------------------------------------------------------------
// CTextureAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CTextureAppUi::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
{
	/* Texts for menu items. */
    if( aResourceId == R_TEXTURE_MENU )
    {
        /* Text for perspective correction. */
        if( iAppContainer->iTexture->iPerspectiveCorrEnabled )
        {
            aMenuPane->SetItemTextL( ETexturePerspective, R_PERSPECTIVE_OFF );
        }
        else
        {
            aMenuPane->SetItemTextL( ETexturePerspective, R_PERSPECTIVE_ON );
        }

        /* Text for texture wrapping.*/
        if( iAppContainer->iTexture->iTextureWrapEnabled )
        {
            aMenuPane->SetItemTextL( ETextureWrapping, R_WRAPPING_OFF );
        }
        else
        {
            aMenuPane->SetItemTextL( ETextureWrapping, R_WRAPPING_ON );
        }

        /* Text for lighting.*/
        if( iAppContainer->iTexture->iLightingEnabled )
        {
            aMenuPane->SetItemTextL( ETextureLights, R_LIGHTS_OFF );
        }
        else
        {
            aMenuPane->SetItemTextL( ETextureLights, R_LIGHTS_ON );
        }
    }
}

// ----------------------------------------------------
// CTextureAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ?implementation_description
// ----------------------------------------------------
//
TKeyResponse CTextureAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
{
    if ( iAppContainer->iTexture->GetState() == CTexture::ERunning )
		{
			if( aType == EEventKeyDown )
			{
			    TUint scan = aKeyEvent.iScanCode;

			    switch(scan)
			    {
			    case(167):
					iAppContainer->iTexture->ToggleBlending();
					return EKeyWasConsumed;
			    default:
					return EKeyWasNotConsumed;
			    }
			}
		}
    return EKeyWasNotConsumed;
}

// ----------------------------------------------------
// CTextureAppUi::HandleCommandL(TInt aCommand)
// ?implementation_description
// ----------------------------------------------------
//
void CTextureAppUi::HandleCommandL(TInt aCommand)
{
    switch ( aCommand )
    {
    case EAknSoftkeyBack:
    case EEikCmdExit:
    {
        Exit();
        break;
    }
    case ETexturePerspective:
    {
        iAppContainer->iTexture->TogglePerspective();
        break;
    }
    case ETextureWrapping:
    {
        iAppContainer->iTexture->ToggleWrapping();
        break;
    }
    case ETextureLights:
        iAppContainer->iTexture->ToggleLighting();
        break;
    default:
        break;
    }
}

// End of File
