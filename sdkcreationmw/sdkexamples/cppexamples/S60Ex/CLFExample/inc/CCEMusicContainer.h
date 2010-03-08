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



#ifndef __CCEMUSICCONTAINER_H__
#define __CCEMUSICCONTAINER_H__

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CAknView;
class CAknDoubleStyleListBox;

// CLASS DECLARATION

/**
*  Container for music view.
*/
class CCEMusicContainer 
    : public CCoeControl
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
		* @param aParent Parent view of the container
        */
        CCEMusicContainer( CAknView* aParent );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CCEMusicContainer();

    public:     // New functions

        /**
        * Append a new list box item with song name, artist and album to the 
        * list box of group listing.
        * @param aTextItems Array that contains descriptors for song name, 
        *        artist and album
        */
		void AppendListBoxItemL( CDesCArray* aTextItems );

        /**
        * Clear the list box of group listing.
        */
		void ResetListBoxL();

        /**
        * Get index of the item that is currently selected in the list box.
        * @return Index of selected item
        */
		TInt CurrentItemIndex();

        /**
        * Selects specified item of the list box.
        * @param aIndex Index of the item to select
        */
		void SetCurrentItemIndex( TInt aIndex );

    protected:     // Functions from base classes

        /**
        * From CCoeControl. Draw the screen.
        */
        void Draw( const TRect& aRect ) const;
        
        /**
        * From CCoeControl. Get count of component controls in the container.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl. Get component control by its index.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl. Handle change of the screen resolution.
        */
        void HandleResourceChange( TInt aType );

        /**
        * From CCoeControl. Handle user's key events.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                           TEventCode aType );

        /**
        * From CCoeControl. Responds to size changes to sets the size and 
        * position of the contents of this control.
        */
        void SizeChanged();

    private:    // Data
        
        // Parent view of the container (not owned)
        CAknView* iParent;
        
        // List box for grouping listing (owned)
        CAknDoubleStyleListBox* iListBox;
        
        // Text array for list box of the group listing (owned)
        CDesCArray* iItemList;
        
    };

#endif // __CCEMUSICCONTAINER_H__

// End of File
