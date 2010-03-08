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

import java.awt.event.MouseEvent;

import javax.swing.Action;

import javax.swing.text.DefaultEditorKit;

/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:  Small class to encapsulate and re-use text cut/copy/paste facilities
 * Company:      Nokia
 * @version 1.0
 */


class RCEMSText {

    private static Action cut = new DefaultEditorKit.CutAction();
    private static Action copy = new DefaultEditorKit.CopyAction();
    private static Action paste = new DefaultEditorKit.PasteAction();

    private static Action[] actions = { cut, copy, paste };

    static {
        ActionsUtilities.cutActionStyler(cut);
        ActionsUtilities.copyActionStyler(copy);
        ActionsUtilities.pasteActionStyler(paste);
    }

    static Action getCutAction() { return cut; }
    static Action getCopyAction() { return copy; }
    static Action getPasteAction() { return paste; }

    static Object[] getRightClickActions(MouseEvent e) { return actions; }
}
