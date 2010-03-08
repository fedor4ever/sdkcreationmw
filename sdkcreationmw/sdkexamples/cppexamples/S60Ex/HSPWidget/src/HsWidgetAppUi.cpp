/*
 ============================================================================
 Name		: HsWidgetAppUi.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : CHsWidgetAppUi implementation
 ============================================================================
 */

// INCLUDE FILES
#include <eikmenup.h>
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>

#include <HsWidget.rsg>

#include "HsWidget.hlp.hrh"
#include "HsWidget.hrh"
#include "HsWidget.pan"
#include "HsWidgetApplication.h"
#include "HsWidgetAppUi.h"
#include "HsWidgetAppView.h"
#include "HsWidgetExample.h"

_LIT(KFileName, "C:\\private\\E965D1BA\\HsWidget.txt");
_LIT(KText, "Hello World!");

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CHsWidgetAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHsWidgetAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);

	// Create view object
	iAppView = CHsWidgetAppView::NewL(ClientRect() );

	// Create a file to write the text to
	TInt err = CCoeEnv::Static()->FsSession().MkDirAll(KFileName);
	if ( (KErrNone != err) && (KErrAlreadyExists != err))
		{
		return;
		}

	RFile file;
	err = file.Replace(CCoeEnv::Static()->FsSession(), KFileName, EFileWrite);
	CleanupClosePushL(file);
	if (KErrNone != err)
		{
		CleanupStack::PopAndDestroy(1); // file
		return;
		}

	RFileWriteStream outputFileStream(file);
	CleanupClosePushL(outputFileStream);
	outputFileStream << KText;

	CleanupStack::PopAndDestroy(2); // outputFileStream, file

	}
// -----------------------------------------------------------------------------
// CHsWidgetAppUi::CHsWidgetAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHsWidgetAppUi::CHsWidgetAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CHsWidgetAppUi::~CHsWidgetAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CHsWidgetAppUi::~CHsWidgetAppUi()
	{
	if (iAppView)
		{
		delete iAppView;
		iAppView = NULL;
		}
	delete iWidgetExample;
	}

// -----------------------------------------------------------------------------
// CHsWidgetAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CHsWidgetAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case ERegisterWidget:
			{
			if( !iWidgetExample)
				{
				iWidgetExample = CHsWidgetExample::NewL();
				}
			iWidgetExample->RegisterWidget();
			}
			break;
		case EChangeWidgetValues:
			{
			iWidgetExample->ChangeWidgetValuesL();
			}
			break;
		case ERemoveWidget:
			{
			iWidgetExample->RemoveWidget();
			delete iWidgetExample;
			iWidgetExample = NULL;
			}
			break;
		case EHelp:
			{

			CArrayFix<TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
			HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
			}
			break;
		case EAbout:
			{

			CAknMessageQueryDialog* dlg = new (ELeave)CAknMessageQueryDialog();
			dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
			HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
			dlg->QueryHeading()->SetTextL(*title);
			CleanupStack::PopAndDestroy(); //title
			HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
			dlg->SetMessageTextL(*msg);
			CleanupStack::PopAndDestroy(); //msg
			dlg->RunLD();
			}
			break;
		default:
			Panic(EHsWidgetUi);
			break;
		}
	}
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CHsWidgetAppUi::HandleStatusPaneSizeChange()
	{
	iAppView->SetRect(ClientRect() );
	}

CArrayFix<TCoeHelpContext>* CHsWidgetAppUi::HelpContextL() const
	{
//#warning "Please see comment about help and UID3..."
	// Note: Help will not work if the application uid3 is not in the
	// protected range.  The default uid3 range for projects created
	// from this template (0xE0000000 - 0xEFFFFFFF) are not in the protected range so that they
	// can be self signed and installed on the device during testing.
	// Once you get your official uid3 from Symbian Ltd. and find/replace
	// all occurrences of uid3 in your project, the context help will
	// work.
	CArrayFixFlat<TCoeHelpContext>* array = new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
	CleanupStack::PushL(array);
	array->AppendL(TCoeHelpContext(KUidHsWidgetApp, KGeneral_Information));
	CleanupStack::Pop(array);
	return array;
	}


// -----------------------------------------------------------------------------
// CHsWidgetAppUi::DynInitMenuPaneL
// Dimm items depend on widget existing.
// -----------------------------------------------------------------------------
//
void CHsWidgetAppUi::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
	{
	if( aResourceId != R_MENU )
		{
		return;
		}
	if( !iWidgetExample )
		{
	    aMenuPane->SetItemDimmed( ERegisterWidget, EFalse );
	    aMenuPane->SetItemDimmed( EChangeWidgetValues, ETrue );
	    aMenuPane->SetItemDimmed( ERemoveWidget, ETrue );
	    }
	else
		{
		aMenuPane->SetItemDimmed( ERegisterWidget, ETrue );
	    aMenuPane->SetItemDimmed( EChangeWidgetValues, EFalse );
	    aMenuPane->SetItemDimmed( ERemoveWidget, EFalse );
		}
	}

// End of File
