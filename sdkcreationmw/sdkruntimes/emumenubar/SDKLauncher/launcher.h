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


#ifndef __LAUNCHER_APPLICATION_H__
#define __LAUCNHER_APPLICATION_H__

#include <aknapp.h>
#include <eikdoc.h>
#include <aknappui.h>
#include <coecntrl.h>
#include <aknserverapp.h>       // MAknServerAppExitObserver

//===========================================================================
// application class
//===========================================================================

class CLauncherApplication : public CAknApplication
{
    public: // from CAknApplication
        TUid AppDllUid() const;

    protected: // Functions from base classes

        CApaDocument* CreateDocumentL();
};

//===========================================================================
// document class
//===========================================================================

class CEikApplication;

/**
 * CLauncherDocument implements OpenFile-functionality. Document-class just
 * forwards the call to UI-class.
 */
class CLauncherDocument : public CEikDocument
{
    public: // Constructors and destructor

        static CLauncherDocument* NewL( CEikApplication& aApp );
        static CLauncherDocument* NewLC( CEikApplication& aApp );
        virtual ~CLauncherDocument();

    private: // Constructors

        void ConstructL();
        CLauncherDocument( CEikApplication& aApp );

    private: // Functions from base classes

        CEikAppUi* CreateAppUiL();

        /**
        * From CEikDocument.
        * Always return NULL, so as not to create a INI file.
        * @param aDoOpen ETrue if file exists. (ignored)
        * @param aFilename file to view.
        * @param aFs FileServerSession. (ignored)
        * @return file store. (always NULL)
        */
        CFileStore* OpenFileL(TBool aDoOpen, const TDesC& aFilename,RFs& aFs);

        /**
        * From CEikDocument.
        * @param aFileStore. (ignored)
        * @param aFile.
        */
        void OpenFileL(CFileStore*& aFileStore, RFile& aFile);
};

//===========================================================================
// UI class
//===========================================================================

class CLauncherContainer;
class CDocumentHandler;

/**
 * CLauncherAppUi-class contains all what little intelligence this application
 * has:
 * - OpenFile-functionality implements launching a target app to handle the
 *   content. Launching is done with DocumentHandler and the target app is
 *   launched in embedded mode.
 * - CLauncherAppUi-class also acts as an observer for exit of launched app,
 *   so that this application exits, when the target app does.
 */
class CLauncherAppUi : public CAknAppUi, public MAknServerAppExitObserver
{
    public: // Constructors and destructor

        void ConstructL();
        CLauncherAppUi();
        virtual ~CLauncherAppUi();

    public: // Functions from base classes

        void HandleCommandL( TInt aCommand );

        /**
        * From CEikAppUi.
        * Command line processing. (Comments copied from NpdViewer)
        *
        * Ecmt Menubar launches this application as standalone
        * with the help of RApaLsSession::StartDocument(filename, ...).
        * CEikonEnv::ConstructAppFromCommandLineL(...), at first, passes
        * the filename information to ProcessCommandParametersL
        * correctly. But default CEikAppUi::ProcessCommandParametersL()
        * overwrites it's reference parameter (TFileName& aDocumentName) to
        * application's default document name such as "SdkLauncher.ini".
        * (In EikAppUi.cpp,
        * Application()->GetDefaultDocumentFileName(aDocumentName);
        * does this).
        * So, when CEikonEnv::ConstructAppFromCommandLineL(...) calls
        * CApaDocument::OpenFileL(...), the filename information from
        * Menubar had been lost.
        *
        * So, in order to pass a correct filename information to OpenFileL
        * this function is overridden.
        *
        * The implementation just checks, whether the file exists.
        *
        * @param aCommand command. (ignored)
        * @param aDocumentName Filename.
        * @param aTail optional command line parampeter. (ignored)
        * @return ETrue if aDocumentName file exists.
        */
        TBool ProcessCommandParametersL( TApaCommand aCommand,
                                         TFileName& aDocumentName,
                                         const TDesC8& aTail);
        /**
        * From CEikAppUi.
        * Start this application with specified filename.
        * SDK launcher then uses DocumentHandler to start the proper
        * application to handle the content as embedded application.
        *
        * @param aFilename File to be opened
        */
        void OpenFileL(const TDesC& aFilename);

        /**
        * From MAknServerAppExitObserver
        * Forwards server application exit to registered listeners.
        *
        * SDK launcher always exits, when the embedded application actually
        * handling the file exits.
        */
        void HandleServerAppExit(TInt aReason);

    private: // Data

        CLauncherContainer* iAppContainer; // dummy container
        CDocumentHandler*   iDocHandler;
};

//===========================================================================
// container class
//===========================================================================

/**
 * CLauncherContainer is more or less dummy container. It does not do nothing
 * and in its construction it just specifies that UI of this app is blank i.e.
 * it does not draw anything.
 */
class CLauncherContainer : public CCoeControl
{
    public: // Constructors and destructor
		static CLauncherContainer* NewL( const TRect& aRect );
		static CLauncherContainer* NewLC( const TRect& aRect );

	private:
        void ConstructL( const TRect& aRect );

    private: // Functions from base classes

        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
};

#endif
