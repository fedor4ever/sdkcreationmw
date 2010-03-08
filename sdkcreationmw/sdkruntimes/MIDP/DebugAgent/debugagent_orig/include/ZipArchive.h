/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef ZIP_ARCHIVE_H
#define ZIP_ARCHIVE_H

#include <e32base.h>


class CZipArchive: public CBase
	{
public:

    enum 
		{	
		KZipArchiveError 					= -256,
	    KCentralDirectoryTrailerNotFound	= KZipArchiveError - 1,
	    KCentralDirectoryTrailerInvalid		= KZipArchiveError - 3,
	    KCompressionMethodNotSupported		= KZipArchiveError - 4,
	    KLocalHeaderSignatureInvalid		= KZipArchiveError - 5,
	    KMultiDiskArchivesNotSupported		= KZipArchiveError - 6,
		KMembetNotFound						= KZipArchiveError - 7,

		KZipArchiveMinError					= KZipArchiveError - 8

		};
    
    enum TCompressionMethod 
		{
		
		EStored 		= 0,
		EShrunk 		= 1,
		EReducedCFOne	= 2,
		EReducedCFTwo	= 3,
		EReducedCFThree	= 4,
		EReducedCFFour	= 5,
		EImploded		= 6,
		EReservedOne	= 7,
		EDeflated		= 8,
		EReservedTwo	= 9,
		EPKWAREImploded	= 10
		
		};

protected:

    enum // Constants 
		{
    
        KCentralDirectorySignature  		= 0x06054b50,
    	KCentralDirectoryHeaderSignature	= 0x02014b50,
    	KLocalHeaderSignature   			= 0x04034b50
		};
	
	enum // Lengths 
		{
	
		KCentralDirectoryTrailerFixedLength = 22,
		KLocalHeaderFixedLength             = 30,
		KSignatureLength                    =  4
		};
	
	enum // Offsets 
		{
	
		KCentralFileHeaderFileNameLengthOffset   = 28,
		KCentralFileHeaderExtraFieldLengthOffset = 30,
		KCentralFileHeaderFileNameOffset         = 46
		};
	
    struct TCentralDirectoryTrailer 
		{
        TUint32 signature;
		TUint16 diskNumber;
		TUint16 startDiskNumber;
		TUint16 localEntryCount;
		TUint16 iTotalEntryCount;
		TUint32 size;
		TUint32 iOffset;
		TUint16 commentLength;
		// comment -- variable length
		};
	
	struct TCentralDirectoryHeader 
		{
		TUint32 iSignature;
		TUint16 iMadeBy;
		TUint16 iRequired;
		TUint16 iFlags;
		TUint16 iCompressionMethod;
		TUint16 iLastModifiedFileTime;
		TUint16 iLastModifiedFileDate;
		TUint32 iCRC32;
		TUint32 iCompressedSize;
		TUint32 iUncompressedSize;
		TUint16 iFileNameLength;
		TUint16 iExtraFieldLength;
		TUint16 iFileCommentLength;
		TUint16 iDiskNumberStart;
		TUint16 iInternalFileAttributes;
		TUint32 iExternalFileAttributes;
		TUint32 iLocalHeaderOffset;
		// file name    -- variable length 
		// extra field  -- variable length
		// file comment -- variable length
		};

	struct TLocalHeader 
		{
		TUint32 iSignature;
		TUint16 iVersionNeeded;
		TUint16 iFlags;
		TUint16 iCompressionMethod;
		TUint16 iLastModifiedFileTime;
		TUint16 iLastModifiedFileDate;
		TUint32 iCRC32;
		TUint32 iCompressedSize;
		TUint32 iUncompressedSize;
		TUint16 iFileNameLength;
		TUint16 iExtraFieldLength;
		// file name    -- variable length 
		// extra field  -- variable length
		};
	};

#endif /* !_MIDP_SYSTEM_AMS_TEST_ZIP_ARCHIVE_H_ */

