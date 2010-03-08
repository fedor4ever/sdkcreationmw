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


#ifndef __LOG_H__
#define __LOG_H__

// CLASS DECLARATIONS
/**
* MLog
* An instance of MLog has the ability to log text.
*/
class MLog
    {
    public: // From MLog

        /**
        * LogL()
        * Add an entry to the log.
        * @param aText the text of the entry
        */
        virtual void LogL( const TDesC& aText ) = 0;

        /**
        * LogL()
        * Add an entry to the log.
        * @param aText the text of the entry
        * @param aExtraText extra text to append onto the entry
        */
        virtual void LogL( const TDesC& aText, const TDesC& aExtraText ) = 0;

        /**
        * LogL()
        * Add an entry to the log.
        * @param aText the text of the entry
        * @param aNumber a number to append onto the entry
        */
        virtual void LogL( const TDesC& aText, TInt aNumber ) = 0;

    };

#endif // __LOG_H__

// End of File
