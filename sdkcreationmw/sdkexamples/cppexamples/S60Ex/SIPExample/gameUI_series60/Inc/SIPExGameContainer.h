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


#ifndef __SIPEXGAMECONTAINER_H__
#define __SIPEXGAMECONTAINER_H__

// INCLUDES
#include <coecntrl.h>
   
// FORWARD DECLARATIONS
class CSIPExEngine;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

/**
*  CSIPExGameContainer  container control class.
*  
*/
class CSIPExGameContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor
    
        /**
        * C++ default constructor
        * @param aEngine The reference to the application engine.
        */
        CSIPExGameContainer( CSIPExEngine& aEngine );
        
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        ~CSIPExGameContainer();

   private: // Functions from base classes

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

       /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

       /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

       /**
        * From CCoeControl,Draw.
        */
        void Draw( const TRect& aRect ) const;

       /**
        * From CCoeControl, HandleControlEventL.
        */
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );
        
        /**
        * From CCoeControl, OfferKeyEvent.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * From CCoeControl, HandleResourceChange.
        */
        void HandleResourceChange( TInt aType );
  
        /**
        * From CCoeControl, MopSupplyObject.
        */
        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
        
        
    private:

        /**
        * Drawing game. Decides if the cursor or board will be drawn.
        * @param aGc    The system's graphics context.
        * @param aRect  The rectangle where to draw.
        */
        void DrawGame(CWindowGc& aGc, const TRect& aRect) const;

        /**
        * Drawing the game board
        * @param aGc    The system's graphics context.
        * @param aRect  The rectangle where to draw.
        */
        void DrawBoard( CWindowGc& aGc, const TRect& aRect ) const;

        /**
        * Drawing the game cursor.
        * @param aGc    The system's graphics context.
        * @param aRect  The rectangle where to draw.
        */
        void DrawCursor(CWindowGc& aGc, const TRect& aRect) const;
        
        /**
        * Calculating game board dimensions.
        * @param aRect    The rectangle available to the game.
        */
        void CalculateBoardDimensions( const TRect& aRect );
        
        
    private: //data
    
        CSIPExEngine& iEngine;
        
        CAknsBasicBackgroundControlContext* iBgContext;
        
        TInt iXOffset;
        TInt iYOffset;
        TInt iBoxW;
        TInt iMidW;
    };

#endif // __SIPEXGAMECONTAINER_H__

// End of File
