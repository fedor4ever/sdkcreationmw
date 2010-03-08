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


#include <f32file.h>
#include "ZipFile.h"
#include "ZipFileMemberInputStream.h"


CZipFileMemberInputStream* CZipFileMemberInputStream::NewL(    
	CZipFile& aZipFile,
    TUint32   aDataOffset,
    TUint32   aCompressedSize,
    TUint32   aCompressionMethod )
	{
	CZipFileMemberInputStream* zfmis = new (ELeave) CZipFileMemberInputStream(
			aZipFile, aDataOffset, aCompressedSize, aCompressionMethod);

	CleanupStack::PushL(zfmis);
    
	TInt status = ::inflateInit2(&(zfmis->iStream), -MAX_WBITS);

    if (status != Z_OK)
        {
		CleanupStack::PopAndDestroy();
		User::Leave(KErrNoMemory);
        }
	CleanupStack::Pop();
    return zfmis;
	}

CZipFileMemberInputStream::CZipFileMemberInputStream(
    CZipFile& aZipFile,
    TUint32   aDataOffset,
    TUint32   aCompressedSize,
    TUint32   aCompressionMethod):
    iZipFile(aZipFile),
    iCompressionMethod(aCompressionMethod),
	iCompressedSize(aCompressedSize),
	iFileOffset(aDataOffset),
    iOffset(0),
    iDone(EFalse),
    iBytesLength(0),
    iBytesOffset(0),
    iCompressedBytesOffset(0)
    {
    iStream.zalloc = NULL;
    iStream.zfree = NULL;
    iStream.avail_in = NULL;
    iStream.next_in = NULL;
    iStream.avail_out = NULL;
    iStream.next_out= NULL;
    }
    
CZipFileMemberInputStream::~CZipFileMemberInputStream()
	{
    (void)::inflateEnd(&iStream);
	}

void CZipFileMemberInputStream::ReadL(TDes8& aBuffer, TInt aLength)
	{
	if (iCompressionMethod == CZipArchive::EDeflated)
		{
		GetBytesL(aBuffer, aLength);
		}
	else
		{
		GetStoredBytesL(aBuffer);
		}
	}

void CZipFileMemberInputStream::GetBytesL(TDes8& aBuffer, TInt aLength)
	{
	TUint32 bytesLeftToRead = aLength;

	while (bytesLeftToRead > 0)
		{
		TUint32 bytesAvailable = iBytesLength - iBytesOffset;
		TUint32 bytesToCopy;

		while (bytesAvailable == 0)
			{
			GetBytesL();
			bytesAvailable = iBytesLength - iBytesOffset;
			}
		if (bytesAvailable > bytesLeftToRead)
			{
			bytesToCopy = bytesLeftToRead;
			}
		else
			{
			bytesToCopy = bytesAvailable;
			}
		aBuffer.Append(iBytes + iBytesOffset, bytesToCopy);
		iBytesOffset += bytesToCopy;
		bytesLeftToRead -= bytesToCopy;
		}
	}


	
void CZipFileMemberInputStream::GetBytesL(void)
	{
	if (iStream.avail_in == 0)
		{
		GetCompressedBytesL();
		}
	iStream.next_out = iBytes;
	iStream.avail_out = sizeof(iBytes);

	TInt status = ::inflate(&iStream, Z_SYNC_FLUSH);

	iBytesOffset = 0;
	iBytesLength = sizeof(iBytes) - iStream.avail_out;
	switch (status)
		{
	case Z_OK:
		
		break;
		
	case Z_STREAM_END:
	
		if (iBytesLength > 0)
			{
			break;
			}
		// Fall-through

	default:
	
		User::Leave(-1);
		}
	}

void CZipFileMemberInputStream::GetCompressedBytesL(void)
	{
	if (static_cast<TUint32>(iOffset) < iCompressedSize)
		{
		TUint32	nBytesLeft;
		TUint32 nBytesToRead;
		
		nBytesLeft = iCompressedSize - iOffset;
		nBytesToRead = (nBytesLeft > sizeof(iCompressedBytes)) ? sizeof(iCompressedBytes) : nBytesLeft;
		User::LeaveIfError(iZipFile.iFile.Seek(ESeekStart, iFileOffset));

		TPtr8 pointer(iCompressedBytes, nBytesToRead);

		User::LeaveIfError(iZipFile.iFile.Read(pointer, nBytesToRead));

		iFileOffset += nBytesToRead;
		iOffset += nBytesToRead;
		iStream.next_in = iCompressedBytes;
		iStream.avail_in = nBytesToRead;
		}
	else
	if (iDone == EFalse)
		{
		iCompressedBytes[0] = 0;
		iStream.avail_in = 1;
		iStream.next_in = iCompressedBytes;
		iDone = ETrue;
		}
	else
		{
		User::Leave(-1);
		}
	}
	
void CZipFileMemberInputStream::GetStoredBytesL(TDes8&)
	{
	}

TInt CZipFileMemberInputStream::GetStoredBytes(TUint8* aBytes, TUint32 aLength)
	{	
	if ((iOffset + aLength) > iCompressedSize)
		{
		return -1;
		}
	TRAPD(status,iZipFile.iFile.Seek(ESeekStart, iFileOffset));
	if ( status != KErrNone )
		{
		return status;
		}
	
	TPtr8 pointer(aBytes, aLength);

	TRAP(status, iZipFile.iFile.Read(pointer, aLength));
	return status;
	}

