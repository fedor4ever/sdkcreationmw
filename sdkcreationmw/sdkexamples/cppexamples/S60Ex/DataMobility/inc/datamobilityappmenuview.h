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
* Description:  Declares the menu view class.
*
*/


#ifndef DATAMOBILITYAPPMENUVIEW_H
#define DATAMOBILITYAPPMENUVIEW_H

// INCLUDES
#include <aknview.h>
#include <aknnavide.h>

// FORWARD DECLARATIONS
class CDataMobilityAppMenuContainer;

// CLASS DECLARATION

/**
*  CDataMobilityAppMenuView view class.
*
*/
class CDataMobilityAppMenuView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CDataMobilityAppMenuView* NewL();

        /**
        * Destructor.
        */
        ~CDataMobilityAppMenuView();

    public: // Functions from base classes

        /**
        * Returns view´s id.
        * @return View id.
        */
        TUid Id() const;

        /**
        * Handles a command.
        * @param aCommand A command ID.
        */
        void HandleCommandL( TInt aCommand );

    private:

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Initializes view when activated.
        * @param aPrevViewId Id of the previous view (not used).
        * @param aCustomMessageId Custom message id (not used).
        * @param aCustomMessage Custom message (not used).
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * Deactivates view.
        */
        void DoDeactivate();

    private: // Data

        CDataMobilityAppMenuContainer*     iContainer;
        CAknNavigationDecorator*           iNaviDecorator;
    };

#endif  // DATAMOBILITYAPPMENUVIEW_H

// End of File
