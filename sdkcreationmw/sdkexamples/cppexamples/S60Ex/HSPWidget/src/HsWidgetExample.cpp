/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "HsWidgetExample.h"
#include <HsWidget.rsg>
#include <hswidgetpublisher.h>
#include <hswidget.h>
#include <hsexception.h>
#include <hsdataobserver.h>

#include <coemain.h> 
#include <eikenv.h> 
#include <utf.h> 
#include <apgtask.h>

const char* image = "image1";
const char* text = "text1";

// -----------------------------------------------------------------------------
// CHsWidgetExample::CHsWidgetExample()
// 
// -----------------------------------------------------------------------------
//
CHsWidgetExample::CHsWidgetExample()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::~CHsWidgetExample()
// 
// -----------------------------------------------------------------------------
//

CHsWidgetExample::~CHsWidgetExample()
	{
	delete iHsWidgetPublisher;
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::NewLC()
// 
// -----------------------------------------------------------------------------
//
CHsWidgetExample* CHsWidgetExample::NewLC()
	{
	CHsWidgetExample* self = new (ELeave)CHsWidgetExample();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::NewL()
// 
// -----------------------------------------------------------------------------
//
CHsWidgetExample* CHsWidgetExample::NewL()
	{
	CHsWidgetExample* self=CHsWidgetExample::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::ConstructL()
// Create inscance of HSPApi
// -----------------------------------------------------------------------------
//
void CHsWidgetExample::ConstructL()
	{
	try
		{
		iHsWidgetPublisher = new Hs::HsWidgetPublisher( this );
		}
	catch( Hs::HsException& e)
		{
		//catch error from HSPApi
		User::Leave( e.getReason() );
		}
	catch( ... )
		{
		//catch error from other libraries
		User::Leave( KErrGeneral );
		}
	//converitng string from UNICODE to UTF-8 std::string
	HBufC* buf1 = CCoeEnv::Static()->AllocReadResourceL( R_TEMPLATE_TYPE );
	templateType = ToString( *buf1 );
	delete buf1;
	buf1 = CCoeEnv::Static()->AllocReadResourceL( R_WIDGET_NAME );
	widgetName = ToString( *buf1 );
	delete buf1;
	buf1 = CCoeEnv::Static()->AllocReadResourceL( R_WIDGET_ID );
	widgetId = ToString( *buf1 );
	delete buf1;
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::RegisterWidget()
// 
// -----------------------------------------------------------------------------
//
void CHsWidgetExample::RegisterWidget()
	{
	try
		{
		iHsWidgetPublisher->createHsWidget( templateType, widgetName, widgetId );
		}
	catch( Hs::HsException& e)
		{
		//catch error from HSPApi
		User::Leave( e.getReason() );
		}
	catch( ... )
		{
		//catch error from other libraries
		User::Leave( KErrGeneral );
		}
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::PublishWidget()
// Template is published by HSPApi. If HS gets template's data it obtain actual
// information. 
// -----------------------------------------------------------------------------
//
void CHsWidgetExample::PublishWidget()
	{
	try
		{	
		iHsWidgetPublisher->publishHsWidget( 
			iHsWidgetPublisher->getHsWidget( templateType, widgetName,
				widgetId ) );
		}
	catch( Hs::HsException& e)
		{
		//catch error from HSPApi
		User::Leave( e.getReason() );
		}
	catch( ... )
		{
		//catch error from other libraries
		User::Leave( KErrGeneral );
		}
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::ChangeWidgetValues()
// Change templates items values.
// -----------------------------------------------------------------------------
//
void CHsWidgetExample::ChangeWidgetValuesL()
	{
	try
		{
		HBufC* buf1 = NULL;
		HBufC* buf2 = NULL;
		if( !( iCounter % 2 ) )
			{
			buf1 = CCoeEnv::Static()->AllocReadResourceL( R_FILE_BEGINING );
			buf2 = CCoeEnv::Static()->AllocReadResourceL( R_TEXT_BEGINING );
			}
		else
			{
			buf1 = CCoeEnv::Static()->AllocReadResourceL( R_FILE_UPDATED );
			buf2 = CCoeEnv::Static()->AllocReadResourceL( R_TEXT_UPDATED );
			}
		
		iHsWidgetPublisher->getHsWidget( templateType, widgetName, widgetId ).
			setItem( image, ToString( *buf1 ) );
		iHsWidgetPublisher->getHsWidget( templateType, widgetName, widgetId ).
			setItem( text, ToString( *buf2 ) );
		
		delete buf1;
		delete buf2;
		}
	catch( Hs::HsException& e)
		{
		//catch error from HSPApi
		User::Leave( e.getReason() );
		}
	catch( ... )
		{
		//catch error from other libraries
		User::Leave( KErrGeneral );
		}
	iCounter++;
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::RemoveWidget()
// Remove template from HSPApi. 
// -----------------------------------------------------------------------------
//
void CHsWidgetExample::RemoveWidget()
	{
	try
		{	
		iHsWidgetPublisher->removeHsWidget( templateType, widgetName, widgetId);
		}
	catch( Hs::HsException& e)
		{
		//catch error from HSPApi
		User::Leave( e.getReason() );
		}
	catch( ... )
		{
		//catch error from other libraries
		User::Leave( KErrGeneral );
		}
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::handleEvent
// Template is being published when comming action is EActivated or EResumed
// -----------------------------------------------------------------------------
//
void CHsWidgetExample::handleEvent( std::string /*aWidgetName*/, 
	Hs::IHsDataObserver::EEvent aEvent )	
	{
	if ( aEvent == EResume )
		{
		PublishWidget();
		}
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::handleItemEvent
// Brings application to foreground.
// -----------------------------------------------------------------------------
//
void CHsWidgetExample::handleItemEvent( std::string /*aWidgetName*/,
    std::string /*aTemplateItemName*/,
    Hs::IHsDataObserver::EItemEvent /*aEvent*/)
	{
	TApaTask task( CEikonEnv::Static()->WsSession() );
	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
	task.BringToForeground();
	}

// -----------------------------------------------------------------------------
// CHsWidgetExample::ToString
// Converts string from UNICODE to UTF-8 std::string.
// Add one extra character fot ending zero. 
// -----------------------------------------------------------------------------
//
std::string CHsWidgetExample::ToString(const TDesC& aText)
    {
    HBufC8* text = HBufC8::NewL( aText.Length() + 4 /*for ending zero*/ );
    TPtr8 dest( text->Des() );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( dest, aText );    
    std::string ret((const char*)dest.PtrZ());
    delete text;
    return ret;
    }

