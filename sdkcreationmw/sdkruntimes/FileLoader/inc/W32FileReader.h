/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __W32FILEREADER_H_
#define __W32FILEREADER_H_

//  INCLUDES
#include <e32base.h>
#include <emulator.h>
// CONSTANTS

// MACROS

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CW32FileConnection;

// CLASS DECLARATION

/**
*  This is a simple incremental file reader.
*  @lib resLoader.lib
*  @since 3.1
*/
class CW32FileReader : public CActive
    {
    public:  // Constructors and destructor        
        
        /**
        * Two-phased constructor.
        * @since 3.1
        * @param aFile File to read.
        * @param aChunkSize Chunk size to be read in one turn.
        * @param aFileReaderCallback Observer object-file transaction.
        * @return File reader object
        */
        static CW32FileReader* NewL( HANDLE aFh, TInt aChunkSize, CW32FileConnection* aFileConnection );
        
        /**
        * Destructor.
        */
        virtual ~CW32FileReader();

    public: // New functions

        /**
        * Start reading the file from the beginning.
        * @since 3.1
        * @param 
        * @return void
        */
        void StartReading();

        /**
        * Stop reading 
        * @since 3.1
        * @param 
        * @return void
        */
        void StopReading();

        /**
        * Get partial body buffer.
        * @since 3.1
        * @param aChunkBuffer Pointer to the partial body buffer
        * @return void
        */
        void GetChunkBuffer( TPtrC8& aChunkBuffer );

        /**
        * Purge the chunk buffer.
        * @since 3.1
        * @param 
        * @return void
        */
        void ReleaseChunkBuffer();

    public: // from CActive

        /**
        * Cancel active object
        * @since 3.1
        * @param
        * @return void
        */
        void DoCancel();

        /**
        * Asynchronous handling of transaction events.
        * @since 3.1
        * @param 
        * @return void
        */
        void RunL();

    private:

        /**
        * Construct.
        * @since 3.1
        * @param aFile File to read.
        * @param aChunkSize Chunk size to be read in one turn.
        * @param aFileReaderCallback Observer object-file transaction.
        * @return void
        */
        CW32FileReader( HANDLE aFh, TInt aChunkSize, CW32FileConnection* aFileConnection );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
        /**
        * Read from the file from the current position.
        * @since 3.1
        * @param 
        * @return void
        */
        void ReadFile();

    private:    // Data

        // file to read
		HANDLE iFh; //file handle
        // actual file position 
        TInt          m_filePos;
        // chunk size to read in one turn
        TInt          m_chunkSize;
        // chunk buffer
        HBufC8*       m_chunkBuffer;
        // observer object
        CW32FileConnection* m_fileConnection;
        // pointer to the chunk buffer
        TPtr8         m_chunkBufPtr;
        // ETrue if the file transaction is cancelled
        TBool         m_stopReading;
  };

#endif      // __W32FILEREADER_H__
            
// End of File
