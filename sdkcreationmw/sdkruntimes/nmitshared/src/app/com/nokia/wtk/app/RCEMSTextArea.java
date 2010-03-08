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

/* java.awt */
import java.awt.event.MouseEvent;

/* javax.swing */
import javax.swing.Action;
import javax.swing.JTextArea;

/* javax.swing.text */
import javax.swing.text.Document;

/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:  An extension of JTextArea that supports EditMenuSupport and
 * Right clicking consistently.
 * Company:      Nokia
 * @version 1.0
 */

public
class       RCEMSTextArea
extends     JTextArea
implements  EditMenuSupport, RightClickMenuSupport {

    public RCEMSTextArea() { super(); }

    public RCEMSTextArea(String text) { super(text); }

    public RCEMSTextArea(int rows, int columns) { super(rows, columns); }

    public RCEMSTextArea(String text, int rows, int columns) {
        super(text, rows, columns);
    }

    public RCEMSTextArea(Document doc) { super(doc); }

    public RCEMSTextArea(Document doc, String text, int rows, int columns) {
        super(doc, text, rows, columns);
    }

    // Has to be done at document level
    public Action getUndoAction() { return null; }

    // HAs to be done at document level
    public Action getRedoAction() { return null; }
    public Action getCutAction() { return RCEMSText.getCutAction(); }
    public Action getCopyAction() { return RCEMSText.getCopyAction(); }
    public Action getPasteAction() { return RCEMSText.getPasteAction(); }

    // Find? For a text area? Maybe. Not now, though.
    public Action getFindAction() { return null; }

    public Object[] getRightClickActions(MouseEvent e) {
        return  RCEMSText.getRightClickActions(e);
    }
}
