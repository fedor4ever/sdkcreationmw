/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include "RichTextEditorAppUi.h"
#include "RichTextEditorContainer.h"
#include <RichTextEditor.rsg>

#include <avkon.hrh> //Font ids

#include "RichTextEditor.hrh"

#define KEnableSkinFlag 0x1000
#define KLayoutAwareApp 0x08

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CRTEAppUi::ConstructL()
//
// ----------------------------------------------------------
//
void CRTEAppUi::ConstructL()
    {

   	BaseConstructL(EAknEnableSkin);

    iAppContainer = new (ELeave) CRTEContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CRTEAppUi::~CRTEAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CRTEAppUi::~CRTEAppUi()
    {
    if (iAppContainer)
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }

// ----------------------------------------------------
// CRTEAppUi::HandleCommandL(TInt aCommand)
//
// ----------------------------------------------------
//
void CRTEAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
		case EEikCmdExit:
            {
            Exit();
            break;
            }
        case ERichTextCmdAppSetbold:
			iAppContainer->SetCharacterBIUAttribL(CEikGlobalTextEditor::EBold);
			break;
		case ERichTextCmdAppSetItalic:
			iAppContainer->SetCharacterBIUAttribL(CEikGlobalTextEditor::EItalic);
			break;
		case ERichTextCmdAppSetUnderline:
			iAppContainer->SetCharacterBIUAttribL(CEikGlobalTextEditor::EUnderline);
            break;
		// Font names defined at AknUtils.h in 1st and 2nd editions.
		// Font ids at avkon.hrh from 3rd Edition onwards.
		case ERichTextCmdAppFont1:
			iAppContainer->SetFont(EAknLogicalFontPrimaryFont);
			break;
		case ERichTextCmdAppFont2:
			iAppContainer->SetFont(EAknLogicalFontSecondaryFont);
			break;
		case ERichTextCmdAppFont3:
			iAppContainer->SetFont(EAknLogicalFontPrimarySmallFont);
			break;
		case ERichTextCmdAppSetStrikethrough:
			iAppContainer->Strike();
			break;
		case ERichTextCmdAppLeft:
			iAppContainer->SetAlignment(CParaFormat::ELeftAlign);
			break;
		case ERichTextCmdAppCenter:
			iAppContainer->SetAlignment(CParaFormat::ECenterAlign);
			break;
		case ERichTextCmdAppRight:
			iAppContainer->SetAlignment(CParaFormat::ERightAlign);
			break;
		case ERichTextCmdAppSetRed:
			iAppContainer->SetColor(KRgbRed);
			break;
		case ERichTextCmdAppSetGreen:
			iAppContainer->SetColor(KRgbGreen);
			break;
		case ERichTextCmdAppSetBlue:
			iAppContainer->SetColor(KRgbBlue);
			break;
		case ERichTextCmdAppSetBlack:
			iAppContainer->SetColor(KRgbBlack);
			break;
		case ERichTextCmdAppDemo:
			iAppContainer->DemoL();
			break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
//  Called by framework when layout is changed.
//  Passes the new client rectangle to the AppContainer
// -----------------------------------------------------------------------------
//
void CRTEAppUi::HandleResourceChangeL( TInt aType )
    {
    // base-class call also
    CAknAppUi::HandleResourceChangeL(aType);
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
    	if(iAppContainer)
    		iAppContainer->SetRect( ClientRect() );
        }
    }

// End of File
