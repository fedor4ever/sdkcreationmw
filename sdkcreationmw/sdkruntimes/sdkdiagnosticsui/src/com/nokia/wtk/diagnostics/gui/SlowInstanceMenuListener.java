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
import java.awt.KeyboardFocusManager;

import javax.swing.Action;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.MenuElement;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;

/**
 * A class to keep the similarities between EMSMenuListener and
 * SaveableMenuListener. These two have not been combined into one class
 * because of flexibility issues (I feel I may have to split the two groups
 * of menu entries into deparate menus soon) yet have a single place to
 * debug the code of these very similar classes.
 */
// This class is designed to be a listener to both JMenu and JPopupMenu, again
// for flexibility in the future As such it contains a helper interface to
// abstract away the difference between a JMenu and JPopupMenu for the purposes
// of this class, and classes to instantiate the interface. Nuthin sublcasses
// of this need worry about.
abstract class SlowInstanceMenuListener
    implements MenuListener, PopupMenuListener {

    static interface MenuContainer {
        public JMenuItem getMenuElementAt(int i);
        public int getNum();
    }

    static protected class NullAction extends Commands.EmptyAction {
            {
                setEnabled(false);
            }
        }

    public final static String NAME_PROPERTY_KEY = "namePropertyKey";
    private int count_ = 0;

    private final MenuContainer mc_;
    private final int size_;

    public SlowInstanceMenuListener(JMenu menu) {
        mc_ = new JMenuMenuContainer(menu);
        size_ = mc_.getNum();
    }
    public SlowInstanceMenuListener(JPopupMenu menu) {
        mc_ = new JPopupMenuMenuContainer(menu);
        size_ = mc_.getNum();

    }

    private JMenuItem checkItem(int i, String name) {
        JMenuItem ret;
        ret = mc_.getMenuElementAt(i);
        if ((ret != null)
            && (name.equals(ret.getClientProperty(NAME_PROPERTY_KEY)))) {
            return ret;
        }
        return null;
    }

    /**
     * This call will be called by this class just before the menu becomes
     * visible.
     * It is at this point that actual subclasses of this class must,
     * if appropriate, extract the salient actions from the argument
     * <code>inFocus</code> and replace all elements in the menu with
     * the appropriate keys.
     * @param inFocus
     */
    abstract void doWillBeVisible(Component inFocus);

    /**
     * Find and return the JMenuItem in the menu with client property value
     * <code>name</code< for the key <code>NAME_PROPERTY_KEY</code>.
     * @param name the name to look up
     * @return JMenuItem the JMenuItem with that name value, or null if not
     * found
     */
    private JMenuItem findItem(String name) {
        JMenuItem ret;
        count_++;
        if (count_ < size_) {
            ret = checkItem(count_, name);
            if (ret != null) {
                return ret;
            }
        }
        for (count_ = 0; count_ < size_; count_++) {
            ret = checkItem(count_, name);
            if (ret != null) {
                return ret;
            }
        }
        return null;
    }
    public void menuCanceled(MenuEvent e) {}
    public void menuDeselected(MenuEvent e) {}

    /**
     * Implements JMenuListener.menuSelected. This gets called just before
     * the menu this listener is attached to will become selected, and
     * in turn will call doWillBeVisible with the component that had
     * permanentFocus just before this menu was called.
     */
    public void menuSelected(MenuEvent fe) {
        Component inFocus =
            KeyboardFocusManager
                .getCurrentKeyboardFocusManager()
                .getPermanentFocusOwner();
        doWillBeVisible(inFocus);
    }
    public void popupMenuCanceled(PopupMenuEvent pme) {}
    public void popupMenuWillBecomeInvisible(PopupMenuEvent pe) {}

    /**
     * Implements JPopupMenuListener.popupMenuWillBecomeVisible. This gets
     * called just before
     * the menu this listener is attached to will become selected, and
     * in turn will call doWillBeVisible with the component that had
     * permanentFocus just before this menu was called.
     */
    public void popupMenuWillBecomeVisible(PopupMenuEvent pme) {
        Component inFocus =
            KeyboardFocusManager
                .getCurrentKeyboardFocusManager()
                .getPermanentFocusOwner();
        doWillBeVisible(inFocus);
    }

    /**
     * Switches the JMEnuItem in the menu with the client property
     * key = NAME_PROPERTY_KEY, value = name with Action a. If Action a
     * happens to be null, action defaultA is put in its place
     * @param name the value of the JMenuItem client property with key
     * NAME_PROPERTY_KEY. The JMenuItem with a cleint property with this
     * value is the one that will have its action replaced
     * @param a The action to put into the JMenuItem
     * @param defaultA a default action to use should a be null.
     */
    protected void switchAction(String name, Action a, Action defaultA) {
        JMenuItem mi = findItem(name);
        if (mi != null) {
            mi.setAction(a != null ? a : defaultA);
        }
    }

    private static class JMenuMenuContainer implements MenuContainer {
        final private JMenu menu_;

        JMenuMenuContainer(JMenu menu) {
            menu_ = menu;
        }

        public JMenuItem getMenuElementAt(int i) {
            return menu_.getItem(i);
        }

        public int getNum() {
            return menu_.getItemCount();
        }
    }

    private static class JPopupMenuMenuContainer implements MenuContainer {
        final private MenuElement[] menuElts_;

        JPopupMenuMenuContainer(JPopupMenu menu) {
            menuElts_ = menu.getSubElements();
        }

        public JMenuItem getMenuElementAt(int i) {
            Object o = menuElts_[i];
            return (o instanceof JMenuItem ? (JMenuItem)o : null);
        }

        public int getNum() {
            return menuElts_.length;
        }
    }
}
