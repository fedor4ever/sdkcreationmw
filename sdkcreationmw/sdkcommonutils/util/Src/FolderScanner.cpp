/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


//  Include Files
#include "FolderScanner.h"

EXPORT_C CFolderScanner::~CFolderScanner()
	{
	iFs.Close();
	delete iTimer;
	delete iFolder;
	delete iScanExt;
	}

void CFolderScanner::ConstructL(  CFolderScanner_if* client
								, const TDesC& folder
								, const TDesC& ext
								, int aScanPeriod)

	{
	iClient = client;
	iTimeout = aScanPeriod;

	iFolder = folder.AllocL();
	iScanExt = ext.AllocL();

	User::LeaveIfError(iFs.Connect());	

	// Check that folder really exist
    TEntry entry;
	TInt err = iFs.Entry(folder, entry);

	// If not found, attempt to create
	if(err == KErrNotFound)
		{
		err = iFs.MkDirAll(*iFolder);
		}
	User::LeaveIfError(err);

	// Create a timer
	iTimer = CSysTimer::NewL(this, 0);

	// we want first callback immediately
	iTimer->MilliSeconds(0);
	}

EXPORT_C CFolderScanner* CFolderScanner::NewL(  CFolderScanner_if* client
								, const TDesC& folder
								, const TDesC& ext
								, int aScanPeriod)
	{
	CFolderScanner* self = new (ELeave)CFolderScanner();
	CleanupStack::PushL(self);
	self->ConstructL(client, folder, ext, aScanPeriod);
	CleanupStack::Pop();
	return self;
	}

void CFolderScanner::Timer(int)
{
    if(ScanOnce() == false)
    {
        iTimer->MilliSeconds(iTimeout);
    }
}

/*
 * Scan folders contents once
 */
inline bool CFolderScanner::ScanOnce()
{
    // Perform scan
    CDir* SMSInList = NULL;
    if (iFs.GetDir(*iFolder,KEntryAttNormal,ESortNone,SMSInList) == KErrNone)
    {
        for(TInt i = 0; i<SMSInList->Count(); i++)
        {
            // if we're given a wild card, we ignore chars before '.'
            // just extension chars?
            const TEntry& entry = (*SMSInList)[i];
            if (!entry.IsDir())
            {
                TPtrC ext(entry.iName.Mid(entry.iName.LocateReverse('.')));
                if (!ext.CompareF(iScanExt->Mid(iScanExt->LocateReverse('.'))))
                {
                    TFileName fileName;
                    fileName.Append(*iFolder);
                    fileName.Append(entry.iName);
                    iClient->FileFound(fileName);
                    delete SMSInList;
                    return true;
                }
            }
        }
        delete SMSInList;
    }
    return false;
}

/*
 * We continue scanning by getting a callback asap
 */
EXPORT_C void CFolderScanner::ContinueScan()
{
    iTimer->MilliSeconds(0);
}
