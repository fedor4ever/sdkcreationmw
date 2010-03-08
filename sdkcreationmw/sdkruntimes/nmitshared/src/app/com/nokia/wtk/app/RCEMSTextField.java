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
import javax.swing.JTextField;

/* javax.swing.text */
import javax.swing.text.Document;


/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:
 * Company:      Nokia
 * @version 1.0
 */

public
class       RCEMSTextField
extends     JTextField
implements  EditMenuSupport, RightClickMenuSupport {

    public RCEMSTextField() { super(); }

    public RCEMSTextField(Document doc, String text, int columns) {
        super(doc, text, columns);
    }

    public RCEMSTextField(int columns) {
        super(columns);
    }

    public RCEMSTextField(String text) {
        super(text);
    }

    public RCEMSTextField(String text, int columns) {
        super(text, columns);
    }

    // Has to be done at document level
    public Action getUndoAction() { return null; }

    // HAs to be done at document level
    public Action getRedoAction() { return null; }
    public Action getCutAction() { return RCEMSText.getCutAction(); }
    public Action getCopyAction() { return RCEMSText.getCopyAction(); }
    public Action getPasteAction() { return RCEMSText.getPasteAction(); }

    // Find? For a text field? Nuh-uh.
    public Action getFindAction() { return null; }

    public Object[] getRightClickActions(MouseEvent e) {
        return  RCEMSText.getRightClickActions(e);
    }
}
