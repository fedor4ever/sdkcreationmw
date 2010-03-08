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
* Description:  
*
*/


#ifndef __SIPEXLOGVIEW_H__
#define __SIPEXLOGVIEW_H__

// INCLUDES
#include <aknview.h>

#include "SIPEx.hrh"

// CONSTANTS
// UID of view
const TUid KLogViewId = { ESIPExView2Id };

// FORWARD DECLARATIONS
class CSIPExEngine;
class CSIPExLogContainer;

// CLASS DECLARATION

/**
*  CSIPExLogView view class.
* 
*/
class CSIPExLogView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * First phase contruction
        */
        static CSIPExLogView* NewLC( CSIPExEngine& aEngine );
        
        /**
        * Destructor.
        */
        ~CSIPExLogView();
        
    private:
    
        /**
        * C++ default constructor
        * @param aEngine The reference to the application engine.
        */
        CSIPExLogView( CSIPExEngine& aEngine );
        
        /**
        * EPOC default constructor.
        */
        void ConstructL();

    public: // Functions from base classes
        
        /**
        * Return Uid
        */
        TUid Id() const;

        /**
        * Handle Commands
        */
        void HandleCommandL( TInt aCommand );

        /**
        * Handle size change
        */
        void HandleViewRectChange();
        
        /**
        * Handle menu initialization event.
        * @param aResourceId Resource ID identifying the menu pane to initialise.
        * @param aMenuPane The in-memory representation of the menu pane.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane ); 

    private:

        /**
        * From AknView, activates view
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

        /**
        * From AknView, deactivates view
        */
        void DoDeactivate();
        
    public:
        
        /**
        * Return pointer to container, ownership is not transferred
        */
        CSIPExLogContainer* Container();

    private: // Data
        CSIPExLogContainer* iContainer;
        
        CSIPExEngine& iEngine;
    };

#endif // __SIPEXLOGVIEW_H__

// End of File
