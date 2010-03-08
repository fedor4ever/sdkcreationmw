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
#include <avkon.hrh>
#include <eikapp.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <Localization.rsg>
#include <f32file.h>
#include <s32file.h>

#include "Localization.pan"
#include "LocalizationAppUi.h"
#include "LocalizationAppView.h"
#include "Localization.hrh"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CLocalizationAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLocalizationAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL(EAknEnableSkin);
    // Create view object
    iAppView = CLocalizationAppView::NewL( ClientRect() );

    
    }
// -----------------------------------------------------------------------------
// CLocalizationAppUi::CLocalizationAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CLocalizationAppUi::CLocalizationAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CLocalizationAppUi::~CLocalizationAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CLocalizationAppUi::~CLocalizationAppUi()
    {
    if ( iAppView )
        {
        delete iAppView;
        iAppView = NULL;
        }

    }

// -----------------------------------------------------------------------------
// CLocalizationAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CLocalizationAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

		// Number
        case ELocalizationCommandNumber:
            {
				// buffer for localized text
				TBuf<50> myBuf;
				
				// Amount to show
				TReal myAmount = 1234.567;
				
				// Real number formatter, initialized with system's current locale settings
				TRealFormat myFormat;

				// Format real with current locales decimal separator setting
				myBuf.AppendNum(myAmount, myFormat);
				
				// Show formatted text
            	CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
	            informationNote->ExecuteLD( myBuf );
            }
            break;
		
		// Currency
		case ELocalizationCommandCurrency:
			{
				// locale is initialized with system's current locale settings
				TLocale myLocale;
				
				// buffer for localized text
				TBuf<50> myBuf;

				// amount is integer, but it is treated as last two digits
				// were decimal digits e.g. 1249 = 12.49, 2 = 0.02 ...
				TInt myAmount = 123456789;
				
				// Format currency according to current locale settings
				myLocale.FormatCurrency(myBuf, myAmount);

				// Show formatted text				
            	CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
	            informationNote->ExecuteLD( myBuf );
			}
			break;
			
		// Date
		case ELocalizationCommandDate:
			{
				// buffer for localized text
				TBuf<50> myBuf;
				
				// Object for datetime data
				TTime myDate;
				
				// Set current datetime to object
				myDate.HomeTime();
				
				// Format date according to current locale settings
				// Format string is universal, so that whatever the locale is,
				// date is always formatted correctly
				myDate.FormatL(myBuf, _L("%/0%1%/1%2%/2%3%/3%X"));
				
				// Show formatted text				
            	CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
	            informationNote->ExecuteLD( myBuf );
			}
			break;
			
		// Time
		case ELocalizationCommandTime:
			{
				// buffer for localized text
				TBuf<50> myBuf;
				
				// Object for datetime data
				TTime myTime;
				
				// Set current datetime to object
				myTime.HomeTime();

				// Format time to current locale
				// Format string is universal, so that whatever the locale is,
				// time is always formatted correctly
				myTime.FormatL(myBuf, _L("%-B%:0%J%:1%T%:2%S%:3%+B"));
				
				// Show formatted text			
            	CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
	            informationNote->ExecuteLD( myBuf );
			}
			break;
			
		// Text
		case ELocalizationCommandText:
			{
				// buffer for string parameters that may change order
				CDesCArrayFlat* strings = new CDesCArrayFlat( 2 );
				CleanupStack::PushL( strings );

				// Initialize parameter buffer
				
				// First parameter
				strings->AppendL( _L("Heidi") );
				
				// Second parameter
				strings->AppendL( _L("2") );
				
	            // Load a string from the resource file, and append parameters into it
				HBufC* textResource = StringLoader::LoadL( R_LOC_COMMANDTEXT_TEXT, *strings );
				CleanupStack::PushL( textResource );
	            
				// Show formatted text			
	            CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
	            informationNote->ExecuteLD( *textResource );
	            
	            // Cleanupstack
				CleanupStack::PopAndDestroy( textResource );
				CleanupStack::PopAndDestroy( strings );
			}
			break;
			
		// Image
		case ELocalizationCommandImage:
			{
			    // find the drive where the app has been installed
			    TParse parse;
			    parse.Set(Application()->AppFullName(),0,0);
			    	            
				// Load bitmap name from the resource file
            	HBufC* bmpFile = StringLoader::LoadLC( R_LOC_COMMANDIMAGE_BMPFILE, parse.Drive());
            	
				// bitmap pointer
				CFbsBitmap* bitmap;
				
		        // Create and Load the Bitmap
		        bitmap = new( ELeave )CFbsBitmap;
		        CleanupStack::PushL( bitmap );
		        
		        // Load the first bitmap (index 0) from multi-bitmap file
		        User::LeaveIfError(bitmap->Load( *bmpFile, 0 ));
		        
                CleanupStack::Pop( bitmap );		        
		        // Draw bitmap to screen 
		        // ownership of bitmap is transferred to view
		        iAppView->DrawImage( bitmap );

		        // Cleanupstack
            	CleanupStack::PopAndDestroy( bmpFile );
			}
			break;
		
		// Default case
        default:
            break;
        }
    }
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CLocalizationAppUi::HandleStatusPaneSizeChange()
{
	iAppView->SetRect( ClientRect() );
	
} 

// End of File

