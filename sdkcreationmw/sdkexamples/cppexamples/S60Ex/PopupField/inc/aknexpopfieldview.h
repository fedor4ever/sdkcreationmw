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


#ifndef __AKNEXPOPFIELDVIEW_H__
#define __AKNEXPOPFIELDVIEW_H__

// INCLUDES
#include <aknview.h>

// CONSTANTS
const TUid KViewId = { 1 }; // UID of view
const TInt KAknExPopFieldResourceIdInitialize = 0;
const TInt KAknExPopFieldTitleBufLength = 256;


// FORWARD DECLARATIONS
class CAknExPopFieldContainer;



// CLASS DECLARATION

/**
* CAknExPopFieldView view class.
*
*/
class CAknExPopFieldView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * CAknExPopFieldView
        * C++ default constructor.
        */
        CAknExPopFieldView();

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CAknExPopFieldView
        * Virtual Destructor.
        */
        virtual ~CAknExPopFieldView();


    public: // New functions

        /**
        * DisplayNextOutlineL
        * Display next outline.
        */
        void DisplayNextOutlineL();

    public: // From CAknView.

        /**
        * From CAknView, Id.
        * Returns the ID of view.
        * @return The ID of view.
        */
        TUid Id() const;

        /**
        * From CAknView, HandleCommandL.
        * handles the commands. if the command is command which is require to
        * display outline-screen, the command is reported to container class.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * HandleSizeChange.
        * Handles a resolution change
        */
        void HandleSizeChange();

    private: // New functions

        /**
        * IndicateTitlePaneTextL
        * Set title text to status-pane
        * @param aCommand
        */
        void IndicateTitlePaneTextL( TInt aCommand );


    private: // From CAknView

        /**
        * From CAknView, DoActivateL.
        * Creates the Container class object.
        * @param aPrevViewId aPrevViewId is not used.
        * @param aCustomMessageId aCustomMessageId is not used.
        * @param aCustomMessage aCustomMessage is not used.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From CAknView, DoDeactivate.
        * Deletes the Container class object.
        */
        void DoDeactivate();


    private: // Data

        /**
        * iContainer
        * Owned by CAknExPopFieldView object.
        */
        CAknExPopFieldContainer* iContainer;


        // Integer for Current outline
        TInt iCurrentOutline;
    };

#endif  //__AKNEXPOPFIELDVIEW_H__

// End of File
