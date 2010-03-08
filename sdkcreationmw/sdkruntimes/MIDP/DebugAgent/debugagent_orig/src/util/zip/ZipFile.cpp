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


#include "ZipFileMember.h"
#include "ZipFileMemberInputStream.h"
#include "ZipFile.h"

/* Apparently unused which causes a warning with RVCT2/armv5
static const CZipArchive::TCompressionMethod KCompressionMethodTable[] =
	{
	CZipArchive::EStored,
	CZipArchive::EShrunk,
	CZipArchive::EReducedCFOne,
	CZipArchive::EReducedCFTwo,
	CZipArchive::EReducedCFThree,
	CZipArchive::EReducedCFFour,
	CZipArchive::EImploded,
	CZipArchive::EReservedOne,
	CZipArchive::EDeflated,
	CZipArchive::EReservedTwo,
	CZipArchive::EPKWAREImploded
	};
*/
CZipFile* CZipFile::OpenLC(RFs& iFS, const TDesC& aFileName)
	{
	CZipFile* zipFile;
	
	zipFile = new (ELeave) CZipFile;
	CleanupStack::PushL(zipFile);
	User::LeaveIfError(zipFile->iFile.Open(iFS, aFileName, EFileRead));
	zipFile->iOpen = ETrue;
	zipFile->OpenL();
	return zipFile;
	}


CZipFile::~CZipFile()
	{
	TUint32 memberCount = iTrailer.iTotalEntryCount;

	for (TUint32 i = 0; i < memberCount; i++)
		{
		delete iMemberPointers[i].iName;
		}
	delete[] iMemberPointers;
	if (iOpen)
		{
		iFile.Close();
		}
	}
	
void CZipFile::Close()
	{
	iFile.Close();
	iOpen = EFalse;
	}

CZipFileMemberInputStream* CZipFile::GetInputStreamL(const CZipFileMember& aMember)
	{
	TUint32 compressionMethod;
	
	compressionMethod = aMember.iCompressionMethod;
	if ((compressionMethod != EStored) &&
	    (compressionMethod != EDeflated)) 
	    {
		User::Leave(KCompressionMethodNotSupported);
	    }
	return CZipFileMemberInputStream::NewL(
							*this,
						    aMember.iDataOffset,
				            aMember.iCompressedSize,
				            compressionMethod);
	}

CZipFileMember* CZipFile::GetMemberL(const TDesC8& aName)
	{
	TLocalHeader		  header;
	const TMemberPointer* memberPointer;
	
	memberPointer = FindMemberPointer(aName);
	if (memberPointer == NULL)
		{
		User::Leave(KErrNotFound);
		}
	ReadLocalHeaderL(memberPointer->iLocalHeaderOffset, header);
	return MakeMemberL(*memberPointer, header);
	}

void CZipFile::OpenL(void)
	{
	TInt offset;
	
	FindCentralDirectoryTrailerL(offset);
	ReadCentralDirectoryTrailerL(offset, iTrailer);
	if (iTrailer.startDiskNumber != iTrailer.diskNumber) 
		{
		User::Leave(KMultiDiskArchivesNotSupported);
		}
	if ((static_cast<TInt>(iTrailer.iOffset) > iFileLength) || (static_cast<TInt>(iTrailer.iOffset + iTrailer.size) > iFileLength)) 
		{
	    User::Leave(KCentralDirectoryTrailerInvalid);
		}
	LoadMemberPointersL();
	}

// Implementation

/*
 * Find the 'end of central directory record'. This is at the 'end' of
 * the file, but since it is not a fixed length structure, we have to
 * hunt for it.
 *
 * We try assuming that the variable length section of the record is
 * empty, which usually appears to be the case.
 *
 * If this does not work we resort to 'walking backwards' through the
 * file looking for the signature bytes.
 *
 */
 
