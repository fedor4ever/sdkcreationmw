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


#ifndef HEADER_SWIENGINE_SWIREMOVERUI
#define HEADER_SWIENGINE_SWIREMOVERUI

#include "Driver.h"
#include <JavaRemover.h>

class CSwiTask;

class CSwiRemoverUi : public MJavaRemoverUI
	{
public:

	CSwiRemoverUi(CSwiTask* aTask);
	virtual ~CSwiRemoverUi();

public:
public:
	/**
	 * Returns TRUE if the user accepts confirmation of removal.
	 *
	 * @param aRemovalMessage	The message to display
	 * @return					ETrue if the user wishes to continue, EFalse if abort
	 */
	TBool ConfirmRemovalL(const TDesC& aRemovalMessage);

	/**
	 * Initialises the progress bar to 0 and sets its total length
	 * 
	 * @param aMaximumLength	The maximum length of the progress bar
	 */
	void InitialiseProgressBarL(TInt aMaximumLength);

	/**
	 * Updates the progress bar by a given number of notches
	 * 
	 * @param aUnits	The number of units to increase the progress bar
	 */
	void UpdateProgressBarL(TInt aUnits);

	/**
	 * Informs user that the MIDlet is in use. Return ETrue if the user 
	 * wishes to continue
	 * 
	 * @return	ETrue if the user wishes to continue with removal
	 */
	TBool MIDletInUseL();

	/**
	 * Informs user that a file is in use. Return ETrue if the user wishes
	 * to continue
	 *
	 * @param aFileName	The file in use
	 * @return 			ETrue if the user wishes to continue
	 */
	TBool FileInUseL(const TDesC& aFileName);

private:
	CSwiTask* iTask;
	};


#endif