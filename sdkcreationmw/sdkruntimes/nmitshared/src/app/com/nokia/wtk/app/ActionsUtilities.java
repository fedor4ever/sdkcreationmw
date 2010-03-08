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

/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:  This class has a set of utility calls to manage common Action
 * calls in the system. Call this library of functions to get the perfect
 * look for those Cut/Copy?Paste actions, have the right keystrokes registered
 * automatically, and centralize interaction.
 * (LookFactory became too big and there were more calls
 * coming).
 * <p>
 * The tooltips, keystrokes, and action names are extremely generic. Specifc
 * parts of the application may wish to specialize the actions after they have
 * been made to look uniform here.
 * Company:      Nokia
 * @version 1.0
 */

import javax.swing.Action;
import javax.swing.JComponent;
import javax.swing.KeyStroke;

public class ActionsUtilities {

    static {
        AppEnvironment.addResources("resources/ActionsUtilities.properties",
                ActionsUtilities.class);
    }

    private final static String UNDO = "app.general.undo";
    private final static String REDO = "app.general.redo";
    private final static String COPY = "app.general.copy";
    private final static String CUT = "app.general.cut";
    private final static String PASTE = "app.general.paste";
    private final static String FIND = "app.general.find";
    private final static String FIND_NEXT = "app.general.find-next";
    private final static String NEW = "app.general.new";
    private final static String EDIT = "app.general.edit";
    private final static String SAVE = "app.general.save";
    private final static String SAVEAS = "app.general.saveAs";
    private final static String OPEN = "app.general.open";

    private final static String ICON = ".icon";
    private final static String TT = ".tooltip";
    private final static String NAME = ".text";
    private final static String KEYSTROKE = ".keystroke";
    private final static String MNEMONIC = ".mnemonic";

    /**
     * Load and make ready all standard icons and actions for this environment.
     */
    // Doesn't need to do anything, all work done in static initializer
    public static void init() {}

    public static void actionStyler(Action a, String attribu) {
        a.putValue(Action.SMALL_ICON, AppEnvironment.getIcon(attribu + ICON));
        a.putValue(Action.SHORT_DESCRIPTION, AppEnvironment.getString(attribu + TT));
        a.putValue(Action.NAME, AppEnvironment.getString(attribu + NAME));
        a.putValue(Action.ACCELERATOR_KEY, AppEnvironment.getKeyStroke(attribu + KEYSTROKE));
        String m = AppEnvironment.getString(attribu + MNEMONIC);
        if (m != null && m.length() > 0) {
        	a.putValue(Action.MNEMONIC_KEY, new Integer((int)m.charAt(0)));
        }
    }

    public static void undoActionStyler(Action a) {
        actionStyler(a, UNDO);
    }
    public static void redoActionStyler(Action a) {
        actionStyler(a, REDO);
    }
    public static void cutActionStyler(Action a) {
        actionStyler(a, CUT);
    }
    public static void copyActionStyler(Action a) {
        actionStyler(a, COPY);
    }
    public static void pasteActionStyler(Action a) {
        actionStyler(a, PASTE);
    }
    public static void findActionStyler(Action a) {
        actionStyler(a, FIND);
    }
    public static void findNextActionStyler(Action a) {
        actionStyler(a, FIND_NEXT);
    }
    public static void saveActionStyler(Action a) {
        actionStyler(a, SAVE);
    }
    public static void saveAsActionStyler(Action a) {
        actionStyler(a, SAVEAS);
    }
    public static void openActionStyler(Action a) {
        actionStyler(a, OPEN);
    }
    public static void newActionStyler(Action a) {
        actionStyler(a, NEW);
    }
    public static void editActionStyler(Action a) {
        actionStyler(a, EDIT);
    }


    /**
     * Register action with the input map. Depends on the Action.ACCELERATOR_KEY
     * property being set in the action.
     * @param comp the JComponent on which to register the action
     * @param action the Action to register
     * @param key the key to register the action with. This has to be a unique
     * string that identifies the action to the system.
     */
    static public void registerAction(JComponent comp, Action action, String key) {
        KeyStroke ks = (KeyStroke)action.getValue(Action.ACCELERATOR_KEY);
        if (ks != null) {
            comp.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(ks, key);
            comp.getActionMap().put(key, action);
        }
    }

    /**
     * Style actions and register the keystrokes.
     * @param ems the object implementing the EditMenuSupport interface. The
     * Action objects will be fetched and, if not null, styled appropriatly.
     * They will also be registered with the JComponent implementing the
     * EMS interface for keystrokes.
     */
    static public void manageEMS(EditMenuSupport ems) {
        try {
            manageEMS(ems, (JComponent) ems);
        } catch (ClassCastException cce) {
        }
    }

    /**
     * Style actions and register the keystrokes to the supplied component.
     * @param ems the object implementing the EditMenuSupport interface. The
     * Action objects will be fetched and, if not null, styled appropriatly.
     * @param comp the Component the actions will be registered to with
     * appropritae keystrokes.
     */
    static public void manageEMS(EditMenuSupport ems, JComponent comp) {
        Action at = ems.getCopyAction();
        if (at != null) {
            ActionsUtilities.copyActionStyler(at);
            registerAction(comp, at, "copy");
        }
        at = ems.getCutAction();
        if (at != null) {
            ActionsUtilities.cutActionStyler(at);
            registerAction(comp, at, "cut");
        }
        at = ems.getPasteAction();
        if (at != null) {
            ActionsUtilities.pasteActionStyler(at);
            registerAction(comp, at, "paste");
        }
        at = ems.getUndoAction();
        if (at != null) {
            ActionsUtilities.undoActionStyler(at);
            registerAction(comp, at, "undo");
        }
        at = ems.getRedoAction();
        if (at != null) {
            ActionsUtilities.redoActionStyler(at);
            registerAction(comp, at, "redo");
        }
        at = ems.getFindAction();
        if (at != null) {
            ActionsUtilities.findActionStyler(at);
            registerAction(comp, at, "find");
        }
        /*at = ems.getFindNextAction();
        if (at != null) {
            ActionsUtilities.findNextActionStyler(at);
            registerAction(comp, at, "findNext");
        }*/
    }

}
