/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


// System includes
#include <eikgted.h>
#include <f32file.h>
#include <txtglobl.h>
#include <aknutils.h>
#include <OcrExample.rsg> // resources

#include "OcrExampleTextContainer.h"
#include "OcrExampleUids.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

COcrExampleTextContainer::COcrExampleTextContainer(TTextContainerData& aData)
    : iEditor(0)
    , iData(aData)
    {
    }

void COcrExampleTextContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    // construct editor without resource
    iEditor = new (ELeave) CEikGlobalTextEditor;

    // to get the scrollbar created automatically (based on the flag below)
    // we have to set avkon-flags *before* ConstructL-call
    //
    iEditor->SetAknEditorFlags( EAknEditorFlagEnableScrollBars
                                | EAknEditorFlagNoLRNavigation);
    iEditor->ConstructL(
        this,
        0,  // number of lines
        0,  // text limit
        (EEikEdwinReadOnly | EEikEdwinNoAutoSelection), // edwin flags
        EGulFontControlAll, // font control flags
        EGulAllFonts ); // font name flags

    iEditor->SetFocus(ETrue);
    SetRect(aRect);
    }

COcrExampleTextContainer* COcrExampleTextContainer::NewL(const TRect& aRect,
                                                         TTextContainerData& aData)
    {
    COcrExampleTextContainer* self = new (ELeave) COcrExampleTextContainer(aData);
    self->ConstructL(aRect);
    return self;
    }

COcrExampleTextContainer::~COcrExampleTextContainer()
    {
    delete iEditor;
    }

void COcrExampleTextContainer::SizeChanged()
    {
    TSize size = Size();

    // we should have active scrollbar, but let's do paranoia check anyway
    if (iEditor->ScrollBarFrame())
        {
        size.iWidth -= iEditor->ScrollBarFrame()->VerticalScrollBar()->ScrollBarBreadth();
        }
    iEditor->SetExtent(TPoint(0,0), size);
    }

void COcrExampleTextContainer::ActivateL()
    {
    ReadTextFromFileL();
    CCoeControl::ActivateL();
    AdjustCursorL(); // has to be done after activation
    }

TInt COcrExampleTextContainer::CountComponentControls() const
    {
    return 1; // return number of controls inside this container
    }

CCoeControl* COcrExampleTextContainer::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            return iEditor;
        default:
            return NULL;
        }
    }

void COcrExampleTextContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.Clear(aRect);
    }

TKeyResponse COcrExampleTextContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if (iEditor)
        {
        if (aKeyEvent.iCode == EKeyDownArrow)
            {
            iEditor->MoveCursorL(TCursorPosition::EFLineDown, EFalse);
            return EKeyWasConsumed;
            }
        else if (aKeyEvent.iCode == EKeyUpArrow)
            {
            iEditor->MoveCursorL(TCursorPosition::EFLineUp, EFalse);
            return EKeyWasConsumed;
            }
		else
		    {
			return iEditor->OfferKeyEventL(aKeyEvent, aType);
		    }
        }
    else
        return CCoeControl::OfferKeyEventL(aKeyEvent, aType);
    }

void COcrExampleTextContainer::ReadTextFromFileL()
    {
	TFileName file(KLogFile);
    iEditor->InsertFromTextFileL(file, CPlainText::EOrganiseByParagraph);
    }

void COcrExampleTextContainer::AdjustCursorL()
    {
    iEditor->SetCursorPosL(iData.iStartPosition, EFalse);
    iData.iStartPosition = iEditor->TextLength();
    }

// End of File
