/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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



package com.nokia.wtk.util.gui;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.PropertySet;

/**
 * Interface for instantiating property pages.
 *
 */
public interface PropPageFactory {

    /**
     * Creates a new property page for editing the specified property set
     * or a subset of it.
     *
     * @param ps the property set to edit
     * @return the property page, or <code>null</code> if this
     *         property set does not contain any properties editable
     *         by this property page
     */
    public PropPage createPropertyPage(PropertySet ps);
}
