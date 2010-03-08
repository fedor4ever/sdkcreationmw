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


#ifndef HEADER_SWIENGINE_SWIDOWNLOADERUI
#define HEADER_SWIENGINE_SWIDOWNLOADERUI

#include "Driver.h"
#include <JavaInstaller.h>
#include <MJarDownloaderUI.h>

class CSwiTask;

class CSwiDownloaderUi : public MJarDownloaderUI 
	{
public:

	CSwiDownloaderUi(CSwiTask* aTask) { iTask = aTask ; }
	virtual ~CSwiDownloaderUi(){};

public:
	/**
	 * This function is called when copying of the Jar file starts. This
	 * notification is only called for file system copies.
	 * @param aSize The size of the file that is going to be copied. This 
	 * can be used to set the finalvalue of a progress bar for instance.
	 * @return KErrNone if OK, standard error code otherwise
	 */
	TInt CopyStarted(TInt aSize);

	/**
	 * This function is called when the download of the Jar file starts. 
	 * This notification is only called for HTTP downloads.
	 * @param aSize The size of the file that is going to be downloaded. 
	 * This can be used to set the finalvalue of a progress bar for 
	 * instance.
	 * @return KErrNone if OK, standard error code otherwise
	 */
	TInt DownloadStarted(TInt aSize);

	/**
	 * This function is called to update the progress of the download/copy. 
	 * The size indicates how many extra bytes have been downloaded/copied.
	 * @param aSize The number of bytes that has been downloaded/copied 
	 * since the last UpdateProgress call
	 * @return KErrNone if OK, standard error code otherwise
	 */
	TInt UpdateProgress(TInt aSize);

	/**
	 * This function gets the username and password entered whenever
	 * some kind of credentially checking is required
	 * @param aUserName Returned pointer to the username descriptor - 
	 * ownership is transferred to the caller
	 * @param aPassword Returned pointer to the password descriptor - 
	 * ownership is transferred to the caller
	 * @return Indication whether or not a username and password are
	 * returned
	 */
	TBool GetUsernamePasswordL(HBufC8*& aUsername, HBufC8*& aPassword);

private:
	CSwiTask* iTask;
	};


#endif