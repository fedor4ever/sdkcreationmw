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


#ifndef __HELPEXAMPLE_PAN__
#define __HELPEXAMPLE_PAN__

/* HelpExample application panic codes */
enum THelpExamplePanics 
    {
    EHelpExampleBasicUi = 1
    // add further panics here
    };

void Panic( THelpExamplePanics aReason )
    {
    _LIT( applicationName,"HelpExample" );
    User::Panic( applicationName, aReason );
    }

#endif // __HELPEXAMPLE_PAN__

// End of File
