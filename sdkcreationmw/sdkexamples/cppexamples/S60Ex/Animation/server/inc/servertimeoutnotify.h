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


#ifndef __SERVERTIMEOUTNOTIFY_H__
#define __SERVERTIMEOUTNOTIFY_H__

// CLASS DECLARATION

/**
* MTimeoutNotify.
* An instance of a class derived from MTimeoutNotify is an object which can be
* informed when a timeout completes.
*/
class MTimeoutNotify 
    {
    public:

		/**
		* TimedOut
		* Handle the event when a time out occurs.
		*/
        virtual void TimedOut() = 0;
    };

#endif //__SERVERTIMEOUTNOTIFY_H__

// End of File
