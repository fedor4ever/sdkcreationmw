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
#include <eikstart.h>
#include "IsvTelCallAppApplication.h"
#include "IsvTelCallAppUi.h"
#include "IsvTelCallAppDocument.h" 


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// TUid CIsvTelCallAppApplication::AppDllUid()
// Returns application UID.
// ----------------------------------------------------------------------------
//
TUid CIsvTelCallAppApplication::AppDllUid() const
    {
    return KUidIsvTelCallApp;
    }

// ----------------------------------------------------------------------------
// CApaDocument* CIsvTelCallAppApplication::CreateDocumentL()
// Creates CIsvTelCallAppDocument object.
// ----------------------------------------------------------------------------
//
CApaDocument* CIsvTelCallAppApplication::CreateDocumentL()
    {
    return CIsvTelCallAppDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS =================================


// ----------------------------------------------------------------------------
// CApaApplication* NewApplication()
// Constructs CIsvTelCallAppApplication.
// Returns: CApaApplication*: created application object
// ----------------------------------------------------------------------------
//

LOCAL_C CApaApplication* NewApplication()
    {
    return ( static_cast< CApaApplication* > (new CIsvTelCallAppApplication ) );
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }


// End of File

