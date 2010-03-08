/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <HelloWorldBasic.rsg>
#include <f32file.h>
#include <s32file.h>

#include "HelloWorldBasic.pan"
#include "HelloWorldBasicAppUi.h"
#include "HelloWorldBasicAppView.h"
#include "HelloWorldBasic.hrh"
#include "HelloWorldBasicQueryDialog.h"

_LIT( KHelloFileName, "Hello.txt" );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CHelloWorldBasicAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHelloWorldBasicAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    // Here the Hello.txt file is created. Because HelloWorld application is
    // localized to various languages, the Hello.txt-file is created every time
    // with current localization language
    //
    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL( fsSession );
    TInt objectsInStack = 1;

    #if defined(__WINS__) || defined(__WINSCW__)
    // create private folder, when testing in emulator.
    // ignore the return value; if this fails, then file.Replace() will fail
    // and a warning note will be printed.
    //
    fsSession.CreatePrivatePath(EDriveC);
    #endif

    RFile file;

    // Create a file to write the text to
    TInt err = file.Replace(fsSession, KHelloFileName, EFileWrite );
    if (KErrNone == err)
        {
        CleanupClosePushL( file );

        RFileWriteStream outputFileStream( file );
        CleanupClosePushL( outputFileStream );

        // Load a string from the resource file and stream it to file
        HBufC* textResource = StringLoader::LoadLC( R_HEWB_FILE_TEXT );
        objectsInStack += 3; // file, outputFileStream, testResource

        outputFileStream << *textResource;
        }
    else
        {
        _LIT(KFileWriteFailed,"Writing file %S failed: error %d");
        CAknWarningNote* note = new ( ELeave ) CAknWarningNote(ETrue);

        TBuf<64> text;
        text.Format(KFileWriteFailed, &KHelloFileName, err);
        note->ExecuteLD( text );
        }

    CleanupStack::PopAndDestroy(objectsInStack, &fsSession);

    // Create view object
    iAppView = CHelloWorldBasicAppView::NewL( ClientRect() );

    }
// -----------------------------------------------------------------------------
// CHelloWorldBasicAppUi::CHelloWorldBasicAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHelloWorldBasicAppUi::CHelloWorldBasicAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CHelloWorldBasicAppUi::~CHelloWorldBasicAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CHelloWorldBasicAppUi::~CHelloWorldBasicAppUi()
    {
    if ( iAppView )
        {
        delete iAppView;
        iAppView = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CHelloWorldBasicAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CHelloWorldBasicAppUi::HandleCommandL( TInt aCommand )
    {
    // clear possible old user-given text
    if (iAppView->GetText().Size() > 0)
        {
        iAppView->GetText().Zero();
        iAppView->DrawNow();
        }

    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EHelloWorldBasicCommand1:
            {
            // Load a string from the resource file and display it
            HBufC* textResource = StringLoader::LoadLC( R_HEWB_COMMAND1_TEXT );
            CAknInformationNote* note = new ( ELeave ) CAknInformationNote;

            // Show the information Note with
            // textResource loaded with StringLoader.
            note->ExecuteLD( *textResource );

            // Pop HBuf from CleanUpStack and Destroy it.
            CleanupStack::PopAndDestroy( textResource );
            }
            break;

        case EHelloWorldBasicCommand2:
            {
            RFs fsSession;
            RFile rFile;

            // Connects a client process to the fileserver
            User::LeaveIfError(fsSession.Connect());
            CleanupClosePushL(fsSession);

            //Open file where the stream text is
            User::LeaveIfError(rFile.Open(fsSession,KHelloFileName, EFileStreamText));
            CleanupClosePushL(rFile);

            // copy stream from file to RFileStream object
            RFileReadStream inputFileStream(rFile);
            CleanupClosePushL(inputFileStream);

            // HBufC descriptor is created from the RFileStream object.
            HBufC* fileData = HBufC::NewLC(inputFileStream, 32);

            CAknInformationNote* note = new ( ELeave ) CAknInformationNote;

            // Show the information Note
            note->ExecuteLD( *fileData );

            // Pop loaded resources from the cleanup stack:
            // filedata, inputFileStream, rFile, fsSession
            CleanupStack::PopAndDestroy(4, &fsSession);
            }
            break;

        case EHelloWorldBasicCommand3:
            {
            // Load a string from the resources and use it as a default value
            HBufC* defaultText = StringLoader::LoadLC( R_HEWB_FILE_TEXT );

            CHelloWorldQueryDialog *dlg = new (ELeave)
                CHelloWorldQueryDialog( iAppView->GetText(), defaultText );

            dlg->ExecuteLD( R_DIALOG_TEXT_EDIT_QUERY );
            iAppView->DrawNow();

            // Pop HBuf from CleanUpStack and Destroy it.
            CleanupStack::PopAndDestroy( defaultText );
            }
            break;

        default:
            break;
        }
    }
// -----------------------------------------------------------------------------
//  Called by framework when layout is changed.
//  Passes the new client rectangle to the AppView
// -----------------------------------------------------------------------------
//
void CHelloWorldBasicAppUi::HandleResourceChangeL( TInt aType )
{
    // base-class call also
    CAknAppUi::HandleResourceChangeL(aType);
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        if (iAppView)
            iAppView->SetRect( ClientRect() );
        }
}

// End of File

