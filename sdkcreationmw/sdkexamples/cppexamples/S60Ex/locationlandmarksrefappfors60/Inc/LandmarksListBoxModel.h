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
* Description:  See class description below
*
*/


#ifndef __LANDMARKS_LIST_BOX_MODEL_H__
#define __LANDMARKS_LIST_BOX_MODEL_H__

// INCLUDES
#include <aknutils.h>

// CLASS DECLARATION

/**
*  CLandmarksListBoxModel bypasses the default filtering algorithm provided by 
*  CAknFilteredTextListBoxModel. The filtering responsibility is forwarded to 
*  the CCoeControl instance owning the list box. That instance must update the
*  model when the filter has been modified.
*/
class CLandmarksListBoxModel : public CAknFilteredTextListBoxModel
    {
    public: // from CAknFilteredTextListBoxModel

        /**
        * Gets an array of strings to be displayed in the listbox for 
        * incremental matching of typed characters. This array always contains 
        * the total number of items rather than a subset filtered out from the 
        * model.
        *
        * @return A text array for matching.
        */
        const MDesCArray* MatchableTextArray() const;

        /**
        * Gets the number of text items in this list box model. This number is 
        * always the total number of items in the model rather than a subset 
        * filtered out from the model
        *
        * @return The number of text items in this list box model
        */
        TInt NumberOfItems() const;

        /**
        * Gets the text to be displayed in the list box control for a specific 
        * item.
        *
        * @param aItemIndex Index of an item
        * @return Text string to be displayed
        */
        TPtrC ItemText(TInt aItemIndex) const;

    };

#endif // __LANDMARKS_LIST_BOX_MODEL_H__

// End of File

