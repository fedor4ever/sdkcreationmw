/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares interface class to print in output view.
*
*/


#ifndef DATAMOBILITYPRINTER_H
#define DATAMOBILITYPRINTER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  MDataMobilityPrinter  interface to print in output view.
*
*/
class MDataMobilityPrinter
    {
    public:
        virtual void Print( const TDesC& aLine ) = 0;
    };

#endif // DATAMOBILITYPRINTER_H

// End of File
