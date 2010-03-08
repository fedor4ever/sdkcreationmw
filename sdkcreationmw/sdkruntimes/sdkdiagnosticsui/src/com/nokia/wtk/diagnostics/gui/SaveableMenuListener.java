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
import java.awt.event.ActionEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.JComponent;
import javax.swing.JMenu;
import javax.swing.JPopupMenu;

import com.nokia.wtk.app.ActionsUtilities;
import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.Saveable;
import com.nokia.wtk.util.FileFilterMIMEUtils;

/**
 * <p>Title: </p>
 * <p>Description: A listener to attach to a JMenu that will replace the actions
 * with ids "save" and "saveAs" with the appropriate
 * actions of whatever UI element in the hierarchy that has the focus that is an
 * instance of Saveable. Attach this listener to the global menu in the
 * window with the right id, and never worry about whether the right actions
 * are available or enabled again.</p>
 * <p>Company: Nokia</p>
 * @version 1.0
 */

// The structure of this file is much like EMSMenuListener. Problem is that
// Saveable does not define the actions to bind to a component. So they have
// to be defined here, and registered to a component. Not sure if I want to
// migrate that to ActionsUtilities just yet. I'd have to make
// SaveableMenuSupport public to use it with this menu listener and stuff.
// Maybe next iteration -- MICES could use this to simplify EditorCommands
public class SaveableMenuListener extends SlowInstanceMenuListener {
    /**
     * This interface described a SaveableMenuSupport, a set of actions that
     * together can be used for Save and Save As... in menus and such.
     */
    public interface SaveableMenuSupport {
        public Action getSaveAction();
        public Action getSaveAsAction();
    }

    public static final String SAVE_AS_KEY = "saveAs";
    public static final String SAVE_KEY = "save";

    final private static String SAVEABLE_MENU_SUPPORT_KEY =
        "saveableMenuSupportClientPropertyKey";

    /**
     * This call will take a Saveable, make Save and SaveAs actions out of it, and register
     * the result with the JComponent so the appropriate keyboard shortcuts work.
     * The actions will be styled and have icons, shortcuts, tooltips, etc
     * set according to the values in ActionsUtilities and AppEnvironment. It
     * can be used much like its official counterpart EditMenuSupport is
     * @param s the Saveable to turn into actions
     * @param c the JComponent to register the actions with with keystrokes
     * @return an object that implements SaveableMenuSupport, which contains
     * the registered actions.
     */
    public static SaveableMenuSupport register(Saveable s, JComponent c) {
        SaveableMenuSupport sms = new SaveableMenuSupportInstance(s);
        c.putClientProperty(SAVEABLE_MENU_SUPPORT_KEY, sms);
        Action a = sms.getSaveAction();
        ActionsUtilities.saveActionStyler(a);
        ActionsUtilities.registerAction(c, a, SAVE_KEY);
        a = sms.getSaveAsAction();
        ActionsUtilities.saveAsActionStyler(a);
        ActionsUtilities.registerAction(c, a, SAVE_AS_KEY);
        return sms;
    }

    private Action defaultSaveAction, defaultSaveAsAction;

    public SaveableMenuListener(JMenu menu) {
        super(menu);
        initMenu();
    }
    public SaveableMenuListener(JPopupMenu menu) {
        super(menu);
        initMenu();
    }



    void doWillBeVisible(Component inFocus) {
        Action save = null, saveAs = null;
        for (Component c = inFocus; c != null; c = c.getParent()) {
            if (c instanceof Saveable) {
                SaveableMenuSupport sms =
                    c instanceof JComponent
                        ? (SaveableMenuSupport)
                            ((JComponent) c).getClientProperty(
                            SAVEABLE_MENU_SUPPORT_KEY)
                        : null;
                if (sms != null) {
                    save = sms.getSaveAction();
                    saveAs = sms.getSaveAsAction();
                    break;
                }
            }
        }
        switchAction(SAVE_KEY, save, defaultSaveAction);
        switchAction(SAVE_AS_KEY, saveAs, defaultSaveAsAction);
    }
    private void initMenu() {
        defaultSaveAction = new NullAction();
        defaultSaveAsAction = new NullAction();

        ActionsUtilities.saveActionStyler(defaultSaveAction);
        ActionsUtilities.saveAsActionStyler(defaultSaveAsAction);
    }
}

class SaveableMenuSupportInstance
    implements SaveableMenuListener.SaveableMenuSupport {

    private final PropertyChangeListener enableSaveable_ =
        new PropertyChangeListener() {
        public void propertyChange(PropertyChangeEvent es) {
            if (es.getSource().equals(s_)) {
                getSaveAction().setEnabled(s_.isDirty());
                getSaveAsAction().setEnabled(s_.getContentType() != null);
            }
        }
    };

    private final Saveable s_;
    private Action saveAction_, saveAsAction_;

    public SaveableMenuSupportInstance(Saveable s) {
        s_ = s;
        s_.addPropertyChangeListener(enableSaveable_);
        enableSaveable_.propertyChange(
            new PropertyChangeEvent(s_, "", null, null));
    }

    public Action getSaveAction() {
        if (saveAction_ == null) {
            saveAction_ = new AbstractAction("save") {
                public void actionPerformed(ActionEvent ae) {
                    AppEnvironment
                        .getAppEnvironment()
                        .getFilingManager()
                        .doSaveFile(
                        s_,
                        FileFilterMIMEUtils.getFileFilter(s_.getContentType()),
                        FileFilterMIMEUtils.getCompatibleFileFilters(
                            s_.getContentType()));
                }
            };
        }
        return saveAction_;
    }

    public Action getSaveAsAction() {
        if (saveAsAction_ == null) {
            saveAsAction_ = new AbstractAction("saveAs") {
                public void actionPerformed(ActionEvent ae) {
                    AppEnvironment
                        .getAppEnvironment()
                        .getFilingManager()
                        .doSaveAsFile(
                        s_,
                        FileFilterMIMEUtils.getFileFilter(s_.getContentType()),
                        FileFilterMIMEUtils.getCompatibleFileFilters(
                            s_.getContentType().toString()));
                }
            };

        };
        return saveAsAction_;
    }

}
