/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include    "AudioStreamApp.h"
#include    "AudioStreamDocument.h"

#ifdef __SERIES60_3X__
#include 	<eikstart.h>
#endif


// ----------------------------------------------------------------------------
// CAudioStreamApp::AppDllUid()
//
// returns application UID
// ----------------------------------------------------------------------------
TUid CAudioStreamApp::AppDllUid() const
    {
    return KUidAudioStream;
    }

   
// ----------------------------------------------------------------------------
// CAudioStreamApp::CreateDocumentL()
//
// creates CAudioStreamDocument object
// ----------------------------------------------------------------------------
CApaDocument* CAudioStreamApp::CreateDocumentL()
    {
    return CAudioStreamDocument::NewL( *this );
    }


// ----------------------------------------------------------------------------
// NewApplication() 
//
// constructs CAudioStreamApp
// ----------------------------------------------------------------------------
EXPORT_C CApaApplication* NewApplication()
    {
    return new CAudioStreamApp;
    }

// ---------------------------------------------------------
// Entry point function for Symbian Apps, separate function for
// S60 3rd Ed and 1st/2nd Ed
// ---------------------------------------------------------
//
#ifdef __SERIES60_3X__
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

#else
GLDEF_C TInt E32Dll( TDllReason )
	{
	return KErrNone;
	}
#endif
// End of File  

