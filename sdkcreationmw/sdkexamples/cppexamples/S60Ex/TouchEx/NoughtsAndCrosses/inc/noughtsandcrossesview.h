/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __NOUGHTSANDCROSSESVIEW_H__
#define __NOUGHTSANDCROSSESVIEW_H__

// INCLUDES
#include <aknview.h>
#include "noughtsandcrossesengine.h"


// FORWARD DECLARATIONS
class CNoughtsAndCrossesContainer;


// CLASS DECLARATION
/**
*  CNoughtsAndCrossesView view class.
*
*/
class CNoughtsAndCrossesView : public CAknView
    {
    public:

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CNoughtsAndCrossesView using two phase construction,
        * and return a pointer to the created object
        * @return A pointer to the created instance of CNoughtsAndCrossesView
        */
        static CNoughtsAndCrossesView* NewL();

        /**
        * ~CNoughtsAndCrossesView
        * Virtual Destructor.
        */
        virtual ~CNoughtsAndCrossesView();

    public: // Functions from base classes

        /**
        * From CAknView, Id.
        * Returns the ID of view.
        * @return The ID of view.
        */
        virtual TUid Id() const;

        /**
        * From CAknView, HandleCommandL.
        * Handles the commands.
        * @param aCommand Command to be handled.
        */
        virtual void HandleCommandL (TInt aCommand);

    private: // Functions from base classes

        /**
        * From CAknView, DoActivateL.
        * Creates the Container class object.
        * @param aPrevViewId Specified TVwsViewId.
        * @param aCustomMessageId Specified TUid.
        * @param aCustomMessage Specified custom message.
        */
        virtual void DoActivateL (const TVwsViewId& aPrevViewId,
              TUid aCustomMessageId, const TDesC8& aCustomMessage);

        /**
        * From CAknView, DoDeactivate.
        * Deletes the Container class object.
        */
        virtual void DoDeactivate();

    private: // Constructors

        /**
        * CNoughtsAndCrossesView.
        * C++ default constructor.
        */
        CNoughtsAndCrossesView();

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New functions

        void ResetCoordinates();

        void CreateListBoxL (TInt aResourceId);

        void SwapContainerL();

    private: 

        friend class CNoughtsAndCrossesContainer;

    private: // Data

        /**
        * iContainer, container for this view
        * Owned by CNoughtsAndCrossesView object.
        */
        CNoughtsAndCrossesContainer* iContainer;

        int iCursorRow;
        int iCursorColumn;

        int iWinningRowX;
        int iWinningColumnX;
        int iWinningRowY;
        int iWinningColumnY;
    };

#endif // __NOUGHTSANDCROSSESVIEW_H__

// End of File
