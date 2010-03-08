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



#ifndef __ASYNCTIMEOBSERVER_H__
#define __ASYNCTIMEOBSERVER_H__

// CLASS DECLARATION
/**
* MAsyncTimeObserver
*  Mixin class.
*  Observer which handles updates from the server example application.
*/
class MAsyncTimeObserver
    {
    public:
        /**
        * HandleTimeUpdate.
        * Handle updates from the server.
        */
        virtual void HandleTimeUpdate() = 0;
    };

#endif // __ASYNCTIMEOBSERVER_H__

// End of File