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


// INCLUDE FILES
#include <eikstart.h>
#include <coeutils.h>
#include <apgcli.h>
#include "launcherUID.hrh"
#include "launcher.h"
#include <DocumentHandler.h>
#include <AiwGenericParam.h>

//===========================================================================
// Debug trace
//===========================================================================

#ifdef _DEBUG

#define TRACE_PREFIX "SDK Launcher: "

#  ifdef TRACE_PREFIX
#    if defined(_UNICODE)
#      define ___L(a,b) _L(a L##b)
#    else  // _UNICODE
#      define ___L(a,b) _L(a##b)
#    endif // _UNICODE
#    define __L(z) ___L(TRACE_PREFIX,z)
#  else
#    define __L(z) _L(z)
#  endif // TRACE_PREFIX

#  define TRACE(s)                  RDebug::Print(__L("%s"),_S(s))
#  define TRACE1(s,p1)              RDebug::Print(__L(s),p1)

#else  /* !_DEBUG */

#  define TRACE(s)                  ((void)0)
#  define TRACE1(s,p1)              ((void)0)

#endif /* !_DEBUG */

namespace
{
    const TUid ownUid = { KSDKLauncherUID3 };
}

//===========================================================================
// entrypoint and factory
//===========================================================================

EXPORT_C CApaApplication* NewApplication()
{
    return ( new CLauncherApplication );
}

GLDEF_C TInt E32Main()
{
    return EikStart::RunApplication( NewApplication );
}

//===========================================================================
// application class
//===========================================================================

CApaDocument* CLauncherApplication::CreateDocumentL()
{
    return CLauncherDocument::NewL( *this );
}

TUid CLauncherApplication::AppDllUid() const
{
    //TUid uid = { KSDKLauncherUID3 };
    return ownUid;
}

//===========================================================================
// document class
//===========================================================================

CLauncherDocument::CLauncherDocument( CEikApplication& aApp )
  : CEikDocument( aApp )
{
}

CLauncherDocument* CLauncherDocument::NewL( CEikApplication& aApp )
{
    CLauncherDocument* self = NewLC( aApp );
    CleanupStack::Pop();
    return self;
}

CLauncherDocument* CLauncherDocument::NewLC( CEikApplication& aApp )
{
    CLauncherDocument* self = new (ELeave) CLauncherDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

void CLauncherDocument::ConstructL()
{
}

CLauncherDocument::~CLauncherDocument()
{
}

CEikAppUi* CLauncherDocument::CreateAppUiL()
{
    return new (ELeave) CLauncherAppUi;
}

CFileStore* CLauncherDocument::OpenFileL( TBool /*aDoOpen*/,
                                          const TDesC& aFilename,
                                           RFs& /*aFs*/)
{
    TRACE1("OpenFileL(1): %S", &aFilename);

    // UI takes care of actual file opening
    iAppUi->OpenFileL(aFilename);
    return NULL;
}

void CLauncherDocument::OpenFileL( CFileStore*& /*aStore*/, RFile& aFile)
{
    TFileName file;
    User::LeaveIfError( aFile.FullName(file) );
    aFile.Close();

    TRACE1("OpenFileL(2): %S", &file);

    // UI takes care of actual file opening
    iAppUi->OpenFileL(file);
}

//===========================================================================
// UI class
//===========================================================================

CLauncherAppUi::CLauncherAppUi()
  : iAppContainer(NULL)
  , iDocHandler(NULL)
{
}

void CLauncherAppUi::ConstructL()
{
    BaseConstructL(EAknEnableSkin);
    iAppContainer = CLauncherContainer::NewL( ApplicationRect() );
    AddToStackL( iAppContainer );

    iDocHandler = CDocumentHandler::NewL();

    // register this class as observer for exiting of launched apps
    iDocHandler->SetExitObserver(this);
}

CLauncherAppUi::~CLauncherAppUi()
{
    if (iAppContainer)
    {
        iEikonEnv->RemoveFromStack( iAppContainer );
        delete iAppContainer;
        iAppContainer = NULL;
    }
    if (iDocHandler)
    {
        delete iDocHandler;
        iDocHandler = NULL;
    }
}

void CLauncherAppUi::HandleCommandL( TInt aCommand )
{
    TRACE1("HandleCommandL(): aCommand: %i", aCommand);
    switch( aCommand )
    {
        case EEikCmdExit:
        case EAknSoftkeyExit:
        case EAknSoftkeyBack:
            Exit();
            break;

        default:
            break;
    }
}

TBool CLauncherAppUi::ProcessCommandParametersL( TApaCommand /* aCommand */,
                                                 TFileName& aDoc,
                                                 const TDesC8& /* aTail */ )
{
    TRACE1("ProcessCommandParametersL(): %S", &aDoc);

    // never call CEikAppUi::ProcessCommandParametersL(..) because
    // it is so harmfull to overwrite and destroy aDocumentName.
    //
    return ConeUtils::FileExists(aDoc);
}

void CLauncherAppUi::OpenFileL(const TDesC& aFileName)
{
    TRACE("AppUI OpenFileL()");

    // todo: url-handling is missing. Which application use that? How to find
    // out?
    //
    CAiwGenericParamList* param = CAiwGenericParamList::NewLC();
    TAiwVariant yes(ETrue);
    //param->AppendL(TAiwGenericParam(EGenericParamURL,TAiwVariant(urlPtr)));
    param->AppendL(TAiwGenericParam(EGenericParamAllowSave, yes));
    param->AppendL(TAiwGenericParam(EGenericParamAllowMove, yes));

    RFile file;
    iDocHandler->OpenTempFileL(aFileName, file);
    CleanupClosePushL(file);

    TDataType emptyType;
    iDocHandler->OpenFileEmbeddedL(file, emptyType, *param);

    TRACE("file given to DocHandler");

    // the last thing to do it to change this launcher's caption to be the
    // same as the launched app's.
    TUid uid;
    if (KErrNone == iDocHandler->HandlerAppUid(uid))
    {
        TApaAppInfo info;
        RApaLsSession apaLs;
        if ((KErrNone == apaLs.Connect())
            && (KErrNone == apaLs.GetAppInfo(info, uid)))
        {
            // we don't care, if this fails.
            apaLs.SetAppShortCaption(info.iShortCaption, ELangNone, ownUid);
            apaLs.Close();
        }
    }

    CleanupStack::PopAndDestroy(2); // file, param
}

void CLauncherAppUi::HandleServerAppExit( TInt aReason )
{
    TRACE1("HandleServerAppExit(): reason %i", aReason);

    // we always exit, when the launched (embedded) application exits
    Exit();
}


//===========================================================================
// container class
//===========================================================================

CLauncherContainer* CLauncherContainer::NewL( const TRect& aRect )
{
    CLauncherContainer* self = CLauncherContainer::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
}

CLauncherContainer* CLauncherContainer::NewLC( const TRect& aRect )
{
    CLauncherContainer* self = new ( ELeave ) CLauncherContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
}

void CLauncherContainer::ConstructL( const TRect& aRect )
{
    CreateWindowL();
    SetRect( aRect );
    SetBlank(); // this app never Draws anything
    ActivateL();
}

TInt CLauncherContainer::CountComponentControls() const
{
    return 0; // there are no components
}

CCoeControl* CLauncherContainer::ComponentControl( TInt /*aIndex*/ ) const
{
    return NULL; // there are no components
}

// End of File
