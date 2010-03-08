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
* Description:  Document class for the example application.
*
*/


#ifndef C_HIERARCHICALCOLUMNLISTDOCUMENT_H
#define C_HIERARCHICALCOLUMNLISTDOCUMENT_H


#include <akndoc.h>

class CHierarchicalColumnListAppUi;
class CEikApplication;


/** 
 *  An instance of class CHierarchicalColumnListDocument is the Document
 *  part of the AVKON application framework for the HierarchicalColumnList
 *  application.
 */
class CHierarchicalColumnListDocument : public CAknDocument
    {

public:

    /**
     * Two phased constructor. Constructs a CHierarchicalColumnListDocument
     * for the AVKON application aApp using two phase construction, and
     * return a pointer to the created object.
     *
     * @param aApp application creating this document
     * @return a pointer to the created instance of
     *      CHierarchicalColumnListDocument
     */
    static CHierarchicalColumnListDocument* NewL( CEikApplication& aApp );

    /**
     * Two phased constructor. Constructs a CHierarchicalColumnListDocument
     * for the AVKON application aApp using two phase construction, and
     * returns a pointer to the created object.
     *
     * @param aApp application creating this document
     * @return a pointer to the created instance of 
     *      CHierarchicalColumnListDocument
     */
    static CHierarchicalColumnListDocument* NewLC( CEikApplication& aApp );

    /**
     * Destructor. Destroys the object and releases all memory objects.
     */
    ~CHierarchicalColumnListDocument();

// from base class CAknDocument
    
    /**
     * From CAknDocument.
     * Create a CHierarchicalColumnListAppUi object and return a pointer to it.
     *
     * @return a pointer to the created instance of the AppUi created
     */
    CEikAppUi* CreateAppUiL();

private:

    /**
     * Second phase constructor of a CHierarchicalColumnListDocument object.
     */
    void ConstructL();

    /**
     * C++ constructor. Performs the first phase of two phase construction.
     *
     * @param aApp application creating this document
     */
    CHierarchicalColumnListDocument( CEikApplication& aApp );

    };


#endif // C_HIERARCHICALCOLUMNLISTDOCUMENT_H
