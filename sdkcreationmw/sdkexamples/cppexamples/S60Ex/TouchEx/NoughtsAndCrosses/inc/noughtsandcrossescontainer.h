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



#ifndef __NOUGHTSANDCROSSESCONTAINER_H__
#define __NOUGHTSANDCROSSESCONTAINER_H__

// INCLUDES
#include <coecntrl.h>

#include "noughtsandcrossesengine.h"


// FORWARD DECLARATIONS
class CNoughtsAndCrossesView;


// CLASS DECLARATION
/**
*  CNoughtsAndCrossesContainer container control class.
*
*/
class CNoughtsAndCrossesContainer : public CCoeControl,
    public CNoughtsAndCrossesEngine::MObserver
    {
    public: // TYPES
        typedef CNoughtsAndCrossesEngine::TPlayer TPlayer;

    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * @param aRect The rectangle this container will be drawn to.
        * @return Pointer to the created instance of CNoughtsAndCrossesContainer.
        */
        static CNoughtsAndCrossesContainer* NewL(const TRect& aRect,
            CNoughtsAndCrossesView& view, TInt aCursorRow, TInt aCursorColumn);

        /**
        * ~CNoughtsAndCrossesContainer.
        * Destructor.
        */
        virtual ~CNoughtsAndCrossesContainer();

    private: // Constructors

        CNoughtsAndCrossesContainer (CNoughtsAndCrossesView& aView, 
            TInt aCursorRow, TInt aCursorColumn);

        /**
        * ConstructL.
        * 2nd phase constructor.
        * @param aRect The rectangle this container will be drawn to.
        */
        void ConstructL (const TRect& aRect);

    private: // Functions from base classes

        /**
        * From CoeControl, SizeChanged.
        * Called by framework when the view size is changed.
        */
        virtual void SizeChanged();

        /**
        * From CoeControl, CountComponentControls.
        * Returns the number of components owned by this container.
        * @return Number of component controls
        */
        virtual TInt CountComponentControls() const;

        /**
        * From CCoeControl, ComponentControl.
        * Returns pointer to a particular control.
        * @param aIndex Index of wanted control
        * @return Pointer to component control
        */
        virtual CCoeControl* ComponentControl (TInt aIndex) const;

        /**
        * From CCoeControl, Draw.
        * Draws this container to the screen
        * @param aRect The region of the control to be redrawn.
        */
        virtual void Draw (const TRect& aRect) const;

        /**
        * From CCoeControl, OfferKeyEventL.
        * Handles the key events.
        * @return if key-event is consumed, EKeyWasConsumed. Else EKeyWasNotConsumed
        * @param aKeyEvent Key event
        * @param aType Type of key event ( EEventKey, EEventKeyUp or EEventKeyDown )
        */
        virtual TKeyResponse OfferKeyEventL (const TKeyEvent& aKeyEvent, TEventCode aType);

        /**
        * From CoeControl, HandleResourceChange.
        * Called by framework when the view layout is changed.
        */
        virtual void HandleResourceChange(TInt aType);
 
        virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
        
    private: // CNoughtsAndCrossesEngine::MObserver

        virtual void HandleMove (TPlayer aPlayer, TUint aRow, TUint aColumn);

        virtual void HandleWinnerL(TBool aHumanWins, TUint aXRow,
            TUint aXColumn, TUint aZRow, TUint aZColumn);

        virtual void HandleBoardReset();
        
    private: // New functions

        void DrawGrid (const TRect& aRect) const;
		
        void DrawNought (TInt aRow, TInt aColumn) const;

        void DrawCross (TInt aRow, TInt aColumn) const;

        void DrawWinnersLine (TInt aXRow, TInt aXColumn, TInt aYRow, TInt aYColumn) const;

        void DrawCursor (const TRgb& aCursorColour, const TRgb& aReturnColour, 
            TInt aRow, TInt aColumn) const;

    private: // Data

        CNoughtsAndCrossesView& iView;
        TUint iCursorRow;
        TUint iCursorColumn;
    };


#endif // __NOUGHTSANDCROSSESCONTAINER_H__

// End of File
