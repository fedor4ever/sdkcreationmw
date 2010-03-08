/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*    Settings T-class used for reading/writing data from/to disk.
*
*/


#ifndef TPOSSIMPSYSETTINGS_H
#define TPOSSIMPSYSETTINGS_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Settings for Simulation PSY
*/
class TPosSimPsySettings
    {    
    public:  // Constructor

        /**
        * C++ default constructor.
        */
        inline TPosSimPsySettings() : iFileName(KNullDesC()) {}

    public:    // Data
        TFileName iFileName;      
    };

#endif      // TPOSSIMPSYSETTINGS_H

// End of File
