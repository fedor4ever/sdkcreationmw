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





package com.nokia.wtk.diagnostics.gui.log;

/* java.awt */
import java.awt.BorderLayout;

/* java.awt.event */
import java.awt.event.MouseEvent;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.Action;
import javax.swing.JPanel;
import javax.swing.JLabel;

/* com.nokia.wtk.app */
import com.nokia.wtk.app.RightClickMenuSupport;

/* com.nokia.wtk.diagnostics.views.support */
import com.nokia.wtk.diagnostics.views.support.PopLabel;

/**
 * A GUI for viewing last line in the log.
 */
class LogSummaryView extends JPanel implements RightClickMenuSupport
{
    private PopLabel textLabel;
    private JLabel iconLabel;
    private Action logToFileAction;
    private Action[] rightClickActions;

    LogSummaryView(Action logAction) {
        super(new BorderLayout());
        logToFileAction = logAction;
        textLabel = new PopLabel();
        iconLabel = new JLabel();
        iconLabel.setVerticalAlignment(JLabel.CENTER);
        add(iconLabel, BorderLayout.WEST);
        add(textLabel, BorderLayout.CENTER);
    }

    void clear() {
        textLabel.setText(" ");
    }

    void setText(String text) {
        if (text == null || text.length() == 0) {
            clear();
        } else {
            textLabel.setText(text);
        }
    }

    void setIcon(Icon icon) {
        iconLabel.setIcon(icon);
        if (icon == null) {
            iconLabel.setText("");
        } else {
            iconLabel.setText(" ");
        }
    }

    void setIconToolTip(String tip) {
        iconLabel.setToolTipText(tip);
    }

    // RightClickMenuSupport
    public Object[] getRightClickActions(MouseEvent e) {
        if (rightClickActions == null) {
            rightClickActions = new Action[] {
                logToFileAction
            };
        }
        return rightClickActions;
    }
}
