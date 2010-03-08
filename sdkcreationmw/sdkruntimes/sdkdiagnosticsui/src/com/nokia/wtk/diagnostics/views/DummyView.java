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

/* javax.swing */
import javax.swing.JLabel;
import javax.swing.JComponent;

/* com.nokia.wtk.util.gui */
import com.nokia.wtk.util.gui.MenuButton;

/**
 * Dummy implementation of {@link ViewAndControlsInterface}.
 * Simply shows a text.
 *
 */
public class DummyView implements ViewAndControlsInterface {
    private JLabel label;
    public DummyView(String text) {
        label = new JLabel(text, JLabel.CENTER);
        label.setVerticalAlignment(JLabel.CENTER);
    }
    public JComponent getControlsPanel() {
        return null;
    }
    public MenuButton getMenuButton() {
        return null;
    }
    public JComponent getPodContent() {
        return label;
    }
}
