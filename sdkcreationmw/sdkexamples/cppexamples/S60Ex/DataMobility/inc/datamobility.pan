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
* Description:  This file contains panic codes.
*
*/


#ifndef __DATAMOBILITY_PAN__
#define __DATAMOBILITY_PAN__

_LIT( applicationName, "datamobility" );

/** datamobility application panic codes */
enum TDataMobilityPanics
    {
    EDataMobilityUi = 1
    // add further panics here
    };

inline void Panic( TDataMobilityPanics aReason )
    {
    User::Panic( applicationName, aReason );
    }

#endif // __DATAMOBILITY_PAN__
