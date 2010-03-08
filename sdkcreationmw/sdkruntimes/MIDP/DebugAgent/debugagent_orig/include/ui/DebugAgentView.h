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


#ifndef HEADER_DEBUG_AGENT_UI_VIEW
#define HEADER_DEBUG_AGENT_UI_VIEW

#include <coemain.h>
#include <coecntrl.h>
#include <s32file.h>
#include <txtglobl.h>
#include <frmtview.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include "ui/DebugAgentAppUi.h"

#include "Driver.h"
#include "UiDriver.h"

/**
Draws to screen.
*/
class CDebugAgentView : public CCoeControl 
    {
public:
    static CDebugAgentView* NewL(const TRect& aRect, CDebugAgentAppUi& aAppUi);
    virtual ~CDebugAgentView();

	void StartL();
	void AppendL(const TDesC& aString);
	void Clear();
	void Shutdown();
	void MakeOutgoingConnectionL();
	
	// CCoeControl
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	
	CDriver* Driver() { return iDriver; };

private:	
	// from CCoeControl
    void Draw(const TRect& aRect) const;

private:
	CDebugAgentView(CDebugAgentAppUi& aAppUi);
    void ConstructL(TRect aRect);

private:
	CDebugAgentAppUi&	iAppUi;

private:
	CGlobalText*		iGlobalText; // global text object
	CParaFormatLayer*	iParaFormatLayer;
	CCharFormatLayer*	iCharFormatLayer;
	TStreamId			iStreamId; // required when storing and restoring global text
	// text layout and view stuff
	CTextLayout*		iLayout; // text layout
	CTextView*			iTextView; // text view
	TRect				iViewRect; // rectangle through which to view text
	TInt 				iLineCounter;
private:
	CUiDriver* 			iDriver;
	TBool				iDriverStarted;
    };

#endif


