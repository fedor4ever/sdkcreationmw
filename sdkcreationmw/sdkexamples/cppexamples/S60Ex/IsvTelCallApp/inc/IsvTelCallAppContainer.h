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


#ifndef ISVTELCALLAPPCONTAINER_H
#define ISVTELCALLAPPCONTAINER_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CIsvTelCallAppView;
class CEikLabel;

// CLASS DECLARATION
/**
* CIsvTelCallAppContainer container control class.
*/
class CIsvTelCallAppContainer : public CCoeControl
    {
    private: // Enumerations
        enum TIsvTelCallAppComponentControls
            {
            EIsvTelCallAppComponentLabel
            };

    public: // Constructors and destructor
        /**
        * Overload constructor.
        * @param aView Pointer to view class object.
        */
        CIsvTelCallAppContainer( CIsvTelCallAppView* aView );

        /**
        * EPOC constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        virtual ~CIsvTelCallAppContainer();

    public: // New functions
        /**
        * Sets text from a resource to a label
        * @param aResourceId
        */
        void SetTextToLabelL( TInt aResourceId );

        /**
        * Creates and executes the form
        * @param aResourceId , Id of resource for the form
        * @param aPageId, Id of the Page to start with
        */
        void DisplayFormL( TInt aResourceId, TInt aPageId );
        void BringToForeGroundL( );

    private: // New functions
        /**
        * Creates the label object.
        */
        void CreateLabelL();

    private: // From CCoeControl
        /**
        * From CCoeControl, OfferKeyEventL.
        * Handles the key events.
        * @param aKeyEvent Key event
        * @param aType Type of key event (EEventKey, EEventKeyUp 
        *         or EEventKeyDown)
        * @return if key-event is consumed, EKeyWasConsumed. 
        *         Else EKeyWasNotConsumed
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

        /**
        * From CCoeControl, SizeChanged.
        *
        */
        void SizeChanged();

        /**
        * From CCoeControl, CountComponentControls.
        * Returns number of component.
        * @return Number of controls
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl, ComponentControl.
        * Returns pointer to particular component.
        * @return Pointer to Control's component control, identified by index
        * @param aIndex Index of the control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl, Draw.
        *
        * @param aRect Region of the control to be (re)drawn.
        */
        void Draw( const TRect& aRect ) const;

    private: // Data
        CIsvTelCallAppView* iView;
        CEikLabel* iLabel;
    };

#endif //ISVTELCALLAPPCONTAINER_H
// End of File