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


#ifndef REGISTRATIONITEMDATA_H
#define REGISTRATIONITEMDATA_H

// INCLUDE FILES 
#include <e32base.h>
#include <eikenv.h>
#include <in_sock.h>

#include <Registration.rsg>

// CONSTANTS
const TInt KMaxSettingTextSize = 50;
const TInt KMaxPasswordTextSize  = 20;

// CLASS DECLARATION

/**
* CRegistrationItemdata 
*/
class TRegistrationItemdata 
    {

    public: // Constructors and destructor
        /**
        * CRegistrationItemdata.
        * C++ default constructor.
        */
        TRegistrationItemdata();

        /**
        * ~CRegistrationItemdata.
        * Virtual Destructor.
        */
        virtual ~TRegistrationItemdata();


    private: // Constructors and destructor

    public: // Data

        // Integer type members 
        TInt iSliderValue;
        TTime iDate;
        TTime iTime;

        // Boolean type members 
        TBool iGender;
        TBool iAccept;

        // Descriptor type members 
        TBuf<KMaxSettingTextSize> iName;
        TBuf<KMaxSettingTextSize> iEmail;
        TBuf<KMaxSettingTextSize> iPhone;
        TBuf<KMaxSettingTextSize> iLogin;
        TBuf<KMaxPasswordTextSize> iPw;
    };


#endif // REGISTRATIONITEMDATA_H

// End of File
