/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares first view of the application
*
*/


#ifndef MYVIEWVIEW_H
#define MYVIEWVIEW_H

// INCLUDES
#include <aknview.h>
#include <akntabgrp.h>        // For handling tabs


// CONSTANTS
// UID of view
const TUid KViewId = {1};

// FORWARD DECLARATIONS
class CMyViewContainer;

// CLASS DECLARATION

/**
*  CMyViewView view class.
* 
*/
class CMyViewView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL(CAknTabGroup*);

        /**
        * Destructor.
        */
        ~CMyViewView();

    public: // Functions from base classes
        
        /**
        * Return Uid
        */
        TUid Id() const;

        /**
        * Handle Commands
        */
        void HandleCommandL(TInt aCommand);

        /**
        * Handle Size changes
        */
        void HandleClientRectChange();
           

    private:

        /**
        * From AknView, Activates view
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * From AknView, Deactivates view
        */
        void DoDeactivate();
        


    private: // Data
        CMyViewContainer* iContainer;
        CAknTabGroup* iTabGroup;

    };

#endif

// End of File
