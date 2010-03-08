/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef ISVTELINFOAPPVIEW_H
#define ISVTELINFOAPPVIEW_H

// INCLUDES
#include <aknview.h>
#include <eikdialg.h>
#include <aknform.h>
#include <akntitle.h>

// FORWARD DECLARATIONS
class CIsvTelInfoAppContainer;

// CLASS DECLARATION
/**
* CIsvTelInfoAppView view class.
*
*/
class CIsvTelInfoAppView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * Default constructor.
        */
        CIsvTelInfoAppView();

        /**
        * EPOC constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CIsvTelInfoAppView();

    public: // New functions

        CAknTitlePane* GetTitlePaneL();


        /**
        * StoreL called from AppUi class
        * @param aStore Reference to a stream store
        * @return Returns the Stream Id
        */
        TStreamId StoreL( CStreamStore& aStore ) const;

        /**
        * RestoreL called from AppUi class
        * @param aStore Reference to a stream store
        * @param aStreamId Reference to a stream id
        */
        void RestoreL( const CStreamStore& aStore, const TStreamId aStreamId );

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

    private: // From CAknView

        /**
        * From CAknView, DoActivateL.
        * Creates the Container class object.
        * @param aPrevViewId aPrevViewId is not used.
        * @param aCustomMessageId aCustomMessageId is not used.
        * @param aCustomMessage aCustomMessage is not used.
        */
        void DoActivateL(
            const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

        /**
        * From CAknView, DoDeactivate.
        * Deletes the Container class object.
        */
        void DoDeactivate();

    private: // Data

        CIsvTelInfoAppContainer* iContainer;
    };

#endif

// End of File
