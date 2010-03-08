/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares UI class for the Haptics Example.
*
*/


#ifndef C_HAPTICSEXAMPLEAPPUI_H
#define C_HAPTICSEXAMPLEAPPUI_H

#include <aknappui.h>

class CHapticsExampleAppView;
class CHWRMHaptics;
/**
 * CHapticsExampleAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CHapticsExampleAppUi : public CAknAppUi
    {
public:

    /**
     * C++ default constructor. This needs to be public due to
     * the way the framework constructs the AppUi
     */
    CHapticsExampleAppUi();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Virtual Destructor.
     */
    virtual ~CHapticsExampleAppUi();


public:
    
    /**
     * From MEikMenuObserver
     * 
     * Dynamically manages options menu contents based on user actions
     * and data
     *
     * @param aResourceId Id of the resource (i.e., the menu) for which
     *                    to do the contents management.
     * @param aMenuPane   Pointer to the in-memory representation of the menu.
     */
     virtual void DynInitMenuPaneL( TInt aResourceId, 
                                    CEikMenuPane* aMenuPane );

private:

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

private:
    void QueryStrengthSettingL();

private:

    CHapticsExampleAppView* iAppView; // owned 
    };

#endif // C_HAPTICSEXAMPLEAPPUI_H

// End of File

