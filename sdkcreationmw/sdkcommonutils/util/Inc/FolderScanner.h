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

#ifndef _FolderScanner_STM_H_
#define _FolderScanner_STM_H_

// Constants


// Include files 

#include <e32base.h>
#include <f32file.h>
#include "SysTimer.h"

// Constants 

static const int KDefaultScanPeriod = 500;


// Interface class definition

class CFolderScanner_if
	{
	friend class CFolderScanner;
	protected:
		virtual void FileFound(const TDesC& filename) = 0;
	};


// Class definition 

class CFolderScanner :    public CBase
						, public CSysTimer_if
	{
	protected:
		// Constructor
		CFolderScanner() {}

		// Second phase constructor
		void ConstructL(			// Users client
			  CFolderScanner_if* client

			// Source folder to scan
			, const TDesC& folder

			// Extension
			, const TDesC& ext

			// Scanning period in milliseconds
			, int aScanPeriod		
			);

	public:
		// Destructor	
		IMPORT_C ~CFolderScanner();

		// Construction initiator - use this to construct objects
		IMPORT_C static CFolderScanner* NewL(

			// Users client
			  CFolderScanner_if* client

			// Source folder to scan
			, const TDesC& folder

			// Extension
			, const TDesC& ext

			// Scanning period in milliseconds
			, int aScanPeriod = KDefaultScanPeriod		
			);

		// The scanner stops when it has found something
		// Call this to continue scanning
		IMPORT_C void ContinueScan();

	protected:
		CFolderScanner_if* iClient;

		// Classes for scannning dirs
		RFs				iFs;		// Fileserver session
		CSysTimer*		iTimer;		// Timer for scanning

		// Timeout
		int				iTimeout;
		
		HBufC*			iFolder;
		HBufC*			iScanPath;
		HBufC*			iScanExt;

		void Timer(int);
		inline bool ScanOnce();

	};
#endif // _FolderScanner_STM_H_
