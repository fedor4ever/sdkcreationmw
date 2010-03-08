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


package com.nokia.wtk.diagnostics.views;

/**
 * <p>Title: </p>
 * <p>Description: The interface that separates a content panel and the controls
 * on the content.</p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
import javax.swing.JComponent;

import com.nokia.wtk.util.gui.*;

public interface ViewAndControlsInterface {
    public MenuButton getMenuButton();
    public JComponent getControlsPanel();
    public JComponent getPodContent();
}
