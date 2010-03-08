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


#ifndef  ZIP_FILE_MEMBER_INPUT_STREAM_H
#define  ZIP_FILE_MEMBER_INPUT_STREAM_H

#include <EZLib.h>

class CZipFile;

class CZipFileMemberInputStream: public CBase
	{
friend class CZipFile;

public:

	virtual ~CZipFileMemberInputStream();
	
	void ReadL(TDes8&, TInt);

private:

	static CZipFileMemberInputStream* NewL(CZipFile&, TUint32, TUint32, TUint32);
	CZipFileMemberInputStream(CZipFile&, TUint32, TUint32, TUint32);
	
	void GetBytesL(void);
	void GetCompressedBytesL(void);
	TInt GetStoredBytes(TUint8*, TUint32);
	
	void GetBytesL(TDes8&, TInt);
	void GetStoredBytesL(TDes8&);

private:

	enum
		{
		KBytesSize    		 = 192,
		kCompressedBytesSize = 64
		};
		
private:
	
	CZipFile& 	iZipFile;
	TUint32   	iCompressionMethod;
	z_stream  	iStream;
	TUint32     iCompressedSize;
	TInt		iFileOffset;
	TInt		iOffset;
	TBool       iDone;
	TUint8		iBytes[KBytesSize];
	TUint32     iBytesLength;
	TUint32     iBytesOffset;
	TUint8		iCompressedBytes[kCompressedBytesSize];
	TUint32     iCompressedBytesOffset;

	};

#endif /* ! _MIDP_SYSTEM_AMS_TEST_ZIP_FILE_MEMBER_INPUT_STREAM_H_ */