void CZipFile::FindCentralDirectoryTrailerL(TInt& offset)
{
	TUint8 signature[KSignatureLength];

	User::LeaveIfError(iFile.Size(iFileLength));
    if (iFileLength <= KCentralDirectoryTrailerFixedLength) 
		{
		User::Leave(KZipArchiveError);
		}

    // Try the obvious place first.

    offset = iFileLength - KCentralDirectoryTrailerFixedLength;

	TPtr8 sigPtr(signature, KSignatureLength);
	
	User::LeaveIfError(iFile.Read(offset, sigPtr));
	if ((signature[0] == 0x50) && 
		(signature[1] == 0x4b) &&
		(signature[2] == 0x05) &&
		(signature[3] == 0x06)) 
		{
		// Found it
		return;
		}

	// This is a slow but fairly obvious way of searching 
	// backwards through the file.
	--offset;
	while (offset > 0) 
		{
		signature[3] = signature[2];
		signature[2] = signature[1];
		signature[1] = signature[0];
		User::LeaveIfError(iFile.Read(offset, sigPtr, 1));
		if ((signature[0] == 0x50) && 
			(signature[1] == 0x4b) &&
			(signature[2] == 0x05) &&
			(signature[3] == 0x06)) 
			{
			return;
			}
		--offset;
		}
	User::Leave(KCentralDirectoryTrailerNotFound);
	}


void CZipFile::ReadCentralDirectoryTrailerL(TInt offset, struct TCentralDirectoryTrailer& r)
	{
	TInt start = offset + KSignatureLength;

    // Skip the signature	
	User::LeaveIfError(iFile.Seek(ESeekStart, start));
	ReadL(r.diskNumber); 
	ReadL(r.startDiskNumber);
	ReadL(r.localEntryCount); 
	ReadL(r.iTotalEntryCount);
	ReadL(r.size);
	ReadL(r.iOffset);
	}

void CZipFile::LoadMemberPointersL(void)
	{
	TCentralDirectoryHeader	header;
	TUint32					i;
	TUint32					memberPointerCount;
	TInt					start = static_cast<TInt>(iTrailer.iOffset);

	User::LeaveIfError(iFile.Seek(ESeekStart, start));
	memberPointerCount = iTrailer.iTotalEntryCount;
	iMemberPointers = new (ELeave) TMemberPointer[memberPointerCount];
	for (i = 0; i < memberPointerCount; i++)
		{
		iMemberPointers[i].iName = NULL;
		}
	for (i = 0; i < memberPointerCount; i++)
		{
		ReadCentralDirectoryHeaderL(header, iMemberPointers[i]);
		}
	}

void CZipFile::ReadCentralDirectoryHeaderL(TCentralDirectoryHeader& aHeader, TMemberPointer& aMemberPointer)
	{
	ReadL(aHeader.iSignature);
	if (aHeader.iSignature != KCentralDirectoryHeaderSignature)
		{
		User::Leave(KZipFileIOError);
		}
	ReadL(aHeader.iMadeBy);
	ReadL(aHeader.iRequired); 
	ReadL(aHeader.iFlags); 
	ReadL(aHeader.iCompressionMethod); 
	ReadL(aHeader.iLastModifiedFileTime);
	ReadL(aHeader.iLastModifiedFileDate);
	ReadL(aHeader.iCRC32); 
	ReadL(aHeader.iCompressedSize);
	ReadL(aHeader.iUncompressedSize); 
	ReadL(aHeader.iFileNameLength); 
	ReadL(aHeader.iExtraFieldLength); 
	ReadL(aHeader.iFileCommentLength);
	ReadL(aHeader.iDiskNumberStart); 
	ReadL(aHeader.iInternalFileAttributes);
	ReadL(aHeader.iExternalFileAttributes); 
	ReadL(aHeader.iLocalHeaderOffset);
	aMemberPointer.iCRC32             = aHeader.iCRC32;
	aMemberPointer.iCompressedSize    = aHeader.iCompressedSize;
	aMemberPointer.iUncompressedSize  = aHeader.iUncompressedSize;
	aMemberPointer.iLocalHeaderOffset = aHeader.iLocalHeaderOffset;
    aMemberPointer.iName = HBufC8::NewL(aHeader.iFileNameLength);

	TPtr8 fileNamePointer = aMemberPointer.iName->Des();

	User::LeaveIfError(iFile.Read(fileNamePointer, aHeader.iFileNameLength));
	
    // Ignore the remaining fields

	TInt current = 0;

	User::LeaveIfError(iFile.Seek(ESeekCurrent, current));

	if ((aHeader.iExtraFieldLength != 0) || (aHeader.iFileCommentLength != 0))
		{
		TInt next = current + aHeader.iExtraFieldLength + aHeader.iFileCommentLength;
	
		User::LeaveIfError(iFile.Seek(ESeekStart, next));
		}
	}

