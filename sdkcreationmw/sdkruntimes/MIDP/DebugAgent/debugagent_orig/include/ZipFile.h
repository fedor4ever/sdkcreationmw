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


#ifndef ZIP_FILE_H
#define ZIP_FILE_H

#include <f32file.h>
#include "ZipArchive.h"


// Forward Class Declaration(s)

class CZipFileMember;
class CZipFileMemberInputStream;

// End of Forward Class Declaration(s)

/**
 * A CZipFile represents a ZIP archive contained in a single file.
 */
 
class CZipFile: public CZipArchive
	{
friend class CZipFileMemberInputStream;
friend class CZipFileMemberIterator;

public:

	enum
		{
		KZipFileError 		= KZipArchiveMinError - 1,
		KZipFileNotFound	= KZipFileError - 1,
		KZipFileIOError 	= KZipFileError - 2
		};

public:

	static CZipFile* OpenLC(RFs&, const TDesC&);
	
	virtual ~CZipFile();
	
    void Close(void);
        
    CZipFileMember* GetMemberL(const TDesC8&);
    	
    CZipFileMemberInputStream* GetInputStreamL(const CZipFileMember&);

protected:

	struct TMemberPointer
		{
		HBufC8*	iName;
		TUint32	iCentralHeaderOffset;
		TUint32	iLocalHeaderOffset;
		TUint32 iCRC32;
		TUint32	iCompressedSize;
		TUint32	iUncompressedSize;
		};
		
protected:

	void OpenL(void);

	void FindCentralDirectoryTrailerL(TInt& offset);
    void ReadCentralDirectoryTrailerL(TInt offset, TCentralDirectoryTrailer&);
    void ReadCentralDirectoryHeaderL(TCentralDirectoryHeader&, TMemberPointer&);

	void ReadLocalHeaderL(TUint32, TLocalHeader&);
	
	const TMemberPointer* FindMemberPointer(const TDesC8&);
	
    void LoadMemberPointersL(void);
    
	/*
    CInputStream*   MakeInputStreamL(TUint32, TUint32, TUint32);
	*/
    CZipFileMember* MakeMemberL(TInt);
    CZipFileMember* MakeMemberL(const TMemberPointer&, const TLocalHeader&);
    
    void ReadL(TUint16&);
    void ReadL(TUint32&);
    
protected:

	RFile						iFile;
	TBool						iOpen;
    TInt						iFileLength;
	TCentralDirectoryTrailer 	iTrailer;
	TMemberPointer*				iMemberPointers;
	
	};

#endif /* !_MIDP_SYSTEM_AMS_TEST_ZIP_FILE_H_ */
