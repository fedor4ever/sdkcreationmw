/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Geo Profile Debug
*
*/



// INCLUDE FILES
#include "geoprofilesdebug.h"
#include <flogger.h>
#include <e32svr.h>
#include <f32file.h>
#include <bautils.h>

// CONSTANTS

/// Folder where the log resides
_LIT( KGeoProfilesLogFolder, "epos" );

/// The name of the log file
_LIT( KGeoProfilesLogFileName, "GeoProfiles.txt" );

/// The format in which the time is formatted in log
_LIT( KGeoProfilesLogTimeFormat, "%02d.%02d:%02d:%06d ");

/// The length of the string produced by KLocLogTimeFormat
const TInt KGeoProfilesLogTimeFormatLength = 16;

/// How many characters a log line can contain
const TInt KGeoProfilesLogLineLength = 256;

// ========================== OTHER EXPORTED FUNCTIONS =========================



// -----------------------------------------------------------------------------
// Debug
// Generates a log file if c:\logs\eventsui\ folder exists
// -----------------------------------------------------------------------------
//
void Debug( TRefByValue<const TDesC> aText, ... )
    {    
    VA_LIST args;
    VA_START( args, aText );
    
    TBuf< KGeoProfilesLogLineLength > buf;
    buf.FormatList( aText, args );

    #ifdef _DEBUG
        RDebug::Print(buf);
    #endif
	
    RFileLogger logger;    
    TInt ret = logger.Connect();
    if ( ret == KErrNone )
        {
        logger.SetDateAndTime( EFalse,EFalse );
        logger.CreateLog( KGeoProfilesLogFolder, KGeoProfilesLogFileName, EFileLoggingModeAppend );       
        TBuf< KGeoProfilesLogTimeFormatLength > timeStamp;
        TTime now;
        now.HomeTime();
        TDateTime dateTime;
        dateTime = now.DateTime();
        timeStamp.Format( KGeoProfilesLogTimeFormat, 
                          dateTime.Hour(), 
                          dateTime.Minute(),
                          dateTime.Second(), 
                          dateTime.MicroSecond());
        buf.Insert( 0, timeStamp );

        logger.Write(buf);
        }

    logger.Close();

    VA_END( args );
    }
