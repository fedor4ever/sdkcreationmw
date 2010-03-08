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


#ifndef __AKNEXNOTECONSTANTS_H__
#define __AKNEXNOTECONSTANTS_H__


// CONSTANTS

// All apllications constants are here.
const TInt KAknExNoteGray = 0xaaaaaa;

const TInt KAknExNoteComponentCount = 1;
const TInt KAknExNoteMessageBufLength = 256;
const TInt KAknExNoteRandomNumberRange = 20;
const TInt KAknExNoteProgressbarFinalValue = 200;
const TInt KAknExNoteInitializeTime = 0;
const TInt KAknExNoteInitializeResourceId = 0;
const TInt KAknExNoteTextBufLength = 256;

// Define show time of wait note and permanent note.
// Unit is microsecond. 
const TInt KMaxPermanentNoteShowTime( 6 * 1000000 );

const TUid KViewId = { 1 }; // UID of view
const TInt KAknExNoteTitleBufLength = 256;

const TInt KPermNoteCallBackPriority( CActive::EPriorityStandard );
const TInt KWaitNoteCallBackPriority( CActive::EPriorityStandard );


#endif  //__AKNEXNOTECONSTANTS_H__

// End of File
