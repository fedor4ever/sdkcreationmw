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




package com.nokia.wtk.diagnostics.gui.traffic;

import com.nokia.wtk.diagnostics.views.support.ContentNode;

/**
 * A class for ContentNode that have an extra call for a summary.
 * Abstract because each class of ContentNode has to decide how to
 * summarize itself.
 */
abstract class TopLevelContentNode extends ContentNode {
    /**
     * Return a summary line for use in a summary view
     * @return String the summary line
     */
    abstract public String getSummary();
    
    /**
     * Return whether this node should be shown according to the filter.
     * @param f the Filter to test against
     * @return boolean true if this node should be filtered out of the view
     * according to the filter
     */
    abstract boolean isFiltered(Filter f);
}
