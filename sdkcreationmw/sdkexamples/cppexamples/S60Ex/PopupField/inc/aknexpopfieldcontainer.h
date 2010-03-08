/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __AKNEXPOPFIELDCONTAINER_H__
#define __AKNEXPOPFIELDCONTAINER_H__

// INCLUDES
#include <coecntrl.h>


// CONSTANTS
const TInt KAknExPopFieldCountComponent = 1;
const TInt KAknExPopFieldMessageBufLength = 256;

// FORWARD DECLARATIONS
class CAknExPopFieldView;
class CEikLabel;

// CLASS DECLARATION

/**
* CAknExPopFieldContainer container control class.
*/
class CAknExPopFieldContainer : public CCoeControl
    {
    
    public: // Constructors and destructor

        /**
        * CAknExPopFieldContainer
        * C++ default constructor.
        * @param aView Pointer to view class object.
        */
        CAknExPopFieldContainer( CAknExPopFieldView* aView );

        /**
        * ConstructL
        * 2nd phase constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * CAknExPopFieldContainer
        * Virtual Destructor.
        */
        virtual ~CAknExPopFieldContainer();

    public: // New functions

        /**
        * SetTextToLabelL
        * Sets text to label        
        * @param aResourceId Resource's Id
        */
        void SetTextToLabelL( TInt aResourceId );

        /**
        * DisplayTextPopFieldL
        * Display Text with PopField
        * @param aResourceId
        */
        void DisplayTextPopFieldL( TInt aResourceId );

        /**
        * DisplayOptionalTextPopFieldL
        * @param aResourceId
        */
        void DisplayOptionalTextPopFieldL( TInt aResourceId );

        /**
        * DisplayManyLineTextPopFieldL
        * @param aResourceId
        */
        void DisplayManyLineTextPopFieldL( TInt aResourceId );

        /**
        * DisplayMultiPagePopFieldL
        * @param aResourceId
        */
        void DisplayMultiPagePopFieldL( TInt aResourceId );

        /**
        * DisplaySetAttribsPopFieldL
        * @param aResourceId
        */
        void DisplaySetAttribsPopFieldL( TInt aResourceId );

    private: // New functions

        /**
        * CreateLabelL
        * Creates the label object.
        */
        void CreateLabelL();


    private: // From CCoeControl

        /**
        * From CCoeControl, OfferKeyEventL.
        * Handles the key events.
        * @param aKeyEvent Key event
        * @param aType Type of event(EEventKey, EEventKeyUp or EEventKeyDown)
        * @return if key-event consumed, EKeyWasConsumed Else EKeyWasNotConsumed
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**

        * From CCoeControl, SizeChanged.
        * Object's Size Changed
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
        * @param aRect Region of the control to be (re)drawn.
        */
        void Draw( const TRect& aRect ) const;


    private: // Enumerations

        /**
        * TAknExPopFieldComponentControls
        * Enumeration
        */

        enum TAknExPopFieldComponentControls
            {
            EAknExPopFieldComponentLabel
            };


    private: // Data

        /**
        * iView
        * Not Owned by CAknExPopFieldContainer object.
        */
        CAknExPopFieldView* iView;

        /**
        * iLabel
        * Owned by CAknExPopFieldContainer object.
        */
        CEikLabel* iLabel;

    };

#endif  //__AKNEXPOPFIELDCONTAINER_H__
