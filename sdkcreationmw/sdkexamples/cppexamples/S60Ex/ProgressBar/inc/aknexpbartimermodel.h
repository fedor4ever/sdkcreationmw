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



#ifndef __AKNEXPBARTIMERMODEL_H__
#define __AKNEXPBARTIMERMODEL_H__


//  INCLUDES
#include <e32def.h>


// STRUCT DECLARATION

/**
*  Struct of Timer Model
*/
struct TAknExPbarTimerModel
    {
    /** Part of the queue time. Used in Queue() function. */
    TInt iHundredths;
    
    /** Increment value for the progressbar */
    TInt iIncrement;
    
    /** Indicates is timer running */
    TBool iRunning;
    };

#endif // __AKNEXPBARTIMERMODEL_H__

// End of File
