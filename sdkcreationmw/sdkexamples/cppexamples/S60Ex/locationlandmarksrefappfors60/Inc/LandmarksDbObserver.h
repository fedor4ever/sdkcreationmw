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
* Description:  See class description below
*
*/


#ifndef __LANDMARKS_DB_OBSERVER_H__
#define __LANDMARKS_DB_OBSERVER_H__

// INCLUDES
#include <e32base.h>
#include <EPos_Landmarks.h>

// CLASS DECLARATION

/**
*  An observer interface used for notifying events originating from the 
*  landmarks default database.
*/
class MLandmarksDbObserver
    {
    public: // new functions

        /**
        * NotifyDbEventL notifies that an event has occurred in the default 
        * landmark database.
        *  
        * @param aEvent contains info about the event.
        * @param aErrorCode possible error codes if reporting the event was 
        * not successful.
        */
        virtual void NotifyDbEventL(TPosLmEvent& aEvent, TInt aErrorCode) = 0;

    };

#endif // __LANDMARKS_DB_OBSERVER_H__

// End of File

