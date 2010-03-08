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
* Description:  Panic codes for the expample application.
*
*/


#ifndef HIERARCHICALCOLUMNLIST_PAN
#define HIERARCHICALCOLUMNLIST_PAN


/** HierarchicalColumnList application panic codes */
enum THierarchicalColumnListPanics 
    {
    EHierarchicalColumnListBasicUi = 1
    // add further panics here
    };


// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Panic.
// ---------------------------------------------------------------------------
//
inline void Panic( THierarchicalColumnListPanics aReason )
    {
	_LIT( KApplicationName, "HierarchicalColumnList" );
    User::Panic( KApplicationName, aReason );
    }


#endif // HIERARCHICALCOLUMNLIST_PAN
