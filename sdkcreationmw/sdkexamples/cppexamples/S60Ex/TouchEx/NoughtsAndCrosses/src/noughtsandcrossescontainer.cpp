/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
#include <aknviewappui.h>
#include <AknUtils.h>

#include "noughtsandcrossescontainer.h"
#include "noughtsandcrossesdocument.h"
#include "noughtsandcrossesinformationandsettings.h"
#include "noughtsandcrossesview.h"

// CONSTANTS
const TInt KMargin=5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAknExGridContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesContainer* CNoughtsAndCrossesContainer::NewL(
    const TRect& aRect, CNoughtsAndCrossesView& aView, TInt aCursorRow, TInt aCursorColumn)
    {
    CNoughtsAndCrossesContainer *self = new (ELeave) CNoughtsAndCrossesContainer(
        aView, aCursorRow, aCursorColumn);
    CleanupStack::PushL (self);
    self->ConstructL (aRect);
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::CNoughtsAndCrossesContainer()
// Default constructor.
// ----------------------------------------------------------------------------
//
CNoughtsAndCrossesContainer::CNoughtsAndCrossesContainer(
    CNoughtsAndCrossesView& aView, TInt aCursorRow, TInt aCursorColumn) :
    iView (aView), iCursorRow (aCursorRow), iCursorColumn (aCursorColumn)
    {
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    SetRect(aRect);
    ActivateL();
    HandleBoardReset();
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::~CNoughtsAndCrossesContainer()
// Destructor.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesContainer::~CNoughtsAndCrossesContainer()
    {
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::SizeChanged()
    {
    }

// ---------------------------------------------------------
// CNoughtsAndCrossesSettingListbox::HandleResourceChange()
// Called by framework when layout is changed.
// ---------------------------------------------------------
//
void CNoughtsAndCrossesContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
        }
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::CountComponentControls()
// Returns number of components.
// -----------------------------------------------------------------------------
//
TInt CNoughtsAndCrossesContainer::CountComponentControls() const
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::ComponentControl()
// Returns pointer to a particular component.
// -----------------------------------------------------------------------------
//
CCoeControl* CNoughtsAndCrossesContainer::ComponentControl (TInt /*aIndex*/) const
    {
    return NULL;	
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::Draw()
// Draw function.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    
    gc.SetBrushColor(KRgbWhite);
    gc.SetBrushStyle (CGraphicsContext::ESolidBrush);
    gc.DrawRect (aRect);

    DrawGrid (aRect);
    DrawCursor (KRgbBlue, KRgbBlack, iCursorRow, iCursorColumn);

    CNoughtsAndCrossesDocument* document = static_cast<
        CNoughtsAndCrossesDocument*> (iView.AppUi()->Document());

    const CNoughtsAndCrossesDocument::TBoard& board = document->Board();

    for (TInt i = 0; i < BOARD_SIZE; ++i)
        {
        for (TInt j = 0; j < BOARD_SIZE; ++j)
            {
            if (board[i][j] == CNoughtsAndCrossesEngine::ENought)
                {
                DrawNought (i, j);
                }
            else if (board[i][j] == CNoughtsAndCrossesEngine::ECross)
                {
                DrawCross (i, j);
                }
            }
        }

    if (iView.iWinningRowX != -1)
        {
        DrawWinnersLine (iView.iWinningRowX, iView.iWinningColumnX, 
        	iView.iWinningRowY, iView.iWinningColumnY);
        }
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::OfferKeyEventL()
// Handles keyevents.
// ---------------------------------------------------------------------------
//
TKeyResponse CNoughtsAndCrossesContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if (aType != EEventKey)
        {
        return EKeyWasNotConsumed;
        }

    if (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK)
        {
        CNoughtsAndCrossesDocument* document = static_cast<
             CNoughtsAndCrossesDocument*> (iView.AppUi()->Document());

        if (!document->CanMove())
            {
            return EKeyWasConsumed;
            }

        if (document->MakeHumanMoveL(iCursorRow, iCursorColumn))
            {
            if (document->CanMove())
                {
                document->MakeComputerMove();
                }
            }	
        return  EKeyWasConsumed;
        }
    else if (aKeyEvent.iCode == EKeyUpArrow && iCursorRow > 0)
        {
        --iCursorRow;
        DrawDeferred();
        return EKeyWasConsumed;
        }
    else if (aKeyEvent.iCode == EKeyDownArrow && iCursorRow < (BOARD_SIZE-1))
        {
        ++iCursorRow;
        DrawDeferred();
        return EKeyWasConsumed;
        }
    else if (aKeyEvent.iCode == EKeyLeftArrow && iCursorColumn > 0)
        {
        --iCursorColumn;
        DrawDeferred();
        return EKeyWasConsumed;
        }
    else if (aKeyEvent.iCode == EKeyRightArrow && iCursorColumn < (BOARD_SIZE-1))
        {
        ++iCursorColumn;
        DrawDeferred();
        return EKeyWasConsumed;
        }	

    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::HandleMove()
// 
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::HandleMove(TPlayer /*aPlayer*/, TUint /*aRow*/,
    TUint /*aColumn*/)
    {
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::HandleWinner()
// 
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::HandleWinnerL(TBool aHumanWins, TUint aXRow,
    TUint aXColumn, TUint aZRow, TUint aZColumn)
    {
    CNoughtsAndCrossesDocument* document = static_cast<
         CNoughtsAndCrossesDocument*> (iView.AppUi()->Document());

    if (aHumanWins)
        {
        document->InformationAndSettings().AddHumanWins();
        }
    else
        {
        document->InformationAndSettings().AddHumanLosses();
        }

    document->SaveSettingsAndInformationL();

    iView.iWinningRowX = aXRow;
    iView.iWinningColumnX = aXColumn;
    iView.iWinningRowY = aZRow;
    iView.iWinningColumnY = aZColumn;
	
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::HandleBoardReset()
// 
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::HandleBoardReset()
    {
    iView.ResetCoordinates();
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::DrawGrid()
// 
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::DrawGrid(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    TPoint point1;
    TPoint point2;

    TInt i;

    // Draw horizontal lines
    for (i=1 ; i<=BOARD_SIZE ; ++i)
        {
        point1.SetXY (aRect.Width() * i / BOARD_SIZE, 0);
        point2.SetXY (aRect.Width() * i / BOARD_SIZE, aRect.Height());	
        gc.DrawLine (point1, point2);
        }

    // Draw vertical lines
    for (i=1 ; i<=BOARD_SIZE ; ++i)
        {
        point1.SetXY (0, aRect.Height() * i / BOARD_SIZE);
        point2.SetXY (aRect.Width(), aRect.Height() * i / BOARD_SIZE);	
        gc.DrawLine (point1, point2);
        }
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::DrawNought()
// 
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::DrawNought(TInt aRow, TInt aColumn) const
    {
    TRect viewRect(Rect());
    TRect ellipseRect(
        TPoint ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin, 
        	(viewRect.Height() *  aRow / BOARD_SIZE) + KMargin),
        TSize ( (viewRect.Width() / BOARD_SIZE) - KMargin*2, 
                (viewRect.Height() / BOARD_SIZE) - KMargin*2) );

    CWindowGc& gc = SystemGc();
    gc.DrawEllipse (ellipseRect);
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::DrawCross()
// 
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::DrawCross(TInt aRow, TInt aColumn) const
    {
    CWindowGc& gc = SystemGc();
    TRect viewRect (Rect());
    TPoint point1;
    TPoint point2;

    point1.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin, 
    	(viewRect.Height() * aRow / BOARD_SIZE) + KMargin);
    point2.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin);

    gc.DrawLine (point1, point2);

    point1.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin, 
    	(viewRect.Height() * aRow / BOARD_SIZE) + KMargin);
    point2.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin);

    gc.DrawLine (point1, point2);
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::DrawWinnersLine()
// 
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::DrawWinnersLine(TInt aXRow, TInt aXColumn,
    TInt aYRow, TInt aYColumn) const
    {
    CWindowGc& gc = SystemGc();
    TRect viewRect (Rect());
    TPoint point1;
    TPoint point2;
    
    // Draw row line
    if (aXRow == aYRow)
        {
        point1.SetXY (0, viewRect.Height() * (2 * aYRow + 1) / (BOARD_SIZE * 2));
        point2.SetXY (viewRect.Width(), viewRect.Height() * (2 * aYRow + 1) / (BOARD_SIZE * 2));
        }
    // Draw column line
    else if (aXColumn == aYColumn)
        {        
        point1.SetXY (viewRect.Width() * (2 * aYColumn + 1) / (BOARD_SIZE * 2), 0);
        point2.SetXY (viewRect.Width() * (2 * aYColumn + 1) / (BOARD_SIZE * 2), viewRect.Height());
        }
    // Draw diagonal line
    else
        {
        point1.SetXY (viewRect.Width() * (aXRow / 2), viewRect.Height() * (aXColumn / 2));
        point2.SetXY (viewRect.Width() * (aYRow / 2), viewRect.Height() * (aYColumn / 2));
        }

    gc.DrawLine (point1, point2);	
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesContainer::DrawCursor()
// 
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesContainer::DrawCursor(const TRgb& aCursorColour,
    const TRgb& aReturnColour, TInt aRow, TInt aColumn) const
    {
    CWindowGc& gc = SystemGc();
    TRect viewRect (Rect());
    TPoint point1;
    TPoint point2;

    gc.SetPenColor (aCursorColour);

    // Top left
    point1.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin, 
    	(viewRect.Height() * aRow / BOARD_SIZE) + KMargin);
    point2.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin*2, 
    	(viewRect.Height() * aRow / BOARD_SIZE) + KMargin);

    gc.DrawLine (point1, point2);

    point1.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin, 
   		(viewRect.Height() * aRow / BOARD_SIZE) + KMargin);
    point2.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin, 
    	(viewRect.Height() * aRow / BOARD_SIZE) + KMargin*2);

    gc.DrawLine (point1, point2);

    // Top right
    point1.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin, 
   		(viewRect.Height() * aRow / BOARD_SIZE) + KMargin);
    point2.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin*2, 
    	(viewRect.Height() * aRow / BOARD_SIZE) + KMargin);

    gc.DrawLine (point1, point2);

    point1.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin, 
    	(viewRect.Height() * aRow / BOARD_SIZE) + KMargin);
    point2.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin, 
    	(viewRect.Height() * aRow / BOARD_SIZE) + KMargin*2);

    gc.DrawLine (point1, point2);

    // Bottom left
    point1.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin);
    point2.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin*2, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin);

    gc.DrawLine (point1, point2);

    point1.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin);
    point2.SetXY ((viewRect.Width() * aColumn / BOARD_SIZE) + KMargin, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin*2);

    gc.DrawLine (point1, point2);

    // Bottom right
    point1.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin);
    point2.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin*2, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin);

    gc.DrawLine (point1, point2);

    point1.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin);
    point2.SetXY ((viewRect.Width() * (aColumn + 1) / BOARD_SIZE) - KMargin, 
    	(viewRect.Height() * (aRow + 1) / BOARD_SIZE) - KMargin*2);

    gc.DrawLine (point1, point2);

    gc.SetPenColor (aReturnColour);
    }


void CNoughtsAndCrossesContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    // Check if touch is enabled or not
    if( !AknLayoutUtils::PenEnabled() )
        {
        return;
        }
    
    if (aPointerEvent.iType == TPointerEvent::EButton1Up)
        {
        CNoughtsAndCrossesDocument* document = static_cast<
        CNoughtsAndCrossesDocument*> (iView.AppUi()->Document());

        TRect boardRect = Rect();
        TInt verticalTileSize = boardRect.Size().iWidth / BOARD_SIZE;
        TInt horisontalTileSize = boardRect.Size().iHeight / BOARD_SIZE;
        
        iCursorRow = aPointerEvent.iPosition.iY / horisontalTileSize;
        iCursorColumn = aPointerEvent.iPosition.iX / verticalTileSize;
        
        if (!document->CanMove())
            {
            return;
            }
        
        if (document->MakeHumanMoveL(iCursorRow, iCursorColumn))
            {
            if (document->CanMove())
                {
                document->MakeComputerMove();
                }
            }    
        }

    // Call base class HandlePointerEventL()
    CCoeControl::HandlePointerEventL(aPointerEvent);
    }




// End of File
