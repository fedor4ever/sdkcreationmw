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
* Description:  Application class for the example application.
*
*/


#ifndef C_HIERARCHICALCOLUMNLISTAPPLICATION_H
#define C_HIERARCHICALCOLUMNLISTAPPLICATION_H


#include <aknapp.h>


/** 
 *  An instance of CHierarchicalColumnListApplication is the application
 *  part of the AVKON application framework for the HierarchicalColumnList
 *  application.
 */
class CHierarchicalColumnListApplication : public CAknApplication
    {

public:

// from base class CAknApplication

    /**
     * Returns the application DLL UID value.
     *
     * @return the UID of this Application/Dll
     */
    TUid AppDllUid() const;

protected:

// from base class CAknApplication

    /**
     * Creates a CApaDocument object and return a pointer to it.
     *
     * @return a pointer to the created document
     */
    CApaDocument* CreateDocumentL();

    };

#endif // C_HIERARCHICALCOLUMNLISTAPPLICATION_H
