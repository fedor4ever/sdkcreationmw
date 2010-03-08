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


package com.nokia.wtk.app;

/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:  Printable interface that contains everything we need for
 * proper printing.
 * Company:      Nokia
 * @version 1.0
 */

import java.awt.print.Printable;

import javax.swing.JComponent;

public interface AppPrintable extends Printable {

    /**
     * Gets the name of the item to be printed for identification on the page.
     */
    public String getName();

    /**
     * Gets a comment that may need to be printed as well.
     */
    public String getComment();

    /**
     * Gets a JComponent that need to be presented to the user to set some
     * printing parameters.
     */
    public JComponent getPrintPanel();
}
