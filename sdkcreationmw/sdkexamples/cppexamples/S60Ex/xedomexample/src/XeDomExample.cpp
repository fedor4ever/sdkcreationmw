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


#include <e32base.h>
#include "XeDomExample.h"

_LIT(KXEDomExName, "XEDomExample");

/**
 * EUMALOR_ENV is defined here for emulator platforms on which
 * testdata can be exported to C: in bld.inf file for this application.
 * For hardware platforms, the testdata files are assumed to be copied
 * to the drive on which this application is installed.
 */
 
#ifdef __WINS__
#define EUMALOR_ENV
#elif __WINSCW__
#define EUMALOR_ENV
#endif

/*
* Implementation of CXEDomEx
*/


CXEDomEx::CXEDomEx(const TDesC& aTestName):iTest(aTestName)
{
	iTest.Start(KNullDesC);
}

EXPORT_C CXEDomEx::~CXEDomEx()
{
    iTest.Close();
	iDomParser.Close();
	iDomImpl.Close();
}

EXPORT_C CXEDomEx* CXEDomEx::NewLC()
{
	CXEDomEx* me = new(ELeave) CXEDomEx(KXEDomExName);
	CleanupStack::PushL(me);
	me->ConstructL();
	return me;
}

EXPORT_C CXEDomEx* CXEDomEx::NewL()
{
	CXEDomEx* me = NewLC();
	CleanupStack::Pop(me);
	return me;
}

void CXEDomEx::GetDriveLetterL()
{
#ifdef EUMALOR_ENV
    _LIT(KCDrive, "C:");
    iDrive = KCDrive;
#else
    RProcess thisProcess;
    TFileName myFileName = thisProcess.FileName();
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL(fs);
	
	TParse fp;
    fs.Parse(myFileName, fp);
    iDrive = fp.Drive();
    CleanupStack::PopAndDestroy(); //fs
#endif
}

void CXEDomEx::ConstructL()
{
	// Initialize the XmlEngine
	iDomImpl.OpenL();
	
	// Instantiate the parser
	TInt err = iDomParser.Open( iDomImpl );
	if(KErrNone != err)
	    User::Leave(err);
	
	//Get the installation drive, required for testdata
	GetDriveLetterL();
	
	_LIT(KFormat, "\nUsing testdata from %S");
	iTest.Console()->ClearScreen();
	iTest.Console()->Printf(KFormat, &iDrive);
    PressAnyKey();
}

void CXEDomEx::PressAnyKey()
{
    _LIT(KPressAnyKey, "\nPress any key to continue");
	iTest.Printf(KPressAnyKey);
	iTest.Getch();
}

HBufC8* CXEDomEx::ReadFileL(const TDesC& aFileName)
{
	// open fs and file
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL(fs);

	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(fs, aFileName, EFileStream));

	TInt size=0;
	User::LeaveIfError(file.Size(size));
	HBufC8* buf = HBufC8::NewLC(size);
	TPtr8 bufPtr = buf->Des();
	User::LeaveIfError(file.Read(bufPtr));

	CleanupStack::Pop(buf);
	CleanupStack::PopAndDestroy(); //file
	CleanupStack::PopAndDestroy(); //fs
	return buf;
}

TInt CXEDomEx::GetSelection(const TDesC& aPrompt, const TDesC& aValidChoices)
//	Present the user with a list of options, and get their selection
{
    _LIT(KFormat1, "%S ");
    _LIT(KFormat2, "[%S] :");
	TKeyCode key = EKeyNull;
	iTest.Console()->SetPos (0, iTest.Console()->WhereY ());
	iTest.Console()->Printf(KFormat1, &aPrompt);
	iTest.Console()->Printf(KFormat2, &aValidChoices);
	TInt xPos = iTest.Console()->WhereX();
	
	TInt retVal = KErrNotFound;
	_LIT(KFormat3, "%c ");
	while (retVal == KErrNotFound)
	{
		key = iTest.Getch();
		iTest.Console()->Printf(KFormat3, key);
		iTest.Console()->SetPos(xPos);
		if(KErrNotFound != aValidChoices.Locate((TChar)key)) // Check if the choice is valid
		{
		    retVal = key - '0';
		}
	}
	
	_LIT(KNewLines, "\n\n");
	iTest.Console()->Printf(KNewLines);
	return retVal;
}


