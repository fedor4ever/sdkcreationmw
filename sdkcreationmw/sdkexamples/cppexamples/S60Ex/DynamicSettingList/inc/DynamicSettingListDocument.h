/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __DYNAMICSETTINGLIST_DOCUMENT_H__
#define __DYNAMICSETTINGLIST_DOCUMENT_H__

// INCLUDE FILES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CDynamicSettingListAppUi;
class CEikApplication;


// CLASS DECLARATION

/**
*  CDynamicSettingListDocument document class.
*/
class CDynamicSettingListDocument : public CAknDocument
    {
public: // Constructors and destructor

	/**
	* Two-phased constructor.
	*/
    static CDynamicSettingListDocument* NewL(CEikApplication& aApp);

	/**
	* Two-phased constructor.
	*/
	static CDynamicSettingListDocument* NewLC(CEikApplication& aApp);

	/**
	* Destructor.
	*/
    ~CDynamicSettingListDocument();

private: // Functions from base classes

    /**
    * From CEikDocument, create CDynamicSettingListAppUi "App UI" object.
    */
    CEikAppUi* CreateAppUiL();

private:

	/**
	* Symbian OS default constructor.
	*/
    void ConstructL();
    CDynamicSettingListDocument(CEikApplication& aApp);

    };


#endif // __DYNAMICSETTINGLIST_DOCUMENT_H__

// End of file