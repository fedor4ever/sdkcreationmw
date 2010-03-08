/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the menu container class.
*
*/


#ifndef DATAMOBILITYAPPMENUCONTAINER_H
#define DATAMOBILITYAPPMENUCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>

// FORWARD DECLARATIONS
class CEikTextListBox;

// CLASS DECLARATION

/**
*  CDataMobilityAppMenuContainer  container control class.
*
*/
class CDataMobilityAppMenuContainer : public CCoeControl, public MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CDataMobilityAppMenuContainer();

    public: // New functions

        /**
        * Get currently selected items index.
        * @return Current item index.
        */
        TInt GetActiveLine() const;

        /**
        * Method HandleSelectedListItemL handles valid index.
        * @param aIndex Current item index.
        */
        void HandleSelectedListItemL( TInt aIndex ) const;

    private: // Functions from base classes

        /**
        * From CoeControl,SizeChanged.
        * Called by framework when the view size is changed.
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls.
        * @return Number of controls indside this container.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        * Gets the specified component of a compound control.
        * @param aIndex The index of the control to get.
        * @return The component control with an index of aIndex.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * Handles list box events.
        * @param aListBox The originating list box.
        * @param aEventType A code for the event. Further information may be
        *                   obtained by accessing the list box itself.
        */
        void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);

        /**
        * From CCoeControl,OfferKeyEventL
        * Handles key events.
        * @param aKeyEvent The key event.
        * @param aType The type of key event.
        * @return Indicates whether or not the key event was used
        *         by this control.
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

        /**
        * From CCoeControl,OfferKeyEventL
        * Called by framework when the view layout is changed.
        * @param aType The type of resource change.
        */
        virtual void HandleResourceChange(TInt aType);

    private: //data

        CEikTextListBox*    iListBox;
    };

#endif  // DATAMOBILITYAPPMENUCONTAINER_H

// End of File
