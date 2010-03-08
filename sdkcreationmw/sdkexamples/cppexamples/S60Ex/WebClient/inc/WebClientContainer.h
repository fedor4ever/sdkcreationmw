/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WEBCLIENTCONTAINER_H
#define WEBCLIENTCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eikrted.h>
#include "WebClientEngine.h"

// CONSTANTS
// Constants for CEikRichTextEditors
const TInt KNumberOfLines = 0;
const TInt KTextLimit = 128;
const TInt KStatusWindowHeight = 34;
const TInt KScrollBarWidth = 10;

// FORWARD DECLARATIONS
class CEikLabel;

// CLASS DECLARATION

/**
*  CWebClientContainer class.
* Applications view contains three CEikRichTextEditors for displaying received
* data and status events.
*
*/
class CWebClientContainer : public CCoeControl,
                            public MCoeControlObserver,
                            public MWebClientObserver
    {
    public:
        /**
        * NewL()
        * Create a CClientEngine object.
        * @param aRect: Extents of the view.
        * @return A pointer to the created instance of CWebClientContainer
        */
        static CWebClientContainer* NewL( const TRect& aRect );

        /**
        * NewLC()
        * Create a CClientEngine object.
        * @param aRect: Extents of the view.
        * @return A pointer to the created instance of CWebClientContainer
        */
        static CWebClientContainer* NewLC( const TRect& aRect );

        /**
        * ~CWebClientContainer()
        * Destructor of CWebClientContainer.
        */
        ~CWebClientContainer();

        /**
        * ResetL()
        * Resets contents of the view.
        * Sets given text to label and clears CEikRichTextEditors.
        * @param aText: Label text.
        */
        void ResetL( const TDesC& aText );

        /**
        * HandleCommandL()
        * Handles user commands.
        * @param aCommand: Command ID.
        */
        void HandleCommandL( TInt aCommand );

    public: // Functions from base classes
        /**
        * From MWebClientObserver (see WebClientEngine.h)
        */
        void ClientEvent( const TDesC& aEventDescription );
        void ClientHeaderReceived( const TDesC& aHeaderData );
        void ClientBodyReceived( const TDesC8& aBodyData );

    private: // Functions from base classes
       /**
        * From CoeControl
        * SizeChanged()
        * Called by framework when the view size is changed.
        */
        void SizeChanged();

        /**
        * From CCoeControl
        * CountComponentControls()
        * Gets the number of controls contained in this compound control. 
        * @return Number of controls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * ComponentControl()
        * Gets the specified component of a compound control. 
        * @param aIndex: 
        * @return Pointer to control with index aIndex.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Draw()
        * From CCoeControl
        * Draws the view.
        * @param 
        *        aRect:
        * @return
        *         -
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl
        * OfferKeyEventL()
        * Handles key events passed to this control (view).
        * @param aKeyEvent: The key event.
        * @param aType:     The type of key event: EEventKey, EEventKeyUp or 
        *                   EEventKeyDown.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From MCoeControlObserver
        * HandleControlEventL()
        * Handles control events.
        * @param aControl: Control
        * @param aEventType: Event type
        */
        void HandleControlEventL( CCoeControl*  /* aControl */, 
                                  TCoeEvent     /* aEventType */ );
    private:
        /**
        * ConstructL()
        * Perform the second phase construction of a CWebClientContainer object.
        * @param aRect: Extents of the view.
        */
        void ConstructL( const TRect& aRect );

        /**
        * CWebClientContainer()
        * Perform the first phase of two phase construction.
        */
        CWebClientContainer();

        /**
        * AddToHeaderWindowL()
        * Adds given text to header window (CEikRichTextEditor, iHeaderWindow)
        * Does not format the added text.
        * @param aText: A single line of text to be added.
        */
        void AddToHeaderWindowL( const TDesC& aText );

        /**
        * AddToSourceWindowL()
        * Adds given text to Source window (CEikRichTextEditor, iSourceWindow)
        * Does not format the added text.
        * @param aText: Text to be added.
        */
        void AddToSourceWindowL( const TDesC8& aText );

        /**
        * AddToStatusWindowL()
        * Adds given text to status window (CEikRichTextEditor, iStatusWindow)
        * @param aText: A single line of text to be added.
        */
        void AddToStatusWindowL( const TDesC& aText );

    private: // Data
        CEikLabel*              iLabel;
        CEikRichTextEditor*     iHeaderWindow;
        CEikRichTextEditor*     iSourceWindow;
        CEikRichTextEditor*     iStatusWindow;

};

#endif // WEBCLIENTCONTAINER_H
