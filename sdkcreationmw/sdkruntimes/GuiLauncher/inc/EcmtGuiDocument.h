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


#ifndef __ECMTGUI_DOCUMENT_H__
#define __ECMTGUI_DOCUMENT_H__


// INCLUDE FILES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CEcmtGuiAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
*  @ingroup EcmtGui
*/
class CEcmtGuiDocument : public CAknDocument
    {
	public:
        static CEcmtGuiDocument* NewL(CEikApplication& aApp, TUid aUid);
        static CEcmtGuiDocument* NewLC(CEikApplication& aApp, TUid aUid);
	    ~CEcmtGuiDocument();

	public:
	    CEikAppUi* CreateAppUiL();
       TUid GetUid();

	private:
	    void ConstructL();
       CEcmtGuiDocument(CEikApplication& aApp, TUid aUid);

    private:
        TUid iUid;
    };


#endif // __ECMTGUI_DOCUMENT_H__
