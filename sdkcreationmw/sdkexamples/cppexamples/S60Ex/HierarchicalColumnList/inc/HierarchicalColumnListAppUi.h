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
* Description:  AppUi class for the example application.
*
*/


#ifndef C_HIERARCHICALCOLUMNLISTAPPUI_H
#define C_HIERARCHICALCOLUMNLISTAPPUI_H


#include <aknappui.h>

class CHierarchicalColumnListAppView;


/**
 *  An instance of class CHierarchicalColumnListAppUi is the UserInterface
 *  part of the AVKON application framework for the HierarchicalColumnList
 *  application.
 */
class CHierarchicalColumnListAppUi : public CAknAppUi
    {

public:

    /**
     * Second phase constructor. Performs the second phase construction of a
     * CHierarchicalColumnListAppUi object this needs to be public due to
     * the way the framework constructs the AppUi .
     */
    void ConstructL();

    /**
     * C++ constructor. Performs the first phase of two phase construction.
     * This needs to be public due to the way the framework constructs the
     * AppUi.
     */
    CHierarchicalColumnListAppUi();

    /**
     * Destructor. Destroys the object and releases all memory objects.
     */
    ~CHierarchicalColumnListAppUi();

// from base class CAknAppUi

    /**
     * Dynamically initialises a menu pane.
     */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
     * Handles user menu selections.
     *
     * @param aCommand the enumerated code for the option selected.
     */
    void HandleCommandL( TInt aCommand );

    /**
     * Handles changes in keyboard focus when an application switches to,
     * or from, the foreground.
     */
    void HandleForegroundEventL( TBool aForeground );

    /**
     * Handles a change to the application's resources.
     *
     * @param aType The type of resources that have changed.
     */
    void HandleResourceChangeL( TInt aType );

private:

    /**
     * View The application view.
     * Own.
     */
    CHierarchicalColumnListAppView* iAppView;

    };


#endif // C_HIERARCHICALCOLUMNLISTAPPUI_H

