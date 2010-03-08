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
* Description:  Override dialogs provided by the Browser Control
*
*/


// INCLUDE FILES
#include "BrCtlSampleAppDialogsProvider.h"
#include "BrCtlSampleAppQueryDialog.h"
#include "BrCtlSampleAppContainer.h"

#include <eiklabel.h>  // for example label control
#include <avkon.hrh>
#include <BrCtlSampleApp.rsg>
#include "brctlsampleapp.hrh"
#include <BrCtlInterface.h>
#include <CHARCONV.H>
#include <gdi.h>
#include <bitdev.h>
#include <aknmessagequerydialog.h>		// DialogAlertL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBrCtlSampleAppDialogsProvider::DialogAlertL
// ---------------------------------------------------------
//
void 
CBrCtlSampleAppDialogsProvider::DialogAlertL(const TDesC& aTitle, const TDesC& aMessage)
    {
    _LIT(KEndMessage, "\n\n");
    HBufC* messagebody = HBufC::NewLC( aMessage.Length()+2 );
    messagebody->Des().Append( aMessage );
    messagebody->Des().Append(KEndMessage);

    HBufC* header = HBufC::NewLC( aTitle.Length() );
	header->Des().Append( aTitle );

    CAknMessageQueryDialog* dialog = 
                            CAknMessageQueryDialog::NewL( *messagebody );

    dialog->PrepareLC( R_AVKON_MESSAGE_QUERY_DIALOG );
    dialog->ButtonGroupContainer().MakeCommandVisible( 
													EAknSoftkeyCancel, EFalse );

    dialog->QueryHeading()->SetTextL( *header );

    dialog->RunLD();

    CleanupStack::PopAndDestroy( 2 );  // header , messagebody 
    }

// ---------------------------------------------------------
// CBrCtlSampleAppDialogsProvider::ConstructL
// ---------------------------------------------------------
//
void CBrCtlSampleAppDialogsProvider::ConstructL( CBrCtlSampleAppContainer* aContainer)
    {
    iContainer = aContainer;
    }

// ---------------------------------------------------------
// CBrCtlSampleAppDialogsProvider::~CBrCtlSampleAppDialogsProvider
// ---------------------------------------------------------
//
CBrCtlSampleAppDialogsProvider::~CBrCtlSampleAppDialogsProvider()
    {
    }

// ---------------------------------------------------------
// CBrCtlSampleAppDialogsProvider::NewL
// ---------------------------------------------------------
//
CBrCtlSampleAppDialogsProvider* CBrCtlSampleAppDialogsProvider::NewL(CBrCtlSampleAppContainer* aContainer)
{
  CBrCtlSampleAppDialogsProvider* self = new(ELeave)CBrCtlSampleAppDialogsProvider;
  CleanupStack::PushL(self);
  self->ConstructL(aContainer);
  CleanupStack::Pop();
  return self;
}

// End of File  

