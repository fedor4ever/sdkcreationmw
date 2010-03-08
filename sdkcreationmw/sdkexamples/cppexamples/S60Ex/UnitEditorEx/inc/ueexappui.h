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



#ifndef __UEEX_APPUI_H__
#define __UEEX_APPUI_H__

// INCLUDES
#include <aknappui.h>


// CLASS DECLARATION
/**
* CUEExAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CUEExAppUi : public CAknAppUi
    {
public:

    /**
    * ConstructL.
    * 2nd phase constructor.
    */
    void ConstructL();


    /**
    * CUEExAppUi.
    * C++ default constructor. This needs to be public due to
    * the way the framework constructs the AppUi
    */
    CUEExAppUi();


    /**
    * ~CUEExAppUi.
    * Virtual Destructor.
    */
    virtual ~CUEExAppUi();

public: // from CAknAppUi

    /**
    * From CEikAppUi, HandleCommandL.
    * Takes care of command handling.
    * @param aCommand Command to be handled.
    */
    void HandleCommandL( TInt aCommand );
    };


#endif // __UEEX_APPUI_H__

