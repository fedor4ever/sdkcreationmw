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
* Description:  Declares second view of the application
*
*/



#ifndef MYVIEWVIEW2_H
#define MYVIEWVIEW2_H

// INCLUDES
#include <aknview.h>


// CONSTANTS
// UID of view
const TUid KView2Id = {2};

// FORWARD DECLARATIONS
class CMyViewContainer2;

// CLASS DECLARATION

/**
*  CMyViewView2 view class.
* 
*/
class CMyViewView2 : public CAknView
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMyViewView2();

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
        * Handle size change
        */
        void HandleClientRectChange();
     

    private:

        /**
        * From AknView, activates view
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * From AknView, deactivates view
        */
        void DoDeactivate();
        


    private: // Data
        CMyViewContainer2* iContainer;
    };

#endif

// End of File