void CZipFile::ReadLocalHeaderL(TUint32 aOffset, TLocalHeader& aHeader)
	{
	TInt start = static_cast<TInt>(aOffset);

	User::LeaveIfError(iFile.Seek(ESeekStart, start));
	ReadL(aHeader.iSignature);
	if (aHeader.iSignature != KLocalHeaderSignature)
		{
		User::LeaveIfError(KLocalHeaderSignatureInvalid);
		}
	ReadL(aHeader.iVersionNeeded);
	ReadL(aHeader.iFlags);
	ReadL(aHeader.iCompressionMethod);
	ReadL(aHeader.iLastModifiedFileTime);
	ReadL(aHeader.iLastModifiedFileDate);
	ReadL(aHeader.iCRC32);
	ReadL(aHeader.iCompressedSize);
	ReadL(aHeader.iUncompressedSize);
	ReadL(aHeader.iFileNameLength);
	ReadL(aHeader.iExtraFieldLength);
	}
	
const CZipFile::TMemberPointer* CZipFile::FindMemberPointer(const TDesC8& aName)
	{
	for (TUint32 i = 0; i < iTrailer.iTotalEntryCount; i++)
		{
		if (!aName.CompareF(*(iMemberPointers[i].iName)))
			{
			return iMemberPointers + i;
			}
		}
	return NULL;
	}
/*
CInputStream* CZipFile::MakeInputStreamL(TUint32 aDataOffset, TUint32 aCompressedSize, TUint32 aCompressionMethod)
	{
	CInputStream* stream;
	
	stream = new CZipFileMemberInputStream(
	               *this,
		           aDataOffset,
			       aCompressedSize,
			       aCompressionMethod);
	return stream;
	}
*/

CZipFileMember* CZipFile::MakeMemberL(TInt aMemberIndex)
	{
	TLocalHeader    header;
	TMemberPointer*	memberPointer;
	
	if (aMemberIndex >= iTrailer.iTotalEntryCount)
		{
		return NULL;
		}
	memberPointer = iMemberPointers + aMemberIndex;
	ReadLocalHeaderL(memberPointer->iLocalHeaderOffset, header);
	return MakeMemberL(*memberPointer, header);
	}

CZipFileMember* CZipFile::MakeMemberL(const TMemberPointer& aMemberPointer, const TLocalHeader&   aHeader)
	{
	CZipFileMember* member;
	
	member = new (ELeave) CZipFileMember;
	member->iCRC32 = aMemberPointer.iCRC32;
	member->iCompressedSize = aMemberPointer.iCompressedSize;
	member->iCompressionMethod = aHeader.iCompressionMethod;
	member->iName = aMemberPointer.iName;
	member->iUncompressedSize = aMemberPointer.iUncompressedSize;
	member->iDataOffset = aMemberPointer.iLocalHeaderOffset + 
	                      KLocalHeaderFixedLength + 
			              aHeader.iFileNameLength + 
				          aHeader.iExtraFieldLength;
	return member;
	}

void CZipFile::ReadL(TUint16& aUs)
	{
	TUint8 bytes[2];
	TPtr8  ptr(bytes, 2);

	User::LeaveIfError(iFile.Read(ptr));
	aUs = static_cast<TUint16>(((bytes[1] << 8) |  bytes[0]) &0xFFFFFFFF);
	}
	
void CZipFile::ReadL(TUint32& aUl)
	{
	TUint8 bytes[4];
	TPtr8  ptr(bytes, 4);

	User::LeaveIfError(iFile.Read(ptr));
	aUl = (bytes[3] << 24) + (bytes[2] << 16) + (bytes[1] << 8) + bytes[0];
	}

