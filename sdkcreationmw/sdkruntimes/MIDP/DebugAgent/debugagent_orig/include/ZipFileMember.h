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


#ifndef ZIP_FILE_MEMBER_H
#define ZIP_FILE_MEMBER_H

#include <e32base.h>

class CZipFileMember: public CBase
	{
friend class CZipFile;

public:

	virtual ~CZipFileMember();

    TUint32       GetCRC32(void) const;
	TUint32		  GetCompressedSize(void) const;
	const TDesC8& GetName(void) const;
	TUint32       GetUncompressedSize(void) const;
	
private:

	const HBufC8* iName;
	TUint16		  iCompressionMethod;
	TUint32       iCompressedSize;
	TUint32       iUncompressedSize;
	TUint32		  iCRC32;
	TUint32	      iDataOffset;
	};
	
#endif /* !_MIDP_SYSTEM_AMS_TEST_ZIP_FILE_MEMBER_H_ */