EXPORT_C void CXEDomEx::StartExampleL()
{
	TBool done = EFalse;
	const TDesC *example = NULL;
	TInt err= KErrNone;
	
    // Repeat until user selects quit
	while (!done)
	{
		iTest.Console()->ClearScreen();
		_LIT(KSelectOption, " Select an option \n\n");
		iTest.Printf(KSelectOption);
		
		_LIT(KPossibleSelectionsText, " 1) Document Parsing\n 2) Document Saving\n 3) Document Processing\n 4) Namespace Processing\n 5) Attributes Access\n 6) Elements Access\n 7) Data Containers\n 0) Quit\n");
		_LIT(KPossibleSelections,"12345670");
		TInt selection = GetSelection(KPossibleSelectionsText, KPossibleSelections);

		// Set the example method to demonstrate for the user
		_LIT(KParsing, "Document Parsing,");
		_LIT(KSave, "Document Saving,");
		_LIT(KDocProcess, "Document Processing,");
		_LIT(KNsProcess, "Namespace Processing,");
		_LIT(KAttributes, "Attributes Access,");
		_LIT(KElements, "Elements Access,");
		_LIT(KContainer, "Data Containers,");
		_LIT(KQuit, "Example application exit...");
		switch (selection)
		{
			case EParsing:
			TRAP(err, ParseExampleL());
			example = &KParsing;
			break;

			case ESave:
			TRAP(err, SaveExampleL());
			example = &KSave;
			break;

			case EDocProcess:
			TRAP(err, DocProcessingExampleL());
			example = &KDocProcess;
			break;

			case ENsProcess:
			TRAP(err, NsProcessingExampleL());
			example = &KNsProcess;
			break;

			case EAttributes:
			TRAP(err, AttributesExampleL());
			example = &KAttributes;
			break;

			case EElements:
			TRAP(err, ElementsExampleL());
			example = &KElements;
			break;

			case EContainer:
			TRAP(err, DataContainersExampleL());
			example = &KContainer;
			break;

			case EQuit:
			done = ETrue;
			iTest.Console()->Printf(KQuit);
			break;

			default:
			;
		}

        if(!done)
        {
            _LIT(KPathNotFound, "Input/output path not found");
            _LIT(KMsg1, "Error occurred, error code:%d\n ");
            _LIT(KMsg2, "%S\n executed successfully.");
    		if(err == KErrPathNotFound)
    		    iTest.Console()->Printf(KPathNotFound);
    		else
    		{
    		    if(err != KErrNone)
    			    iTest.Console()->Printf(KMsg1, err);
    		    else
    			    iTest.Console()->Printf(KMsg2, example);
    		}
		}

		PressAnyKey();
	} // while
}


// Called from E32Main()
LOCAL_D void TestL()
{
	// Create and start the client
	CXEDomEx* domEx = CXEDomEx::NewLC();
	domEx->StartExampleL();

	CleanupStack::PopAndDestroy(domEx);
}


GLDEF_C TInt E32Main()
// Main program - run the tests within a TRAP harness, reporting any errors.
{
	__UHEAP_MARK;
	CTrapCleanup* tc=CTrapCleanup::New();

	TRAPD(err,TestL());
	
	_LIT(KPanic, "Application failed, error: %i");
	if (err!=KErrNone)
	    User::Panic(KPanic, err);
	
	delete tc;
	__UHEAP_MARKEND;
	return KErrNone;
}
