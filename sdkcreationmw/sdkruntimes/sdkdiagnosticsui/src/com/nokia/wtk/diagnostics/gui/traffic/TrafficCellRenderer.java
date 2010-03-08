package com.nokia.wtk.diagnostics.gui.traffic;/*
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





/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;

import javax.swing.Icon;
import javax.swing.JTree;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.TreeCellRenderer;

import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.diagnostics.views.support.ContentNode;

class TrafficCellRenderer extends DefaultTreeCellRenderer {

    public TrafficCellRenderer() {
        super();
        this.setBorderSelectionColor(
            LookFactory.getCodeColorSet().selectionColor);
        this.setBackgroundSelectionColor(Color.LIGHT_GRAY);
        this.setFont(LookFactory.getStandardPlainFont());

    }
    public Component getTreeCellRendererComponent(
        JTree tree,
        Object value,
        boolean selected,
        boolean expanded,
        boolean leaf,
        int row,
        boolean hasFocus) {
        if (leaf) {
            if (value instanceof TreeCellRenderer) {
                TreeCellRenderer renderer = (TreeCellRenderer) value;
                Component c =
                    renderer.getTreeCellRendererComponent(
                        tree,
                        value,
                        selected,
                        expanded,
                        leaf,
                        row,
                        hasFocus);
                if (c != null) {
                    return c;
                }
            }
        }
        super.getTreeCellRendererComponent(
            tree,
            value,
            selected,
            expanded,
            leaf,
            row,
            hasFocus);

        if (value instanceof ContentNode) {
            ContentNode node = (ContentNode) value;
            Icon icon = node.getIcon();
            if (icon != null) {
                setIcon(icon);
            }
        }

        return this;
    }

    

}
