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


#ifndef __LISTBOXGRAPHICCONTAINER_H__
#define __LISTBOXGRAPHICCONTAINER_H__


#include <eiklbo.h>
#include <akniconutils.h>

class CEikColumnListBox;
class CIconFileProvider;
class CNaviScrollTimer;


/**
*  CListboxGraphicContainer class
*  This class is a container for CListboxGraphicView. It owns and draws a listbox
*  control of type CAknSingleGraphicStyleListBox.
*/
class CListboxGraphicContainer : public CCoeControl
    {
    public:

        /**
        * 2nd phase constructor.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Default constructor.
        */
        CListboxGraphicContainer();

        /**
        * Destructor.
        */
        ~CListboxGraphicContainer();

    public:

        /**
        * Adds item to the listbox. Asks the item text from user with
        * data query. Item is added to the index which is currently focused.
        */
        void AddItemL();

        /**
        * Removes focused item from the listbox.
        */
        void RemoveItemL();

        /**
        * Getter for listbox item count.
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
        * Constructs listbox, creates scroll bar, sets empty
        * list background text, and enables marquee.
        */
        void CreateListboxL();

        /**
        * Sets icons for the listbox. Only one icon is used and loaded from
        * application private directory using CIconFileProvider and
        * AknIconUtils.
        */
        void SetIconsL();

        /**
        * Sets items to the listbox.
        */
        void SetItemsL();

    private: // Data

        /**
        * Pointer to listbox control.
        *
        * Owned.
        */
        CEikColumnListBox* iListBox;

        /**
        * File server.
        */
        RFs iFsSession;

        /**
        * Icon file provider. With this it is possible to open a icon file
        * from application private directory.
        *
        * Owned.
        */
        CIconFileProvider* iFileProvider;

        /**
        * Timer for scrolling listbox class name on the navigation pane.
        *
        * Owned
        */
        CNaviScrollTimer* iTimer;

    };

#endif //__LISTBOXGRAPHICCONTAINER_H__

// End of File
