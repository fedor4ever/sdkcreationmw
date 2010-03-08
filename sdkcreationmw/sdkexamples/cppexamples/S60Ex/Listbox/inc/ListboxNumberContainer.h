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
* Description: 
*
*/


#ifndef __LISTBOXNUMBERCONTAINER_H__
#define __LISTBOXNUMBERCONTAINER_H__


#include <eiklbo.h>


class CEikColumnListBox;
class CNaviScrollTimer;


/**
*  CListboxNumberContainer class
*  This class is a container for CListboxNumberView. It owns and draws a listbox
*  control of type CAknSingleNumberStyleListBox.
*/
class CListboxNumberContainer : public CCoeControl
    {
    public:

        /**
        * 2nd phase constructor.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Default constructor.
        */
        CListboxNumberContainer();

        /**
        * Destructor.
        */
        ~CListboxNumberContainer();

    public: //New functions

        /**
        * Getter for listbox item count.
        * @return Number of listbox items.
        */
        TInt ItemCount();

    private: // From CCoeControl

        /**
        * Gets the number of controls contained in a compound control.
        * @return The number of component controls contained by this control.
        */
        TInt CountComponentControls() const;

        /**
        * Gets an indexed component of a compound control.
        * @param aIndex The index of the control.
        * @return The component control with an index of aIndex.
        */
        CCoeControl *ComponentControl(TInt aIndex) const;

        /**
        * Responds to changes to the size and position of the contents of this
        * control.
        */
        void SizeChanged();

        /**
        * Handles key events.
        * @param aKeyEvent The key event.
        * @param aType The type of key event: EEventKey, EEventKeyUp or
        *              EEventKeyDown
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
                            TEventCode aType);

    private: // New functions

        /**
        * Constructs listbox from resource, creates scroll bar and sets empty
        * list background text.
        */
        void CreateListboxL();

    public: // New functions

        /**
        * Adds item to the end of the list.
        */
        void AddItemL();

        /**
        * Removes item from the end of the list.
        */
        void RemoveItemL();

    private: // Data

        /**
        * Pointer to listbox control.
        *
        * Owned.
        */
        CEikColumnListBox* iListBox;

        /**
        * Timer for scrolling listbox class name on the navigation pane.
        *
        * Owned
        */
        CNaviScrollTimer* iTimer;

    };

#endif //__LISTBOXNUMBERCONTAINER_H__

// End of File
