/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "GuiEngineDocument.h"
#include "GuiEngineApplication.h"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CGuiEngineApplication::CreateDocumentL()
// Create an GuiEngine document, and return a pointer to it
// -----------------------------------------------------------------------------
//
CApaDocument* CGuiEngineApplication::CreateDocumentL()
    {
    return (static_cast<CApaDocument*>( CGuiEngineDocument::NewL( *this ) ) );
    }


// ----------------------------------------------------------------------------
// CGuiEngineApplication::AppDllUid()
// Return the UID for the Ui
// ----------------------------------------------------------------------------
//
TUid CGuiEngineApplication::AppDllUid() const
    {
    return KUidGuiEngineUi;
    }


// ----------------------------------------------------------------------------
// CGuiEngineApplication::EngineDllUid()
// Return the UID for the GuiEngine dll
// ----------------------------------------------------------------------------
//
TUid CGuiEngineApplication::EngineDllUid() const
	{
	return KUidEngineDll;
	}


// End of File
