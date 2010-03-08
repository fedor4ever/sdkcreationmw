/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "IconFileProvider.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIconFileProvider::CIconFileProvider()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CIconFileProvider::CIconFileProvider(RFs aSession) : iSession(aSession)
    {
    }

// -----------------------------------------------------------------------------
// CIconFileProvider::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIconFileProvider* CIconFileProvider::NewL(RFs aSession, const TDesC& aFilename)
    {
    CIconFileProvider* self = new (ELeave) CIconFileProvider(aSession);
    CleanupStack::PushL(self);
    self->ConstructL(aFilename);
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CIconFileProvider::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CIconFileProvider::ConstructL( const TDesC& aFilename)
    {
    iFilename = aFilename.AllocL();
    }

// -----------------------------------------------------------------------------
// CIconFileProvider::~CIconFileProvider()
// Destructor
// -----------------------------------------------------------------------------
//
CIconFileProvider::~CIconFileProvider()
    {
    delete iFilename;
    }

// -----------------------------------------------------------------------------
// CIconFileProvider::RetrieveIconFileHandleL()
// Returns an open file handle to the icon file.
// -----------------------------------------------------------------------------
//
void CIconFileProvider::RetrieveIconFileHandleL( RFile& aFile,
                                                const TIconFileType aType )
    {
    // MAknIconFileProvider will by default try to open files that
	// have .mif extension first. If in case of this application, only the
	// file handle of mbm-file is passed to framework,
	// MAknIconFileProvider result will be KErrArgument. This is solved by
	// giving the correct file extension determined by function parameter
	// aType.
	//
    TFileName filename;

    filename = iFilename->Des().Left(iFilename->Length()-3);

    if (aType == EMifFile)
        {
        _LIT(KMif, "mif");
        filename.Append(KMif);
        }
    else // mbm
        {
        _LIT(KMbm, "mbm");
        filename.Append(KMbm);
        }

	User::LeaveIfError(aFile.Open(iSession, filename, EFileShareAny));
    }

// -----------------------------------------------------------------------------
// CIconFileProvider::Finished()
// With this method, AknIcon framework informs that it does not use this
// MAknIconFileProvider instance any more.
// -----------------------------------------------------------------------------
//
void CIconFileProvider::Finished()
    {
    }

