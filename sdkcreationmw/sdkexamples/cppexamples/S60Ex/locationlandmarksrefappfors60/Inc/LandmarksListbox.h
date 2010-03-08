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


#ifndef __LANDMARKS_LIST_BOX_H__
#define __LANDMARKS_LIST_BOX_H__

// INCLUDES
#include <aknlists.h>

// FORWARD DECLARATIONS
class CLandmarksListBoxModel;

// CLASS DECLARATION

/**
*  CLandmarksListBox exists because the list boxes in Landmark reference 
*  application needs a specialized version of CAknFilteredTextListBoxModel 
*  called CLandmarksListBoxModel. CLandmarksListBox creates an instance of 
*  CLandmarksListBoxModel instead of the default CAknFilteredTextListBoxModel.
*/
class CLandmarksListBox : public CAknSingleGraphicStyleListBox
    {
    public: // Constructors and destructor
    
        /**
        * Symbian 2nd phase constructor. Should be called after the 
        * constructor.
        *
        * @param aParent the parent control
        * @param aFlags flags defining the behaviour of the listbox
        */
        void ConstructL(
            const CCoeControl* aParent,
            TInt aFlags);

    public: // from CAknColumnListBox

        /**
        * Creates the listbox model.
        */
        void CreateModelL();

    public: // from CEikColumnListBox

        /**
        * Returns a pointer to the listbox model.
        *
        * @return a pointer to the listbox model
        */
        CLandmarksListBoxModel* Model() const;

    };

#endif // __LANDMARKS_LIST_BOX_H__

// End of File

