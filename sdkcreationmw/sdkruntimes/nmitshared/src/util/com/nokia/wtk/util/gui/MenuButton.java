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



package com.nokia.wtk.util.gui;

/* java.beans */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/* java.io */
import java.io.IOException;
import java.io.InputStream;

/* java.awt.event */
import java.awt.event.ActionEvent;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.Action;
import javax.swing.JButton;
import javax.swing.KeyStroke;
import javax.swing.ImageIcon;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.AbstractAction;
import javax.swing.SwingUtilities;
import javax.swing.DefaultButtonModel;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.IOUtils;

/**
 * A button with a popup menu
 */
public class MenuButton extends JButton {

    private static final Icon UP_ARROW = getIcon("UpArrow.gif");
    private static final Icon DOWN_ARROW = getIcon("DownArrow.gif");

    // instance variables
    private Model model;
    private Action showMenu;
    private Action hideMenu;
    private JPopupMenu popup;

    /**
     * Creates a <code>MenuButton</code>
     */
    public MenuButton(String text) {
        this(text, text);
    }

    /**
     * Creates a <code>MenuButton</code>
     */
    public MenuButton(String up, String down) {
        setHorizontalTextPosition(JButton.LEFT);
        setDefaultCapable(false);
        setModel(model = new Model());

        showMenu = new ShowMenuAction(up);
        hideMenu = new HideMenuAction(down);
        popup = new JPopupMenu();
        popup.setInvoker(this);
        popup.addPropertyChangeListener(new PopupListener());

        setAction(showMenu);
    }

    /**
     * Returns the popup menu
     */
    public JPopupMenu getPopupMenu() {
        return popup;
    }

    /**
     * Overriden from super to set text in both up and down action
     * @param text The text to be set for both menu visible and invisible
     */
    public void setText(String text) {
        setText(text, text);
    }

    /**
     * Set the text for both menu showing and menu hidden states
     * @param up The text when the menu is hidden
     * @param down The text when the menu is showing
     */
    public void setText(String up, String down) {
        showMenu.putValue(Action.NAME, up);
        hideMenu.putValue(Action.NAME, down);
        super.setText(getPopupMenu().isShowing() ? down : up);
    }

    /**
     * Appends a new menu item to the end of the popup menu which
     * dispatches the specified <code>Action</code> object.
     *
     * @deprecated use addAction instead
     */
    public JMenuItem add(Action action) {
        return addAction(action);
    }

    /**
     * Appends a new menu item to the end of the popup menu which
     * dispatches the specified <code>Action</code> object.
     */
    public JMenuItem addAction(Action action) {
        return popup.add(new MenuItem(action));
    }

    /**
     * Appends a new separator at the end of the popup menu.
     */
    public void addSeparator() {
        popup.addSeparator();
    }

    /**
     * Updates the UI
     */
    public void updateUI() {
        super.updateUI();
        if (popup != null) {
            SwingUtilities.updateComponentTreeUI(popup);
        }
    }

    /**
     * Gets a resource icon
     * @param the name of the icon file
     */
    private static Icon getIcon(String name) {
        String fname = "resources/" + name;
        InputStream in = MenuButton.class.getResourceAsStream(fname);
        if (in != null) {
            try { return new ImageIcon(IOUtils.readStream(in)); }
            catch (IOException x) { /* ignore */ }
        }
        return null;

    }

    //======================================================================
    //      M O D E L
    //======================================================================

    private static class Model extends DefaultButtonModel {
        private boolean popupVisible = false;
        public boolean isPressed() {
            return (popupVisible || super.isPressed());
        }
        public boolean isArmed() {
            return (popupVisible || super.isArmed());
        }
        void setPopupVisible(boolean b) {
            if (popupVisible != b) {
                popupVisible = b;
                setArmed(popupVisible);
                setPressed(popupVisible);
            }
        }
    }

    //======================================================================
    //      M E N U    I T E M
    //======================================================================

    private static class MenuItem extends JMenuItem {
        MenuItem(Action action) {
            super(action);
        }
        protected void configurePropertiesFromAction(Action a) {
            setAccelerator((KeyStroke)a.getValue(Action.ACCELERATOR_KEY));
            super.configurePropertiesFromAction(a);
        }
        protected PropertyChangeListener createActionPropertyChangeListener(Action a) {
            return new PropertyListener(super.createActionPropertyChangeListener(a));
        }
        private class PropertyListener implements PropertyChangeListener {
            private PropertyChangeListener superListener;
            PropertyListener(PropertyChangeListener superListener) {
                this.superListener = superListener;
            }
            public void propertyChange(PropertyChangeEvent e) {
                if (e.getPropertyName().equals(Action.ACCELERATOR_KEY)) {
                    setAccelerator((KeyStroke)e.getNewValue());
                    repaint();
                } else {
                    superListener.propertyChange(e);
                }
            }
        }
    }

    //======================================================================
    //      P O P U P    L I S T E N E R
    //======================================================================

    private class PopupListener implements PropertyChangeListener {
        private boolean rollover = false;
        public void propertyChange(PropertyChangeEvent e) {
            if ("visible".equals(e.getPropertyName())) {
                if (Boolean.TRUE.equals(e.getNewValue())) {
                    rollover = RolloverJButton.cancelRollover(MenuButton.this);
                    setAction(hideMenu);
                    model.setPopupVisible(true);
                    setBorderPainted(true);
                } else {
                    setAction(showMenu);
                    model.setPopupVisible(false);
                    if (rollover) {
                       RolloverJButton.transformToRollover(MenuButton.this);
                    }
                }
            }
        }
    }

    //======================================================================
    //      A C T I O N S
    //======================================================================

    private class ShowMenuAction extends AbstractAction {
        ShowMenuAction(String text) { super(text, DOWN_ARROW); }
        public void actionPerformed(ActionEvent e) {
            int x;
            if (getHorizontalTextPosition() == JButton.LEFT) {
                x = 0;
            } else {
                x = getWidth() - popup.getPreferredSize().width;
            }
            popup.show(MenuButton.this, x, getHeight());
        }
    }

    private class HideMenuAction extends AbstractAction {
        HideMenuAction(String text) { super(text, UP_ARROW); }
        public void actionPerformed(ActionEvent e) {
            // PopupListener will fix the action
            popup.setVisible(false);
        }
    }
}
