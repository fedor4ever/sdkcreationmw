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


#ifndef __SIPEXGAMEVIEW_H__
#define __SIPEXGAMEVIEW_H__

// INCLUDES
#include <aknview.h>

#include "SipEx.hrh"

// CONSTANTS
// UID of view
const TUid KGameViewId = { ESIPExView1Id };

// FORWARD DECLARATIONS
class CSIPExEngine;
class CSIPExGameContainer;

// CLASS DECLARATION

/**
*  CSIPExGameView view class.
* 
*/
class CSIPExGameView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * First phase contruction
        */
        static CSIPExGameView* NewLC( CSIPExEngine& aEngine );

        /**
        * Destructor.
        */
        ~CSIPExGameView();
        
    private:
    
        /**
        * C++ default constructor
        * @param aEngine The reference to the application engine.
        */
        CSIPExGameView( CSIPExEngine& aEngine );
        
        /**
        * Second phase construction
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
        * Handle Size changes
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
        * From AknView, Activates view
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

        /**
        * From AknView, Deactivates view
        */
        void DoDeactivate();
        
    public:
        
        /**
        * Return pointer to container, ownership is not transferred
        */
        CSIPExGameContainer* Container();
        

    private: // Data
        CSIPExGameContainer* iContainer;
        
        CSIPExEngine& iEngine;

    };

#endif // __SIPEXGAMEVIEW_H__

// End of File
