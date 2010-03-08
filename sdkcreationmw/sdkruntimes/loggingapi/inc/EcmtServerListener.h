/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Helper class for REcmt
*
*/


#ifndef ECMTSERVERLISTENER_H
#define ECMTSERVERLISTENER_H

//  INCLUDES
#include <e32base.h>

//  FORWARD DECLARATION
class REcmt;
class MEcmtServerObserver;

// CONSTANTS
/// Maximum length of notification text from server.
const TInt KMaxServerNotificationLength = 80;

// CLASS DECLARATION
/**
*  This CActive derived class keeps an outstanding request to
*  Ecmt server for receiving notification messages that it then
*  passes to its own observer.
*
*/
class CEcmtServerListener : public CActive
    {
    public:  // Constructors and destructor

		/**
		* Constructor:
		* @param aObserver Observer to which all notifications will be forwarded.
		* @param aServer Reference to the Ecmt server client interface class.
		*/
		CEcmtServerListener( MEcmtServerObserver* aObserver, REcmt& aServer );

		/**
		* Destructor
		*/
		~CEcmtServerListener();

	protected:
		/**
		* This CActive derived worker function forwards the notification received
		* from Ecmt server to the this class's observer and immediately issues
		* another request for more server notifications.
		*/
		void RunL();

        /**
		* From CActive
		*/
		void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CEcmtServerListener();


    private:    // Data
		TBuf8<KMaxServerNotificationLength> iBuff;
		TBuf<KMaxServerNotificationLength> iBuff2;
		MEcmtServerObserver* iObserver;
		REcmt& iServer;
    };

#endif
            
// End of File
