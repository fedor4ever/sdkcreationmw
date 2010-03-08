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


CZipFileMember::~CZipFileMember()
	{
	}

TUint32 CZipFileMember::GetCRC32(void) const
	{
	return iCRC32;
	}
	
TUint32 CZipFileMember::GetCompressedSize(void) const
	{
	return iCompressedSize;
	}

const TDesC8& CZipFileMember::GetName(void) const
	{
	return *iName;
	}
	
TUint32 CZipFileMember::GetUncompressedSize(void) const
	{
	return iUncompressedSize;
	}

