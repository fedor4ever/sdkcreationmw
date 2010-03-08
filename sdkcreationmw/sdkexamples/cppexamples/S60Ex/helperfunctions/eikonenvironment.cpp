/*
* Copyright (c) 2004, 2006 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDES
#include <eikappui.h>
#include <eikenv.h>
#include <eikapp.h>
#include <s32file.h>
#include <aknnotewrappers.h>

#include "EikonEnvironment.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// NEikonEnvironment::AppFullName()
// Returns full name of the application.
// ----------------------------------------------------------------------------
//
TFileName NEikonEnvironment::AppFullName()
    {
    return EikAppUi().Application()->AppFullName();
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::ApplicationDriveAndPath()
// Returns the drive and path of the application.
// ----------------------------------------------------------------------------
//
TFileName NEikonEnvironment::ApplicationDriveAndPath()
    {
    TParse parse;
//On WINS the application is on the z drive
#ifdef __WINS__
    TFileName appfullname = AppFullName();

    _LIT( KDriveC,"c:" ); //create literal
    parse.Set(KDriveC, &appfullname, NULL);
#else
    parse.Set(AppFullName(), NULL, NULL);
#endif
    return parse.DriveAndPath();
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::AddPath()
// Returns the drive, path and filename.
// ----------------------------------------------------------------------------
//
TFileName NEikonEnvironment::AddPath( const TDesC& aFileName )
    {
//Check that we have not been sent a file name with the drive included
#ifdef _DEBUG
    TParse theFile;
    theFile.Set( aFileName, NULL, NULL );
    ASSERT( theFile.Drive().Length() == 0 );
#endif

    TParse thePath;
    thePath.Set( ApplicationDriveAndPath(),  &aFileName,  NULL );
    return thePath.FullName();
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::EikEnv()
// Return a reference to the CEikonEnv
// ----------------------------------------------------------------------------
//
CEikonEnv& NEikonEnvironment::EikEnv()
    {
    return *CEikonEnv::Static();
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::EikAppUi()
// Return a reference to the CEikAppUi
// ----------------------------------------------------------------------------
//
CEikAppUi& NEikonEnvironment::EikAppUi()
    {
    return *( EikEnv().EikAppUi() );
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::EikDocument()
// Return a reference to CEikDocument.
// ----------------------------------------------------------------------------
//
CEikDocument& NEikonEnvironment::EikDocument()
    {
    return *( EikAppUi().Document() );
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::AddToStackL()
// Add a control to the control stack.
// ----------------------------------------------------------------------------
//
void NEikonEnvironment::AddToStackL( CCoeControl& aControl,
                                     TInt aPriority,
                                     TInt aStackingFlags )
    {
    RemoveFromStack( aControl );
    EikAppUi().AddToStackL( &aControl, aPriority, aStackingFlags );
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::AddToStackL()
// Add a control to the control stack.
// ----------------------------------------------------------------------------
//
void NEikonEnvironment::AddToStackL( CCoeControl& aControl )
    {
    RemoveFromStack( aControl );
    EikAppUi().AddToStackL( &aControl );
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::RemoveFromStack()
// Remove a control from the control stack.
// ----------------------------------------------------------------------------
//
void NEikonEnvironment::RemoveFromStack( CCoeControl& aControl )
    {
    EikAppUi().RemoveFromStack( &aControl );
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::MessageBox()
// Display a message box
// ----------------------------------------------------------------------------
//
void NEikonEnvironment::MessageBox( const TDesC&  aMessage )
    {
    TRAPD( err,
          CAknInformationNote* informationNote = new (
                                                 ELeave ) CAknInformationNote;
          informationNote->ExecuteLD( aMessage );
          );
    if ( err != KErrNone )
        {
        _LIT( KHelperFunctionsPanic, "Helper" );
        User::Panic( KHelperFunctionsPanic, 1 );
        }
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::FlushWindowServer()
// Flush the windows server to ensure all changes to the
// display are reflected on the screen
// ----------------------------------------------------------------------------
//
void NEikonEnvironment::FlushWindowServer()
    {
    CCoeEnv::Static()->WsSession().Flush();
    }

// ----------------------------------------------------------------------------
// NEikonEnvironment::TheRFs()
// The eikon frameworks file server connection.
// ----------------------------------------------------------------------------
//
RFs& NEikonEnvironment::TheRFs()
    {
    return CCoeEnv::Static()->FsSession();
    }

// End of File
