/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_MSGSESSION_PAN__
#define __T_MSGSESSION_PAN__


//  Data Types

enum TT_MsgSessionPanic
    {
    ET_MsgSessionUnknownCommand,
    ET_MsgSessionNullPointer
    };


//  Function Prototypes

GLREF_C void Panic(TT_MsgSessionPanic aPanic);


#endif  // __T_MSGSESSION_PAN__

// End of file
