/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares UI class for the Tactile Example
*
*/


#ifndef __TACTILEEXAMPLEAPPUI_H__
#define __TACTILEEXAMPLEAPPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CTactileExampleAppView;


// CLASS DECLARATION
/**
 * CTactileExampleAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CTactileExampleAppUi : public CAknAppUi
    {
public:

    /**
     * CTactileExampleAppUi.
     * C++ default constructor. This needs to be public due to
     * the way the framework constructs the AppUi
     */
    CTactileExampleAppUi();

    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * ~CTactileExampleAppUi.
     * Virtual Destructor.
     */
    virtual ~CTactileExampleAppUi();


private:
// from base class CEikAppUi

    /**
     * From CEikAppUi.
     * Takes care of command handling.
     * @param aCommand Command to be handled.
     */
    void HandleCommandL( TInt aCommand );

    /**
     * From CEikAppUi.
     * Called by framework when layout is changed.
     * @param aType the type of resources that have changed
     */
    void HandleResourceChangeL( TInt aType );


private: // Data

    /**
     * The application view
     * Own.
     */
    CTactileExampleAppView* iAppView;
    };

#endif // __TACTILEEXAMPLEAPPUI_H__

// End of File

