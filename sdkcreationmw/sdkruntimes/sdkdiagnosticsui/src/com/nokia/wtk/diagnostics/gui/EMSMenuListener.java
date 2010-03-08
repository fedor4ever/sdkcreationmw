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
import java.awt.Component;

import javax.swing.Action;
import javax.swing.JMenu;
import javax.swing.JPopupMenu;

import com.nokia.wtk.app.ActionsUtilities;
import com.nokia.wtk.app.EditMenuSupport;

/**
 * <p>Title: </p>
 * <p>Description: A listener to attach to a JMenu that will replace the actions
 * with ids "undo", "redo", "cut", "copy", "paste", and "find" with the appropriate
 * actions of whatever UI element in the hierarchy that has the focus that is an
 * instance of EditMenuSupport. Attach this listener to the global Edit menu in the
 * window, and never worry about whether the right actions are available or enabled
 * again.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */

class EMSMenuListener extends SlowInstanceMenuListener {

    
    public static final String COPY_KEY = "copy";
    public static final String CUT_KEY = "cut";
    public static final String FIND_KEY = "find";
    public static final String PASTE_KEY = "paste";
    public static final String REDO_KEY = "redo";
    public static final String UNDO_KEY = "undo";
    private Action defaultCutAction,
        defaultCopyAction,
        defaultPasteAction,
        defaultUndoAction,
        defaultRedoAction,
        defaultFindAction;

    EMSMenuListener(JMenu menu) {
        super(menu);
        initDefaultActions();
    }
    EMSMenuListener(JPopupMenu menu) {
        super(menu);
        initDefaultActions();
    }

    void doWillBeVisible(Component inFocus) {
        boolean undoRedo = false;
        boolean cutCopyPaste = false;
        boolean found = false;
        Action cut, copy, paste, undo, redo, find;

        for (Component c = inFocus; c != null; c = c.getParent()) {
            if (c instanceof EditMenuSupport) {
                if (!undoRedo) {
                    undo = ((EditMenuSupport) c).getUndoAction();
                    redo = ((EditMenuSupport) c).getRedoAction();
                    if ((undo != null) || (redo != null)) {
                        undoRedo = true;
                        switchAction(UNDO_KEY, undo, defaultUndoAction);
                        switchAction(REDO_KEY, redo, defaultRedoAction);
                    }
                }
                if (!cutCopyPaste) {
                    cut = ((EditMenuSupport) c).getCutAction();
                    copy = ((EditMenuSupport) c).getCopyAction();
                    paste = ((EditMenuSupport) c).getPasteAction();
                    if ((cut != null) || (paste != null) || (copy != null)) {
                        cutCopyPaste = true;
                        switchAction(CUT_KEY, cut, defaultCutAction);
                        switchAction(COPY_KEY, copy, defaultCopyAction);
                        switchAction(PASTE_KEY, paste, defaultPasteAction);
                    }
                }
                if (!found) {
                    find = ((EditMenuSupport) c).getFindAction();
                    if (find != null) {
                        found = true;
                        switchAction(FIND_KEY, find, defaultFindAction);
                    }
                }
                if (found && undoRedo && cutCopyPaste) {
                    return;
                }
            }
        }
        // Now that the whole hierarchy has been traversed up, set the menu
        // actions that have not been found to the NullAction
        if (!undoRedo) {
            switchAction(UNDO_KEY, defaultUndoAction, defaultUndoAction);
            switchAction(REDO_KEY, defaultRedoAction, defaultRedoAction);
        }
        if (!cutCopyPaste) {
            switchAction(CUT_KEY, defaultCutAction, defaultCutAction);
            switchAction(COPY_KEY, defaultCopyAction, defaultCopyAction);
            switchAction(PASTE_KEY, defaultPasteAction, defaultPasteAction);
        }
        if (!found) {
            switchAction(FIND_KEY, defaultFindAction, defaultFindAction);
        }
    }

    void initDefaultActions() {

        defaultCutAction = new NullAction();
        defaultCopyAction = new NullAction();
        defaultPasteAction = new NullAction();
        defaultUndoAction = new NullAction();
        defaultRedoAction = new NullAction();
        defaultFindAction = new NullAction();

        ActionsUtilities.cutActionStyler(defaultCutAction);
        ActionsUtilities.copyActionStyler(defaultCopyAction);
        ActionsUtilities.pasteActionStyler(defaultPasteAction);
        ActionsUtilities.undoActionStyler(defaultUndoAction);
        ActionsUtilities.redoActionStyler(defaultRedoAction);
        ActionsUtilities.findActionStyler(defaultFindAction);
    }

}
