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


#ifndef __DYNAMICSETTINGLIST_APPUI_H__
#define __DYNAMICSETTINGLIST_APPUI_H__

// INCLUDE FILES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CDynamicSettingListAppView;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* 
*/
class CDynamicSettingListAppUi : public CAknAppUi
    {
public: // Constructors and destructor

    /**
    * Symbian OS default constructor.
    */      
    void ConstructL();

    /**
    * Constructor
    */      
    CDynamicSettingListAppUi();


    /**
    * Destructor.
    */      
    ~CDynamicSettingListAppUi();


public: // Functions from base classes
 
    /**
    * From CEikAppUi, takes care of command handling.
    * @param aCommand command to be handled
    */
    void HandleCommandL(TInt aCommand);

private: // Data members

    CDynamicSettingListAppView* iAppView;
    };


#endif // __DYNAMICSETTINGLIST_APPUI_H__

// End of file