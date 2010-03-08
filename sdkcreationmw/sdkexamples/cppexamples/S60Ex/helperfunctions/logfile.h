/*
* Copyright (c) 2004, 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __LOGFILE_H__
#define __LOGFILE_H__

// INCLUDES
#include <eikapp.h>
#include <charconv.h>

// CONSTANTS
_LIT8(KCrLf8, "\r\n");
_LIT(KCrLf, "\r\n");

static const TInt KAsciiStart = 0x20;
static const TInt KAsciiEnd = 0x7f;
static const TInt KHexCharLeft = '<';
static const TInt KHexCharRight = '>';
static const TInt KNumberOfDecimalPlaces = 3;

// FORWARD DECLARATIONS

// Link with the following libraries: EFSRV.LIB HAL.LIB CHARCONV.LIB
class CCnvCharacterSetConverter;

// CLASS DECLARATIONS
/**
* CLogFile
* Class to generate a text file containing logging information
*/
class CLogFile : public CBase
    {
        public:
            /**
            * NewL
            * Create a CLogFile object
            * @param aFileName the name of the file to create
            * @param aInitialiseLog if true, and the log file
            * already exists, previous contents will be deleted.
            * If false, append to any existing contents
            * @return a pointer to the created instance of CLogFile
            */
            static CLogFile* NewL( const TDesC& aFileName,
                                   TBool aInitialiseLog );

            /**
            * NewLC
            * Create a CLogFile object
            * @param aFileName the name of the file to create
            * @param aInitialiseLog if true, and the log file
            * already exists, previous contents will be deleted.
            * If false, append to any existing contents.
            * @return a pointer to the created instance of CLogFile
            */
            static CLogFile* NewLC( const TDesC& aFileName,
                                    TBool aInitialiseLog );

            /**
            * ~CLogFile
            * Destroy the object and release all memory objects
            */
            virtual ~CLogFile();

            /**
            * Log
            * Append the byte to the log file (if not a printable char, it
            * will be logged as ascii-hex).
            * @param aByte the byte to log
            */
            void Log( TUint8 aByte );

            /**
            * Log
            * Append the integer to the log file (logged as ascii-hex)
            * @param aNumber the integer to log
            */
            void Log( TUint aNumber );

            /**
            * Log
            * Append text to the log file
            * @param aText the text to log
            */
            void Log( const TDesC8& aText );

            /**
            * Log
            * Append text to the log file
            * @param aText the text to log
            */
            void Log( const TDesC& aText );

            /**
            *LogTime
            * Append a timestamp to the log file.
            * Timestamps are in seconds with three decimal places
            * (but resolution is limited to system timer tick period).
            */
            void LogTime();

            /**
            * LogBytes
            * Append the bytes to the log file (non-printable bytes will be
            * logged as ascii-hex).
            * @param aBuffer the bytes to log
            */
            void LogBytes( const TDesC8& aBuffer );

            /**
            * LogNewline
            * Start a newline in the log file
            */
            void LogNewline();

            /**
            * SetAutoFlush
            * Turn AutoFlush on or off. AutoFlush will automatically flush
            * the log file after each write.
            * @param aOn if true turns AutoFlush on
            */
            void SetAutoFlush( TBool aOn );

            /**
            * SetAutoTimeStamp
            * Turn AutoTimeStamp on or off. AutoTimeStamp will add
            * a timestamp to the start of each new line in the log.
            * @param aOn if true turn AutoTimeStamp on
            */
            void SetAutoTimeStamp( TBool aOn );

            /**
            * SetAutoNewline
            * Turn AutoNewline on or off. AutoNewline
            * starts a new line after each log operation.
            * @param aOn if true turn AutoNewline on
            */
            void SetAutoNewline( TBool aOn );

            /**
            * StaticLogL
            * Static option to append text to the log file
            * @param aFileName the file to append to
            * @param aText the text to append
            */
            static void StaticLogL( const TDesC& aFileName,
                                    const TDesC8& aText );

            /**
            * StaticLogL
            * Static option to append text to the log file
            * @param aFileName the file to append to
            * @param aText the text to append
            */
            static void StaticLogL( const TDesC& aFileName,
                                    const TDesC& aText );

        private:

            /**
            * CLogFile
            * Perform the first phase of two phase construction
            */
            CLogFile();

            /**
            * ConstructL
            * Perform the second phase construction of a CLogFile object
            * @param aFileName the file to open
            * @param aInitialiseLog if true, and the log file already exists,
            * previous contents will be deleted. If false,
            * append to any existing contents
            */
            void ConstructL( const TDesC& aFileName, TBool aInitialiseLog );

            /**
            * LogTimeInternal
            * Internal function to log time
            */
            void LogTimeInternal();

            /**
            * LogTextInternal
            * Internal function to log text
            * @param aText the text to log
            */
            void LogTextInternal( const TDesC8& aText );

            /**
            * LogByteInternal
            * internal function to log a byte
            * @param aByte the byte to log
            */
            void LogByteInternal( TUint8 aByte );

            /**
            * LogIntInternal
            * Internal function to log an integer
            * @param aNumber the integer to log
            */
            void LogIntInternal( TUint aNumber );

            /**
            * StartWrite
            * Perform any initial operation before the main log operation
            */
            void StartWrite();

            /**
            * EndWrite
            * Perform any tidying up operations after the main log operation
            */
            void EndWrite();

            /**
            * Write
            * Do the actual writing, and associated error checking
            * @param aText the text to write
            */
            void Write( const TDesC8& aText );

        private:

            /**
            * iLogFile handle to the log file
            */
            RFile       iLogFile;

            /**
            * iSession file server session
            */
            RFs         iSession;

            /**
            * iLogMillisecsPerTick number of millisecs per system timer tick
            */
            TInt        iLogMillisecsPerTick;

            /**
            * iAutoFlush flag - AutoFlush on
            */
            TBool       iAutoFlush;

            /**
            * iAutoTimestamp flag - AutoTimeStamp on
            */
            TBool       iAutoTimestamp;

            /**
            * iAutoNewline flag - AutoNewline on
            */
            TBool       iAutoNewline;

            /**
            * iCheckNestDepth internal to check StartWrite and EndWrite have
            * been called correctly
            */
            TInt        iCheckNestDepth;

            /**
            * iCharacterConverter converts between unicode and
            * non-unicode characters
            */
            CCnvCharacterSetConverter* iCharacterConverter;

            /**
            * iConverterAvailability flag indicating if conversion
            * is able to occur
            */
            CCnvCharacterSetConverter::TAvailability iConverterAvailability;
    };

#endif // __LOGFILE_H__

// End of File
