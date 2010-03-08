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





package com.nokia.wtk.diagnostics.gui;

/* java.awt */
import java.awt.BorderLayout;

/* javax.swing */
import javax.swing.JLabel;
import javax.swing.JPanel;

/* com.nokia.wtk.diagnostics.browser */
import com.nokia.wtk.diagnostics.browser.Browser;

/**
 * <p>Title: </p>
 * <p>Description: The default summary view for a pod. This actually should
 * never be instantiated. It just exists as a development testbed/placeholder
 * </p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */

public class DefaultSmallView extends JPanel {

    public DefaultSmallView(Browser b) {
        super(new BorderLayout());
        add(new JLabel("Hello!"), BorderLayout.NORTH);
    }

}
