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




package com.nokia.wtk.diagnostics.views.support;

import java.awt.Component;
import java.awt.Container;

import javax.swing.JComponent;
import javax.swing.JScrollBar;
import javax.swing.JTextArea;
import javax.swing.text.JTextComponent;

import com.nokia.wtk.app.LookFactory;

/**
 *
 * To change this generated comment go to 
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class Misc {    
    public static void setOpaque(Component c, boolean opaque) {
        if (c instanceof JComponent) {
            if (c instanceof javax.swing.JScrollBar) {
                // No transparent scrollbars, please
                return;
            }
            ((JComponent) c).setOpaque(opaque);
        }
    
        if (c instanceof Container) {
            Container cont = (Container) c;
            int num = cont.getComponentCount();
    
            for (int i = 0; i < num; i++) {
                setOpaque(cont.getComponent(i), opaque);
            }
        }
        c.setFont(LookFactory.getStandardPlainFont());
    }

    /**
     * @return a JEditorPane
     */
    public static JTextComponent createHTMLViewer() {
        JTextArea view = new JTextArea(2, 40); // Not RCEMS -- no need to copy
        view.setEditable(false);
        view.setLineWrap(true);
        return view;
    }
}
