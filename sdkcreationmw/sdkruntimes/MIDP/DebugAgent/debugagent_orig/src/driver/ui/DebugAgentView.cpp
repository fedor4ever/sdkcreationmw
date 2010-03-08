/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <eikenv.h>

#include "ui/DebugAgentView.h"
#include "ui/DebugAgentDocument.h"
#include "SEIConnector.h"
#include "CommandLineArguments.h"





CDebugAgentView* CDebugAgentView::NewL(const TRect& aRect, CDebugAgentAppUi& aAppUi )
    {
    CDebugAgentView* self = new(ELeave) CDebugAgentView(aAppUi);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    CleanupStack::Pop();
    return self;
    }

CDebugAgentView::CDebugAgentView(CDebugAgentAppUi& aAppUi) 
	: iAppUi(aAppUi), iLineCounter(0), iDriver(NULL), iDriverStarted(EFalse)
	{
	}

void CDebugAgentView::ConstructL(TRect aRect)
    {
    CreateWindowL();
	SetRect(aRect);
	ActivateL();
	// Create text object, text view and layout.
	iParaFormatLayer=CParaFormatLayer::NewL(); // required para format layer
	iCharFormatLayer=CCharFormatLayer::NewL(); // required char format layer
	// Create an empty rich text object
	iGlobalText=CGlobalText::NewL(iParaFormatLayer, iCharFormatLayer);
	// prerequisites for view - viewing rectangle
	iViewRect=Rect();
	iViewRect.Shrink(3,3);
	// context and device
	CWindowGc& gc=SystemGc(); // get graphics context
	CBitmapDevice *device=(CBitmapDevice*) (gc.Device()); // device
	// Create the text layout, (required by text view),
	// with the text object and a wrap width (=width of view rect)
	iLayout=CTextLayout::NewL(iGlobalText,iViewRect.Width());
	// Create text view
	iTextView=CTextView::NewL(iLayout, iViewRect,
			device,
			device,
			&Window(),
			0, // no window group
			&iCoeEnv->WsSession()
			); // new view
	iDriver = CUiDriver::NewL(this);
	
	//handle command line arguments
	CCommandLineArguments* args = CCommandLineArguments::NewL();
	if(args->GetLocalPort() != KErrNotFound)
		{
		if(args->GetRemotePort() != KErrNotFound)
			{
			//workaround for connections initialization (i.e. unit tests)
			//get an IP address from DHCP server and open network interfaces
			TRAP_IGNORE(iDriver->PingL(*(args->GetRemoteHost()), args->GetRemotePort()));
			}
		iDriver->InitL();
		iDriver->SetAgentPort(args->GetLocalPort());
		iDriver->StartTcpServerL();
		iDriverStarted = ETrue;
		}
	}

CDebugAgentView::~CDebugAgentView()
    {
	if ( iDriver != NULL )
		{
		delete iDriver;
		}
	delete iTextView; // text view
	delete iLayout; // text layout
	delete iGlobalText; // contained text object
	delete iCharFormatLayer; // char format layer
	delete iParaFormatLayer; // para format layer
    }

void CDebugAgentView::StartL()
	{
	if (iDriverStarted) return;
	if ( iDriver != NULL )
		{
		delete iDriver;
		}
	iDriver = CUiDriver::NewL(this);
	iDriver->InitL();
	iDriver->StartTcpServerL();
	iDriverStarted = ETrue;
	}
	
void CDebugAgentView::Clear()
	{
	// Reset document. 
	//iGlobalText->Reset(); // change whole document!
	//DrawNow();
	}

void CDebugAgentView::Shutdown()
	{
	if ( iDriver != NULL )
		{
		iDriver->Shutdown();
		iDriverStarted = EFalse;
		}
	}
	
	
void CDebugAgentView::MakeOutgoingConnectionL()
	{
	if(!iDriverStarted) 
		{
		StartL();
		}
	CSEIConnector* connector = CSEIConnector::NewL(iDriver);
	connector->MakeOutgoingConnectionL
		(iDriver->GetPCHost()->Des(), iDriver->GetPCPort());
	}
	
void CDebugAgentView::AppendL(const TDesC& aString)
	{
	iLineCounter++;
	
	TInt pos = iGlobalText->LdDocumentLength();

	TBuf<30> dateString;
	_LIT(KDateString5,"%-B%:0%J%:1%T%:2%S%:3");
	_LIT(KDelim," : ");
	TTime time;
	time.HomeTime();
	time.FormatL(dateString,KDateString5);
	
	iGlobalText->InsertL(
		iGlobalText->LdDocumentLength(),
		dateString);
		
	iGlobalText->InsertL(
		iGlobalText->LdDocumentLength(),
		KDelim);
		
	iGlobalText->InsertL(
		iGlobalText->LdDocumentLength(),
		aString);
		
	iGlobalText->InsertL(
		iGlobalText->LdDocumentLength(),
		CEditableText::EParagraphDelimiter);

	iTextView->FormatTextL();

	TInt height = Rect().Height()-20;
	iTextView->SetViewL(pos, height);
	

	DrawNow();
	}

void CDebugAgentView::Draw(const TRect& aRect) const
    {
	// draw surround
	CGraphicsContext& gc=SystemGc(); // context to draw into
	TRect rect=Rect(); // screen boundary
	gc.DrawRect(rect); // outline screen boundary
	rect.Shrink(1,1);
	gc.SetPenColor(KRgbWhite);
	gc.DrawRect(rect);
	rect.Shrink(1,1);
	gc.SetPenColor(KRgbBlack);
	gc.DrawRect(rect);
	// draw editable text - will work unless OOM
	TRAPD(err,iTextView->DrawL(aRect));
	}
	
TKeyResponse CDebugAgentView::OfferKeyEventL(
	const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType!=EEventKey) return EKeyWasNotConsumed;
	TInt code=aKeyEvent.iCode;
	TInt delta;
	switch (code)
		{
		case EKeyDownArrow:
			delta = -1;
			break;
		case EKeyUpArrow:
			delta = 1;
			break;
		case EKeyPageDown:
			delta = -10;
			break;
		case EKeyPageUp:
			delta = 10;
			break;
		default:
			return EKeyWasNotConsumed;
		}
	iTextView->ScrollDisplayLinesL(delta);
	return EKeyWasConsumed;
	}
