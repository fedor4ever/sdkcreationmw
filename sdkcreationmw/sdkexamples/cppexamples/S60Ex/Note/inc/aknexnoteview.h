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


#ifndef __AKNEXNOTEVIEW_H__
#define __AKNEXNOTEVIEW_H__

// INCLUDES
#include <aknview.h>



// FORWARD DECLARATIONS
class CAknExNoteContainer;

// CLASS DECLARATION


/**
* CAknExNoteView view class.
*/
class CAknExNoteView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * CAknExNoteView
        * C++ default constructor.
        */
        CAknExNoteView();

        /**
        * ~CAknExNoteView
        * Virtual Destructor.
        */
        virtual ~CAknExNoteView();

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

    public: // New functions

        /**
        * DisplayNextOutlineL
        * Display next outline.
        */
        void DisplayNextOutlineL();

    public: // From CAknView.

        /**
        * Id
        * Returns the ID of view.
        * @return The ID of view.
        */
        TUid Id() const;


        /**
        * HandleCommandL.
        * handles the commands. if the command is command which is require to
        * display outline-screen, the command is reported to container class.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknView, HandleViewRectChange.
        * Handles the size of the clientrect.
        */
        void HandleViewRectChange();


    private: // New functions

        /**
        * IndicateTitlePaneTextL
        * Set title text to status-pane
        * @param aCommand Command to be handled.
        */
        void IndicateTitlePaneTextL( TInt aCommand );


    private: // From CAknView

        /**
        * DoActivateL.
        * Creates the Container class object.
        * @param aPrevViewId aPrevViewId is not used.
        * @param aCustomMessageId aCustomMessageId is not used.
        * @param aCustomMessage aCustomMessage is not used.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * DoDeactivate.
        * Deletes the Container class object.
        */
        void DoDeactivate();


    private: // Data

        /**
        * iContainer
        * Owned by CAknExNoteView object.
        */
        CAknExNoteContainer*    iContainer;

        /** Integer for Current outline **/
        TInt                    iCurrentOutline;

    };

#endif  //__AKNEXNOTEVIEW_H__

// End of File
